/*Description: Code for the front lights and horns control unit
 *Version: 2
 *Contributors: Steven Lawrence 
 *Date: 02/12/19
 */
#include "FrontControlUnit.cpp"                                                                

MCP_CAN CAN(SPI_CS_PIN);                                              
void setup() {
  //Initialize Serial Monitor
  Serial.begin(9600);                  //Baudrate: 9600
  pinMode(leftLED, OUTPUT);            //Left turn signal output
  pinMode(rightLED, OUTPUT);          //Right turn signal output
  pinMode(headLight, OUTPUT);        //Headlight output
  pinMode(horn, OUTPUT);            //Horn output

  //Initialize CAN bus
  CAN.begin(CAN_1000KBPS);             //Initialize CAN bus; Baudrate = 1000k
    
  //pinmodes 
  
//initialize CAN masks
  CAN.maskRange(0, 1, 0, 0x7ff);
  
//initialize CAN filters
  
  CAN.init_Filt(0, 0, 0x30C);                       //Filter 1: 0011 0000 1100
  CAN.filterRange(1,5, 0, 0x7D1);                   //Filter 6: 0111 1101 0001
}

void loop (){
    
  //Receive message
  if(CAN.hasMessage()){
    struct MessageFrame message = recieveMsg(CAN);
    if(message.canID == LHCU_ID){
        message.receiveBuffer.readBytes(LightsAndHornSignals, 8);
      }
  }
    //check to see if turn indicator message is sent
    if((LightsAndHornSignals[3] || LightsAndHornSignals[4])&&!LightsAndHornSignals[1]){
      activateBlinkers(LightsAndHornSignals[3], kRightLED, kLeftLED, &rightState, &leftState)
    }
    //checking to see if hazard lights are sent
    else if (LightsAndHornSignals[1]){
      activateHazards(&prevHazMillis, kRightLED, kLeftLED, &rightState, &leftState );
    }
    //if not message involving the indicators are sent turn them off
    else turnOffBlinkers(kRightLED, kLeftLED ,&rightState, &leftState);
    
    //head lights
    activateHeadLights(lightsAndHornSignals[0]);
          
    //Horns
     activateHorn(LightsAndHornSignals[2])
  

}