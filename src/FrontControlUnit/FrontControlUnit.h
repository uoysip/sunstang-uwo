#ifndef FRONTCONTROLUNIT_H
#define FRONTCONTROLUNIT_H

#include "mcp_can.h"
#include <SPI.h>
//pins
const int kSPIpin = 10;                                                       
const int headLight = 4;                                                        //OUTPUT: Headlight LEDs MOSFET gate
const int horn = 5;
unsigned long FRONT_LHCU_ID = 0x30C; 
const int kFrontBlinkInterval = 500;
const int kFrontLeftLED = 2;                           // Left turn signal LEDs
const int kFrontRightLED = 3;   
unsigned long prevFrontLeftMillis(0), prevFrontRightMillis(0), prevFrontHazMillis(0);
bool leftFrontState(0), rightFrontState(0);
byte frontLightsAndHornSignals[8]; 
unsigned long canFrontID = 0;  
                                                    //OUTPUT: Horns MOSFET gate
class FrontControlUnit{
    public:
      void activateHorns (bool horn);
      void activateHeadLights(bool lights);
      void activateFrontLeftIndicator(void);
      void activateFrontRightIndicator(void);
      void activateFrontHazards(void);
      void turnOffFrontBlinkers(void);
};

#endif