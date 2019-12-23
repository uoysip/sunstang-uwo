                           // SPI library 
#include "BackControlUnit.h"
MCP_CAN CAN(BACK_SPI_CS_PIN);

 
void setup() {
  Serial.begin(9600);
  CAN.begin(MCP_STD ,CAN_1000KBPS, MCP_16MHZ); 
  pinMode(kBackLeftLED, OUTPUT);                              //Left turn signal output
  pinMode(kBackRightLED, OUTPUT);                             //Right turn signal output
  pinMode(reverseLight, OUTPUT);                         //Reverse light output
  pinMode(brakes, OUTPUT);                               //Brake output
  //Initialize CAN bus
  
  //initialize CAN masks
  CAN.init_Mask(0, 0, 0x7ff);                       //Mask 1: 0111 1111 1111
  CAN.init_Mask(1, 0, 0x7ff);

  //initialize CAN filters

  CAN.init_Filt(0, 0, 0x30C);                       //Filter 1: 0011 0000 1100
  CAN.init_Filt(1, 0, 0x1F4);                               //Filter 2: 0001 1111 0100
  CAN.init_Filt(2, 0, 0x7D1);                       //Filter 3: 0111 1101 0001
  CAN.init_Filt(3, 0, 0x7D1);                       //Filter 4: 0111 1101 0001
  CAN.init_Filt(4, 0, 0x7D1);                       //Filter 5: 0111 1101 0001
  CAN.init_Filt(5, 0, 0x7D1);                //Filter 2: 0111 1101 0001
}

void loop (){
  /*/reading message 
  if(CAN.hasMessage()){
    MCP::MessageFrame message;
    CAN.readMsgBuf(&message.canID, &message.datalen, message.receiveBuffer)
    //if for all other lights than brakes 
    if(message.canID == BACK_LHCU_ID){
        message.receiveBuffer.readBytes(backLightsAndHornSignals, 8);
      }
  }
  else if(message.canID == Brake_ID){
    message.receiveBuffer.readBytes(Brake, 8);
  }*/
//check to see if turn indicator message is sent
  if(backLightsAndHornSignals[3]&&!backLightsAndHornSignals[1]){
    activateBackLeftIndicator();
  }
  else if(backLightsAndHornSignals[4]&&!backLightsAndHornSignals[1]){
    activateBackRightIndicator();
  }
  //checking to see if hazard lights are sent
  else if(backLightsAndHornSignals[1]){
    activateBackHazards();
  }
    //if not message involving the indicators are sent turn them off
  else turnOffBackBlinkers();
  //turning on/off brake lights or reverse lights  
  activateBrakeLights(Brake[1]);
  activateReverseLights(backLightsAndHornSignals[5]);
}