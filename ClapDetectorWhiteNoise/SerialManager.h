#ifndef _SerialManager_h
#define _SerialManager_h

#include <Tympan_Library.h>

//Extern variables
extern Tympan myTympan;
extern bool enable_printCPUandMemory; //defined in main *.ino file

//Extern Functions
extern void setConfiguration(int);
extern void togglePrintMemoryAndCPU(void);
extern void setPrintMemoryAndCPU(bool);
extern void incrementInputGain(float);

//now, define the Serial Manager class
class SerialManager {
  public:
    SerialManager(
      AudioMixer4_F32 &_mixer_1, 
      AudioMixer4_F32 &_mixer_2) : mixer_1(_mixer_1), mixer_2(_mixer_2) { mask = true; respondToByte('m'); };

    void respondToByte(char c);
    void printHelp(void);

  private:
    AudioMixer4_F32 &mixer_1;
    AudioMixer4_F32 &mixer_2;
    bool mask;
};

void SerialManager::printHelp(void) {
  myTympan.println();
  myTympan.println("SerialManager Help: Available Commands:");
  myTympan.println("   h: Print this help");
  myTympan.println("   c/C: Enable/Disable printing of CPU and Memory usage");
  myTympan.println("   m: Toggle masking noise");
}

void SerialManager::respondToByte(char c) {
  switch(c) {
    case 'h': case '?':
      printHelp(); break;
    case 'c':      
      myTympan.println("Received: printing memory and CPU usage.");
      enable_printCPUandMemory = true;
      break;     
    case 'C':
      myTympan.println("Received: stopping printing memory and CPU usage.");
      enable_printCPUandMemory = false;
      break; 
    case 'm':
      myTympan.println("Received: toggling masking noise");
      if(mask) {
        mixer_1.gain(1, 0.f);
        mixer_2.gain(1, 0.f); 
        mask = false;
      } else {
        mixer_1.gain(1, 1.f);
        mixer_2.gain(1, 1.f);
        mask = true;
      }
  }
}

#endif
