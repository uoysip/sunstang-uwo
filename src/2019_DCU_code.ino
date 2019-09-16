#include<mcp_can.h>                         //library for CAN communication
#include<SPI.h>                                //SPI library

const int SPI_CS_PIN = 10;                      //CAN chip select pin
MCP_CAN CAN(SPI_CS_PIN);                   //Initialize CAN

unsigned long Brake_ID = 0x1F4;            //Brake message ID
unsigned long MC_ID = 0x300;               //Motor controller ID

byte BrakeSend[8];                              //Brake transmitting message buffer
byte RelaySend[8];                              //Secondary relay control transmitting message buffer

byte VelocityBuf[8];                            //Velocity recieve buffer from motor controller

const int brakes = 2;                            //Brake lever switch pin

unsigned long brakeTime = 0;               //Last time brake message was sent

int vehicleVelocity = 0;                        //Vehicle velocity 
int motorRPM = 0;                               //Motor RPM

void setup() {
  Serial.begin(115200);                             //Initialize serial monitor: baudrate = 115200

  pinMode(brakes, INPUT);                            //brake signal input

  START_INIT:
  if (CAN_OK == CAN.begin(CAN_500KBPS)) {             //Initialize CAN bus: baudrate = 500k
    Serial.println("CAN BUS Shield init ok!");  
  } else {
    Serial.println("CAN BUS Shield init fail"); 
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;                            
  }
                       
  CAN.init_Mask(0,0,0x7ff);                          //Mask 1: 0111 1111 1111
  CAN.init_Mask(1,0,0x7ff);                          //Mask 2: 0111 1111 1111

  CAN.init_Filt(0, 0, MC_ID + 3);                    //Filter 1: 0011 0000 0011
  CAN.init_Filt(1, 0, 0x7D1);                         //Filter 2: 0111 1101 0001
  CAN.init_Filt(2, 0, 0x7D1);                         //Filter 3: 0111 1101 0001
  CAN.init_Filt(3, 0, 0x7D1);                         //Filter 4: 0111 1101 0001
  CAN.init_Filt(4, 0, 0x7D1);                         //Filter 5: 0111 1101 0001
  CAN.init_Filt(5, 0, 0x7D1);                         //Filter 6: 0111 1101 0001
}

void loop() {
  //Setup variables for receiving CAN messages
  unsigned char lenReceive = 0;
  unsigned char bufReceive[8];
  unsigned char canID = 0;
  
  //Reading inputs
  if (digitalRead(brakes)) {                          //Check if brakes are active
    BrakeSend[0] = true;                              //Set brake state to on
  } 
  else 
    BrakeSend[0] = false;                             //Set brake state to off

//Sending brake message
  if ((millis() - brakeTime) > 50) {                        //Check if enough time has passed since last brake message
    brakeTime = millis();                             //Save the last time brake message was sent
    CAN.sendMsgBuf(Brake_ID, 0, 8, BrakeSend);      //Send Brake message
  }

//Receiving messages
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&lenReceive, bufReceive);
    canID = CAN.getCanId();

    Serial.print("Sensor Value from ID: ");         //Print the ID of the CAN message
    Serial.print(canID);
    Serial.print(" = ");
    
    for (int i = 0; i < lenReceive; i++){           //Print the data from the CAN message
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }
    Serial.println();

//Processing received messages
    if (canID = MC_ID + 3) {                              //Check if message id is motor velocity
      for (unsigned i = 0; i < 8; i++){
        VelocityBuf[i] = bufReceive[i];               //Load receive buffer into velocity buffer
      }
    }
  }

//Convert the volocity buffer into two separate intergers representing vehicle velocity (bytes 5-8) and motor RPM (bytes 0-4). Ask Nathan Dimla how to do this
}
