 /*Description: Code for the front lights and horns control unit
 *Version: 2
 *Contributors: Steven Lawrence 
 *Date: 02/12/19
 */
 
//include libraries
#include "mcp_can.h"                                                            //library for CAN communication
#include <SPI.h>                                                                //SPI library 

//pin assignments
const int leftLED = 2;                                                          //OUTPUT: Left turn LEDs MOSFET gate
const int rightLED = 3;                                                         //OUTPUT: Right turn LEDs MOSFET gate
const int headLight = 4;                                                        //OUTPUT: Headlight LEDs MOSFET gate
const int horn = 5;                                                             //OUTPUT: Horns MOSFET gate
const int SPI_CS_PIN = 10;                                                      //OUTPUT: CAN chip select pin

MCP_CAN CAN(SPI_CS_PIN);                                                        //Init CAN object 

//CAN IDs
unsigned long LHCU_ID=0x30C;                                                    //Lights and Horns signal ID

//CAN recieve buffers
byte LightsAndHornSignals[8];                                                   //Message buffer for turn signals, headlights and horns from SCU

//timing variables
int blinkInterval = 500;
unsigned long prevLeftMillis(0), prevRightMillis(0), prevHazMillis(0);

bool leftState(0), rightState(0);                                               //turn signal states

void setup() {
//Initialize Serial Monitor
  Serial.begin(9600);                           //Baudrate: 9600

//Initialize CAN bus
  CAN.begin(CAN_500KBPS);                         //Initialize CAN bus; Baudrate = 500k
    
//pinmodes 
  pinMode(leftLED, OUTPUT);                       //Left turn signal output
  pinMode(rightLED, OUTPUT);                      //Right turn signal output
  pinMode(headLight, OUTPUT);                     //Headlight output
  pinMode(horn, OUTPUT);                          //Horn output

//initialize CAN masks
  CAN.init_Mask(0,0,0x7ff);                       //Mask 1: 0111 1111 1111
  CAN.init_Mask(1,0,0x7ff);                       //Mask 2: 0111 1111 1111

//initialize CAN filters
  CAN.init_Filt(0,0,0x30C);                       //Filter 1: 0011 0000 1100
  CAN.init_Filt(1,0,0x7D1);                       //Filter 2: 0111 1101 0001
  CAN.init_Filt(2,0,0x7D1);                       //Filter 3: 0111 1101 0001
  CAN.init_Filt(3,0,0x7D1);                       //Filter 4: 0111 1101 0001
  CAN.init_Filt(4,0,0x7D1);                       //Filter 5: 0111 1101 0001
  CAN.init_Filt(5,0,0x7D1);                       //Filter 6: 0111 1101 0001
}

void loop() {
//Setup variables for receiving CAN messages
  unsigned char lenReceive = 0;                   //data length
  unsigned char bufReceive[8];                    //receive data buffer
  unsigned long canID=0;                          //CAN ID

//Receive message
  if(CAN_MSGAVAIL == CAN.checkReceive()){         //Check if anything is in the receive buffer
    CAN.readMsgBuf(&lenReceive, bufReceive);      //Read data,  lenReceive: data length, bufReceive: data buffer
    canID = CAN.getCanId();                       //Read the message id associated with this CAN message (also called a CAN frame)
    
    /*Serial.print("Sensor Value from ID: ");       //Print the ID of the CAN message
    Serial.print(canID);
    Serial.print(" = ");
    
    for (int i = 0; i < lenReceive; i++){         //Print the data from the CAN message
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }Serial.println();*/

//Processing received messages
    if(canID == LHCU_ID){                         //Check if message is from SCU
      //Serial.println("Lights and Horns"); 
      for(unsigned i=0; i<8; i++){
        LightsAndHornSignals[i]=bufReceive[i];    //Load receive buffer into lights and horns buffer
      }
    }
  }

 /*Serial.print("Lights and Horns Buf: ");
    for(int i=0; i<5;i++){
      Serial.print(LightsAndHornSignals[i]);
    }Serial.println();*/

//Hazards and turn signals
   if(LightsAndHornSignals[3]&&!LightsAndHornSignals[1]){             //Check if left turn signal is sent and hazard is not 
    //Serial.println("LEFT");
    digitalWrite(rightLED,LOW);                                       //Turn off right turn LED
       if(millis()-prevLeftMillis>=blinkInterval){                    //Check if enough time has passed since last blink
        prevLeftMillis = millis();                                    //Save the last time you blinked the LED
        if (leftState==0){                                            //If the left LED is off, turn it on and vice-versa
          digitalWrite(leftLED, HIGH);
          leftState=1;
        }
        else {
          digitalWrite(leftLED, LOW);
          leftState=0;
      }
     }
    }
    else if (LightsAndHornSignals[4]&&!LightsAndHornSignals[1]){      //Check if right turn signal is sent and hazard signal is not
      //Serial.println("RIGHT");
      digitalWrite(leftLED,LOW);                                      //Turn off left turn LED
     if(millis()-prevRightMillis>=blinkInterval){                     //Check if enough time has passed since last blink
      prevRightMillis=millis();                                       //Save the last time you blinked the LED
      if(rightState==0){                                              //If the right LED is off, turn it on and vice-versa
        digitalWrite(rightLED,HIGH);
        rightState=1;
      }
      else{
        digitalWrite(rightLED,LOW);
        rightState=0;
      }
     }
    }
    else if(LightsAndHornSignals[1]){                                 //Check if Hazard signal is sent
      //Serial.println("HAZARDS");
      if(millis()-prevHazMillis>=blinkInterval){                      //Check if enough time has passed since last blink
        prevHazMillis=millis();                                       //Save the last time LED was blinked
        if(leftState!=rightState){                                    //Check to see if LED states are not the same
          rightState=leftState;                                       //Change state of right LED
          if(leftState==0){                                           //Check to see if Left LED is off
           digitalWrite(rightLED, LOW);                               //Turn off right LED
          } else digitalWrite(rightLED,HIGH);                         //Turn on right LED
        }
        if(leftState==0){                                             //If LEDs are off, turn them on and vice-versa
          digitalWrite(leftLED,HIGH);
          digitalWrite(rightLED,HIGH);
          leftState=1;
          rightState=1;
        }
        else{
          digitalWrite(leftLED,LOW);
          digitalWrite(rightLED,LOW);
          leftState=0;
          rightState=0;
        }
      }
    }
    else{                                                              //If no left or right signal is sent
      digitalWrite(leftLED,LOW);                                       //Turn off Left LED
      digitalWrite(rightLED,LOW);                                      //Turn off Right LED
      leftState=0;
      rightState=0;
    }
    
//Headlights
  if(LightsAndHornSignals[0]){                    //Check if headlight signal is sent
      //Serial.println("HEADLIGHT");
      digitalWrite(headLight, HIGH);              //Turn on headlights 
    }else digitalWrite(headLight, LOW);           //Turn off headlights 

//Horns
   if(LightsAndHornSignals[2]){                   //Check if Horn signal is sent
      //Serial.println("HORN");
      digitalWrite(horn, HIGH);                   //Turn on horns
    }else digitalWrite(horn, LOW);                //Turn off horns
}
