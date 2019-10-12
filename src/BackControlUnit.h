#ifndef BACKCONTROLUNIT_H
#define BACKCONTROLUNIT_H

#include "GeneralLightControlUnit.h"
//pins
const int kLeftLED = 4;                           // Left turn signal LEDs
const int kRightLED = 3;   
const int reverseLight = 2;                    // Backup LEDs
const int brakes = 5;       
unsigned long Brake_ID = 0x1F4;
unsigned long prevMillis(0), prevHazMillis(0);
bool leftState(0), rightState(0);
byte LightsAndHornSignals[8]; 
byte Brake[8]; 
unsigned long canID = 0;  
                                                    //OUTPUT: Horns MOSFET gate
class BackControlUnit : public GeneralLightControlUnit {
    public:
    
    protected:
        activateBrakeLights (bool brake);
        activateReverseLights(bool lights);
};

#endif