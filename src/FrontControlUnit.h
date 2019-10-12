#ifndef FRONTCONTROLUNIT_H
#define FRONTCONTROLUNIT_H

#include "GeneralLightControlUnit.h"
//pins
const int kLeftLED = 2;                                                          //OUTPUT: Left turn LEDs MOSFET gate
const int kRightLED = 3;                                                        
const int headLight = 4;                                                        //OUTPUT: Headlight LEDs MOSFET gate
const int horn = 5;
unsigned long prevLeftMillis(0), prevRightMillis(0), prevHazMillis(0);
bool leftState(0), rightState(0);
byte LightsAndHornSignals[8];                                                              //OUTPUT: Horns MOSFET gate
unsigned long canID = 0; 

                        

class FrontControlUnit : public GeneralLightControlUnit {
  protected:
    activateHorns (bool horn);
    activateHeadLights(bool lights);
};
#endif