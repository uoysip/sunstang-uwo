#ifndef GENERALLIGHTCONTROLUNIT_H
#define GENERALLIGHTCONTROLUNIT_H

#include "GeneralControlUnit.h"
//pins
const int SPI_CS_PIN = 10; 
//can stuff
unsigned long LHCU_ID = 0x30C; 
const int kBlinkInterval = 500;

                   

//Setup variables for receiving CAN messages

class GeneralLightControlUnit : public GeneralControlUnit {
  protected:
    activateBlinkers (bool blinker, unsigned long *prevMillis, bool *rState, bool *lState);
    flashBlinkers (unsigned long *prevMillis, int LED, bool *state);
    activateHazards(unsigned long *prevMillis, int rLED, int lLED, bool *rState, bool *lState );
    turnOffBlinkers(int rLED, int lLED,bool *rState, bool *lState);
};

