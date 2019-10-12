#include "mcp_can.h"                          // library for CAN communication
#include <SPI.h>                              // SPI library 
#include "BackControlUnit.cpp"
MCP_CAN CAN(SPI_CS_PIN);

 
void setup() {
  Serial.begin(9600);
  pinMode(kLeftLED, OUTPUT);                              //Left turn signal output
  pinMode(kRightLED, OUTPUT);                             //Right turn signal output
  pinMode(reverseLight, OUTPUT);                         //Reverse light output
  pinMode(brakes, OUTPUT);                               //Brake output
  //Initialize CAN bus
  CAN.begin(CAN_1000KBPS); 
  //initialize CAN masks
  CAN.maskRange(0, 1, 0, 0x7ff);

  //initialize CAN filters

  CAN.init_Filt(0, 0, 0x30C);                       //Filter 1: 0011 0000 1100
  CAN.init_Filt(1, 0, 0x1F4);                               //Filter 2: 0001 1111 0100
  CAN.filterRange(2,5, 0, 0x7D1); 
}

void loop (){
  //reading message 
  if(CAN.hasMessage()){
    struct MessageFrame message = recieveMsg(CAN);
    //if for all other lights than brakes 
    if(message.canID == LHCU_ID){
        message.receiveBuffer.readBytes(LightsAndHornSignals, 8);
      }
  }
  else if(message.canID == Brake_ID){
    message.receiveBuffer.readBytes(Brake, 8);
  }
//check to see if turn indicator message is sent
if((LightsAndHornSignals[3] || LightsAndHornSignals[4])&&!LightsAndHornSignals[1]){
    activateBlinkers(LightsAndHornSignals[3], &prevMillis, kRightLED, kLeftLED, &rightState, &leftState);
  }
  //checking to see if hazard lights are sent
  else if (LightsAndHornSignals[1]){
    activateHazards(&prevHazMillis, kRightLED, kLeftLED, &rightState, &leftState);
  }
    //if not message involving the indicators are sent turn them off
  else turnOffBlinkers(kRightLED, kLeftLED ,&rightState, &leftState);
  //turning on/off brake lights or reverse lights  
  activateBrakeLights(Brake[1]);
  activateReverseLights(LightsAndHornSignals[5]);
}