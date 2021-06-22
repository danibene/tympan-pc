/*
*   LowPass Filtering with USB Output 
*
*   Created: Nithin Raj, June 2021
*   Purpose: Process audio using Tympan by applying low pass filter and transmitting filtered audio to an external computer.
*
*   Blue potentiometer adjusts the digital gain/cutoff frequency applied to the audio signal.
*
*   MIT License.  use at your own risk.
*/

//here are the libraries that we need
//#include <Audio.h>/
#include <Tympan_Library.h>  //include the Tympan Library
#include "AudioEffectLowpassFD_F32.h"  //the local file holding your custom function

//set the sample rate and block size
const float sample_rate_Hz = 44117.0f ; //24000 or 44117 (or other frequencies in the table in AudioOutputI2S_F32)
const int audio_block_samples = 128;     //do not make bigger than AUDIO_BLOCK_SAMPLES from AudioStream.h (which is 128)
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

//create audio library objects for handling the audio
Tympan                    myTympan(TympanRev::D);  //do TympanRev::D or TympanRev::C
AudioInputI2S_F32         i2s_in(audio_settings);        //Digital audio *from* the Tympan AIC.
AudioEffectLowpassFD_F32  audioEffectLowpassFD_1(audio_settings);  //create the frequency-domain processing block
AudioEffectLowpassFD_F32  audioEffectLowpassFD_2(audio_settings);  //create the frequency-domain processing block
AudioConvert_F32toI16     convertF32toI16_1, convertF32toI16_2;
AudioOutputUSB            i2s_usb_out;
AudioOutputI2S_F32        i2s_out(audio_settings);       //Digital audio *to* the Tympan AIC.  Always list last to minimize latency

//Make all of the audio connections
AudioConnection_F32       patchCord1(i2s_in, 0, audioEffectLowpassFD_1, 0);    //connect the Left input
AudioConnection_F32       patchCord2(i2s_in, 1, audioEffectLowpassFD_2, 0);    //connect the Right input
AudioConnection_F32       patchCord3(audioEffectLowpassFD_1, 0, convertF32toI16_1, 0);  //convert to I16...next audio connection must be a plain AudioConnection
AudioConnection_F32       patchCord4(audioEffectLowpassFD_2, 0, convertF32toI16_2, 0);  //convert to I16...next audio connection must be a plain AudioConnection
AudioConnection           patchCord41(convertF32toI16_1, 0, i2s_usb_out, 0);  //left output ("0")
AudioConnection           patchCord42(convertF32toI16_2, 0, i2s_usb_out, 1);  //right output ("1")

AudioConnection_F32       patchCord11(audioEffectLowpassFD_1, 0, i2s_out, 0);  //connect the Left gain to the Left output
AudioConnection_F32       patchCord12(audioEffectLowpassFD_2, 0, i2s_out, 1);  //connect the Right gain to the Right output


// define the setup() function, the function that is called once when the device is booting
float input_gain_dB = 20.0f; //gain on the microphone
//float vol_knob_gain_dB = 0.0;      //will be overridden by volume knob/
void setup() {
  //begin the serial comms (for debugging)
  Serial.begin(115200);  delay(500);
  Serial.println("BasicGain: starting setup()...");
  Serial.print("  Sample Rate (Hz): "); Serial.println(audio_settings.sample_rate_Hz);
  Serial.print("  Audio Block Size (samples): "); Serial.println(audio_settings.audio_block_samples);

  //allocate the dynamic memory for audio processing blocks
  AudioMemory(20); AudioMemory_F32(40, audio_settings); 
  int N_FFT = 512;
  audioEffectLowpassFD_1.setup(audio_settings,N_FFT); //do after AudioMemory_F32();
  audioEffectLowpassFD_2.setup(audio_settings,N_FFT); //do after AudioMemory_F32();

  //Enable the Tympan to start the audio flowing!
  myTympan.enable(); // activate the Tympan's audio module

  //Choose the desired input
  myTympan.inputSelect(TYMPAN_INPUT_ON_BOARD_MIC);     // use the on board microphones
  //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_MIC);    // use the microphone jack - defaults to mic bias 2.5V
  //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_LINEIN); // use the microphone jack - defaults to mic bias OFF

  //Set the desired volume levels
  myTympan.volume_dB(0);                   // headphone amplifier.  -63.6 to +24 dB in 0.5dB steps.
  myTympan.setInputGain_dB(input_gain_dB); // set input volume, 0-47.5dB in 0.5dB setps

  // check the volume knob
  servicePotentiometer(millis(),0);  //the "0" is not relevant here.

  Serial.println("Setup complete.");
} //end setup()


// define the loop() function, the function that is repeated over and over for the life of the device
void loop() {

  //check the potentiometer
  servicePotentiometer(millis(),100); //service the potentiometer every 100 msec

  myTympan.printCPUandMemory(millis(),3000); //print every 3000 msec

} //end loop();


// ///////////////// Servicing routines

//servicePotentiometer: listens to the blue potentiometer and sends the new pot value
//  to the audio processing algorithm as a control parameter
void servicePotentiometer(unsigned long curTime_millis, unsigned long updatePeriod_millis) {
  //static unsigned long updatePeriod_millis = 100; //how many milliseconds between updating the potentiometer reading?
  static unsigned long lastUpdate_millis = 0;
  static float prev_val = -1.0;

  //has enough time passed to update everything?
  if (curTime_millis < lastUpdate_millis) lastUpdate_millis = 0; //handle wrap-around of the clock
  if ((curTime_millis - lastUpdate_millis) > updatePeriod_millis) { //is it time to update the user interface?

    //read potentiometer
	float val = float(myTympan.readPotentiometer()) / 1023.0; //0.0 to 1.0
    val = (1.0/9.0) * (float)((int)(9.0 * val + 0.5)); //quantize so that it doesn't chatter...0 to 1.0

    //send the potentiometer value to your algorithm as a control parameter
    if (abs(val - prev_val) > 0.05) { //is it different than before?
      prev_val = val;  //save the value for comparison for the next time around

      #if 0
        //use the potentiometer as a volume knob
        const float min_val = -20.0, max_val = 40.0; //set desired range
        float new_value = min_val + (max_val - min_val)*val;
        input_gain_dB = new_value;
        myTympan.setInputGain_dB(input_gain_dB); // set input volume, 0-47.5dB in 0.5dB setps
        Serial.print("servicePotentiometer: Input Gain (dB) = "); Serial.println(new_value); //print text to Serial port for debugging
      #else
        //use the potentiometer to set the freq-domain low-pass filter
        const float min_val = logf(200.f), max_val = logf(12000.f); //set desired range
        float lowpass_Hz = expf(min_val + (max_val - min_val)*val);
        audioEffectLowpassFD_1.setLowpassFreq_Hz(lowpass_Hz);
        audioEffectLowpassFD_2.setLowpassFreq_Hz(lowpass_Hz);
        Serial.print("servicePotentiometer: Lowpass (Hz) = "); Serial.println(lowpass_Hz); //print text to Serial port for debugging
      #endif
    }
    lastUpdate_millis = curTime_millis;
  } // end if
} //end servicePotentiometer();
