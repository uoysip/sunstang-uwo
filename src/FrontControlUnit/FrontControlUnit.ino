/*Description: Code for the front lights and horns control unit
 *Version: 2
 *Contributors: Steven Lawrence 
 *Date: 02/12/19
 */
#include"FrontControlUnit.h"  
#include "mcp_can.h"                          // library for CAN communication
#include <SPI.h>                  // SPI library 

MCP_CAN CAN(kSPIpin);       
                   
void setup() {
  //Initialize Serial Monitor
  Serial.begin(9600);                  //Baudrate: 9600
  CAN.begin(MCP_STD, CAN_1000KBPS, MCP_16MHZ);

  pinMode(kFrontLeftLED, OUTPUT);            //Left turn signal output
  pinMode(kFrontRightLED, OUTPUT);          //Right turn signal output
  pinMode(headLight, OUTPUT);        //Headlight output
  pinMode(horn, OUTPUT);            //Horn output

  //initialize CAN masks
  CAN.init_Mask(0, 0, 0x7ff);                       //Mask 1: 0111 1111 1111
  CAN.init_Mask(1, 0, 0x7ff);                       //Mask 2: 0111 1111 1111

//initialize CAN filters
  CAN.init_Filt(0, 0, 0x30C);                       //Filter 1: 0011 0000 1100
  CAN.init_Filt(1, 0, 0x7D1);                       //Filter 2: 0111 1101 0001
  CAN.init_Filt(2, 0, 0x7D1);                       //Filter 3: 0111 1101 0001
  CAN.init_Filt(3, 0, 0x7D1);                       //Filter 4: 0111 1101 0001
  CAN.init_Filt(4, 0, 0x7D1);                       //Filter 5: 0111 1101 0001
  CAN.init_Filt(5, 0, 0x7D1);                //Filter 2: 0111 1101 0001
  
}

void loop (){

    if(CAN.hasMessage()){
    MCP_CAN::MessageFrame message;
    CAN.readMsgBuf(&message.canID, &message.dataLen, message.receiveBuffer);

    //if for all other lights than brakes 
    if(message.canID == FRONT_LHCU_ID){
        for (int i = 0; i < 5; i++) {
          frontLightsAndHornSignals[i] = message.receiveBuffer[i];
        }
      }
  }

    //check to see if turn indicator message is sent
  if(frontLightsAndHornSignals[3]&&!frontLightsAndHornSignals[1]){
    activateFrontLeftIndicator();
  }
  else if(frontLightsAndHornSignals[4] && !frontLightsAndHornSignals[1]){
  activateFrontRightIndicator();
  }
//checking to see if hazard lights are sent
  else if (frontLightsAndHornSignals[1]){
    activateFrontHazards();
  }
  //if not message involving the indicators are sent turn them off
  else turnOffFrontBlinkers();
  
  //head lights
  activateHeadLights(frontLightsAndHornSignals[0]);
        
  //Horns
  activateHorn(frontLightsAndHornSignals[2]);


}