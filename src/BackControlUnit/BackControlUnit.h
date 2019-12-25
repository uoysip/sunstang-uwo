#ifndef BACKCONTROLUNIT_H
#define BACKCONTROLUNIT_H

#include "mcp_can.h"
#include <SPI.h>
//pins
const int BACK_SPI_CS_PIN = 10; 
//can stuff
unsigned long BACK_LHCU_ID = 0x30C; 
const int kBackBlinkInterval = 500;
const int kBackLeftLED = 4;                           // Left turn signal LEDs
const int kBackRightLED = 3;   
const int reverseLight = 2;                    // Backup LEDs
const int brakes = 5;       
unsigned long Brake_ID = 0x1F4;
unsigned long prevBackLeftMillis(0), prevBackRightMillis(0), prevBackHazMillis(0);
bool leftBackState(0), rightBackState(0);
byte backLightsAndHornSignals[8]; 
byte Brake[8]; 
unsigned long canBackID = 0;  
                                                    //OUTPUT: Horns MOSFET gate
class BackControlUnit{
    public:
      void activateBrakeLights (bool brake);
      void activateReverseLights(bool lights);
      void activateBackLeftIndicator();
      void activateBackRightIndicator();
      void activateBackHazards();
      void turnOffBackBlinkers();
};

#endif