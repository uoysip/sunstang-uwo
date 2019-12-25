#ifndef STEERINGCONTROLUNIT_H
#define STEERINGCONTROLUNIT_H

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "mcp_can.h"
#include <SPI.h>

float motorDrive[2];
float motorPower[2];
byte LHCUsend[6];

byte Brakes[8];
byte VelocityBuf[8];

const int kSPIpin = 10;
const unsigned long kMCid = 0x300;
const unsigned long kDriverid = 0x200;
const unsigned long kLHCUid = 0x30c;
const unsigned long kBrakeid = 0x1F4;
const unsigned long kMotorid = kMCid + 3;

// Lights and Horn inputs
const int kLeftTurn = 4;               //INPUT: Left turn signal switch
const int kRightTurn = 3;              //INPUT: Right turn signal switch
const int kHeadLightToggle = 7;        //INPUT: Headlight toggle button
const int kHazardLightToggle = 2;      //INPUT: Hazard light toggle button
const int kHorn = 8;                   //INPUT: Horn button
const int kReverse = A0;               //INPUT: Reverse Switch 

// Motor Control inputs
const int kCruiseToggle = 6;           //INPUT: Cruise control on/off toggle button
const int kCruiseSpeedUp = 9;          //INPUT: Increase cruise control speed button
const int kCruiseSpeedDown = 5;        //INPUT: Decrease cruise control speed button
const int kAcceleration = A5;          //INPUT: Acceleration potentiometer
const int kRegenBrakeToggle = A1;      //INPUT: Regen Braking On/Off

// TODO: maybe put the use of these in an "#if DEBUG_MODE" ?
// Test LED outputs
const int kLED1 = A2;              //OUTPUT: Test LED #1
const int kLED2 = A3;              //OUTPUT: Test LED #2
const int kLED3 = A4;              //OUTPUT: Test LED #3

//time variables
uint32_t tNow = 0;                    //Current time
uint32_t tPrev = 0;                   //Previous time
unsigned long CAN_Millis=0;           //CAN message time
//unsigned long serialTime=0;         //Delay length for serial monitor printing

//Lights and Horns variables
bool hazardState(0), hornState, leftState, rightState;                                 //LHCU Current states
bool xHazardState(LOW), xHornState(LOW), xLeftState(LOW), xRightState(LOW);            //LHCU Previous states
bool headLightSig(0), hazardSig(0), hornSig(0), leftSig(0), rightSig(0), revSig(0);    //LHCU signals
long hazardDebounce(0), hornDebounce(0), leftDebounce(0), rightDebounce(0);            //LHCU button debounces
bool lastLeft(0), lastRight(0);

//Motor Controller variables
bool cruiseOnOffState(0), cruiseIncSpeedState, cruiseDecSpeedState;                     //MCU Current states
bool xCruiseOnOffState(LOW), xCruiseIncSpeedState(LOW), xCruiseDecSpeedState(LOW);      //MCU previous states
bool cruiseOnOffSig(0), cruiseIncSpeedSig, cruiseDecSpeedSig, regenBrakeSig(0);         //MCU signals
long cruiseOnOffDebounce(0), cruiseIncSpeedDebounce(0), cruiseDecSpeedDebounce(0);      //MCU button debounces

//SCU potentiometer variables
int accRaw;                     //Acceleration potentiometer input
int accPotMin(560);             //Minimum acceleration value
int accPotMax(955);             //Maximum acceleration value
float accFloat;                 //Acceleration value

long debounceDelay = 50;        //button debounce delay

int currVehVel = 0;             //Current vehicle velocity 
int currMotorRPM = 0;           //Current motor RPM

int vehicleVelocity = 0;        //Vehicle velocity for cruise control 

class SteeringControlUnit {
  private:
  protected:
  public:
    //! There shouldnt be ANY parameters, because the values are already stored on this object
    //! what should happen is i should instantiate the SteeringControlUniit object in the sketch and use/set/get the values from the object..
    //! would need to make getters and setters for most of the variables
    void toggleReadButton(int hazardLightToggle, bool xHazardState, bool hazardState, bool hazardSig, long hazardDebounce);
    void readButton(int kHorn, bool xHornState, bool hornState, bool hornSig, long hornDebounce);
    void readPotentiometer(int kAcceleration, int accRaw, float accFloat, int accPotMin, int accPotMax);
    
}

#endif