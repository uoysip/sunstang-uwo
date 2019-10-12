#include<mcp_can.h>                         //library for CAN communication
#include<SPI.h>                                //SPI library
#include"DashboardControlUnit.cpp"
                 
MCP_CAN CAN(SPI_CS_PIN);                   //Initialize CAN

void setup() {
  Serial.begin(115200);                             //Initialize serial monitor: baudrate = 115200

  pinMode(brakes, INPUT);                            //brake signal input

  CAN.begin(CAN_1000KBPS); 
  
                       
  CAN.maskRange(0, 1, 0, 0x7ff);

  CAN.init_Filt(0, 0, MC_ID + 3);                    //Filter 1: 0011 0000 0011
  CAN.filterRange(1,5, 0, 0x7D1);                     //Filter 6: 0111 1101 0001
  
}

void loop() {

  //Set brake state to off
  void brakeSend(); 
  //Sending brake message
  void checkSend();

  //Receiving messages
  if(CAN.hasMessage()){
    struct MessageFrame message = recieveMsg(CAN); 
    message.receiveBuffer.readBytes(canID, 8);
    if (canID == MC_ID + 3) 
      message.receiveBuffer.readBytes(VelocityBuf, 8);
      
    }
  
  }

//Convert the volocity buffer into two separate intergers representing vehicle velocity (bytes 5-8) and motor RPM (bytes 0-4). Ask Nathan Dimla how to do this
}
