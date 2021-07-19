/*
*   BasicGain
*
*   Created: Chip Audette, Nov 2016
*   Purpose: Process audio using Tympan by applying gain.
*
*   Blue potentiometer adjusts the digital gain applied to the audio signal.
*
*   Uses default sample rate of 44100 Hz with Block Size of 128
*
*   MIT License.  use at your own risk.
*/

//here are the libraries that we need
#include <Tympan_Library.h>  //include the Tympan Library

//set the sample rate and block size
const float sample_rate_Hz = 44117.0f ; //24000 or 44117 (or other frequencies in the table in AudioOutputI2S_F32)
const int audio_block_samples = 128;     //do not make bigger than AUDIO_BLOCK_SAMPLES from AudioStream.h (which is 128)
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

//create audio library objects for handling the audio
Tympan                    myTympan(TympanRev::D, audio_settings);  //do TympanRev::D or TympanRev::C
AudioInputI2S_F32         i2s_in(audio_settings);        //Digital audio *from* the Tympan AIC.
//AudioSynthNoiseWhite_F32  noiseWhite1(audio_settings);
AudioConvert_F32toI16     convertF32toI16_1, convertF32toI16_2;
//AudioMixer4_F32           mixer4(audio_settings);
AudioOutputUSB            i2s_usb_out;
AudioOutputI2S_F32        i2s_out(audio_settings);       //Digital audio *to* the Tympan AIC.  Always list last to minimize latency

//Make all of the audio connections
AudioConnection_F32       patchCord1(i2s_in, 0, convertF32toI16_1, 0);    //connect the Left input
AudioConnection_F32       patchCord2(i2s_in, 1, convertF32toI16_2, 0);    //connect the Right input
AudioConnection           patchCord41(convertF32toI16_1, 0, i2s_usb_out, 0);  //left output ("0")
AudioConnection           patchCord42(convertF32toI16_2, 0, i2s_usb_out, 1);  //right output ("1")
//AudioConnection_F32       patchCord31(i2s_in, 0, mixer4, 0);
//AudioConnection_F32       patchCord32(noiseWhite1, 0, mixer4, 1);
AudioConnection_F32       patchCord11(i2s_in, 0, i2s_out, 0);  //connect the Left gain to the Left output
AudioConnection_F32       patchCord12(i2s_in, 0, i2s_out, 1);  //connect the Right gain to the Right output

// define the setup() function, the function that is called once when the device is booting
const float input_gain_dB = 20.0f; //gain on the microphone
float vol_knob_gain_dB = 0.0;      //will be overridden by volume knob
void setup() {

  //begin the serial comms (for debugging)
  Serial.begin(115200);  delay(500);
  Serial.println("BasicGain: starting setup()...");

  //allocate the dynamic memory for audio processing blocks
  AudioMemory(20); AudioMemory_F32(40); 
///  noiseWhite1.amplitude(0.0125);

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
  while (Serial.available()) Serial.print((char)Serial.read());   //USB

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

      //choose the desired gain value based on the knob setting
      const float min_gain_dB = -20.0, max_gain_dB = 40.0; //set desired gain range
      vol_knob_gain_dB = min_gain_dB + (max_gain_dB - min_gain_dB)*val; //computed desired gain value in dB

      //command the new gain setting
      myTympan.setInputGain_dB(vol_knob_gain_dB);  
      Serial.print("servicePotentiometer: Digital Gain dB = "); Serial.println(vol_knob_gain_dB); //print text to Serial port for debugging
    }
    lastUpdate_millis = curTime_millis;
  } // end if
} //end servicePotentiometer();
