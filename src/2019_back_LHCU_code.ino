/*Desctiption: Code for the back lights and horns control unit
 *Version: 2
 *Contributors: Steven Lawrence
 *Date: 02/12/19
 */

//include libraries
#include "mcp_can.h"                                                            //library for CAN communication
#include <SPI.h>                                                                //SPI library 

const int SPI_CS_PIN = 10;                                                      //CAN chip select pin

MCP_CAN CAN(SPI_CS_PIN);                                                        //init CAN object 

const int leftLED = 4;                                                          //OUTPUT: Left turn signal LEDs
const int rightLED = 3;                                                         //OUTPUT: Right turn signal LEDs
const int reverseLight = 2;                                                     //OUTPUT: Backup LEDs
const int brakes = 5;                                                           //OUTPUT: Brake LEDs

//CAN IDs
unsigned long LHCU_ID=0x30C;                                                    //Lights and Horns signal ID
unsigned long Brake_ID=0x1F4;                                                   //Brake signal ID

//CAN recieve buffers
byte LightsAndHornSignals[8];                                                   //Message buffer for turn signals, headlights and horns from SCU
byte Brake[8];                                                                  //Message buffer for brake signal from DCU

//timing variables
int blinkInterval = 500;
unsigned long prevLeftMillis(0), prevRightMillis(0), prevHazMillis(0);

bool leftState(0), rightState(0);                                               //turn signal states

void setup() {
  Serial.begin(9600);

  pinMode(leftLED, OUTPUT);                              //Left turn signal output
  pinMode(rightLED, OUTPUT);                             //Right turn signal output
  pinMode(reverseLight, OUTPUT);                         //Reverse light output
  pinMode(brakes, OUTPUT);                               //Brake output

  //Initialize CAN bus
  CAN.begin(CAN_500KBPS);              //Init can bus : baudrate = 500k

  CAN.init_Mask(0,0,0x7ff);                               //Mask 1: 0111 1111 1111
  CAN.init_Mask(1,0,0x7ff);                               //Mask 2: 0111 1111 1111        

  CAN.init_Filt(0,0,0x30C);                               //Filter 1: 0011 0000 1100
  CAN.init_Filt(1,0,0x1F4);                               //Filter 2: 0001 1111 0100
  CAN.init_Filt(2,0,0x7D1);                               //Filter 3: 0111 1101 0001
  CAN.init_Filt(3,0,0x7D1);                               //Filter 4: 0111 1101 0001
  CAN.init_Filt(4,0,0x7D1);                               //Filter 5: 0111 1101 0001
  CAN.init_Filt(5,0,0x7D1);                               //Filter 6: 0111 1101 0001
}

void loop() {
//setup variables of receiving CAN messages
  unsigned char lenReceive = 0;                                       //data length
  unsigned char bufReceive[8];                                        //receive data buffer
  unsigned long canId=0;                                              //CAN ID

//Receive message
  if(CAN_MSGAVAIL == CAN.checkReceive()){                             //Check if anything is in the receive buffer
    CAN.readMsgBuf(&lenReceive, bufReceive);                          //Read data,  lenReceive: data length, bufReceive: data buffer
    canId = CAN.getCanId();                                           // read the message id associated with this CAN message (also called a CAN frame)
    
    /*Serial.print("Sensor Value from ID: ");                           //Print the ID of the CAN message
    Serial.print(canId);
    Serial.print(" = ");
    
    for (int i = 0; i < lenReceive; i++){                             //Print the data from the CAN message
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }
    Serial.println();*/

//Processing received messages
    if(canId == LHCU_ID){                                             //Check if message is from SCU
      //Serial.println("Lights and Horns");
      for(unsigned i=0; i<8; i++){
        LightsAndHornSignals[i]=bufReceive[i];                        //Load receive buffer into lights and horns buffer
      }
    }
    else if(canId == Brake_ID){                                       //Check is message is from DCU
      for(unsigned i=0; i<8; i++){
        Brake[i]=bufReceive[i];                                       //Load receive buffer into brake buffer
      }
    }
  }

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
    else{
        digitalWrite(leftLED,LOW);
        digitalWrite(rightLED,LOW);
        leftState=0;
        rightState=0;
      }
      
//Brakes
  if(Brake[1]){                                                       //Check if brake signal is sent
    //Serial.println("BRAKING");
    digitalWrite(brakes,HIGH);                                        //Turn on brake lights
  }else digitalWrite(brakes,LOW);                                     //Turn off brake lights

//Reverse Lights
  if(LightsAndHornSignals[5]){                                         //Check if reverse signal is sent
    //Serial.println("REVERSING");
    digitalWrite(reverseLight,HIGH);                                  //Turn on white reverse lights
  }else digitalWrite(reverseLight,LOW);                               //Turn off white reverse lights

}
