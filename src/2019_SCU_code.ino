/*Desciption: Code for Steering wheel control unit (need to fix MotorDrive buffer code)
 *Version: 3 
 *Contributors: Steven Lawrence
 *              Nathan Dimla
 *Date: 02/12/19
 */

//include libraries 
#include <mcp_can.h>                  //library for CAN communication
#include <SPI.h>                      //SPI library

const int SPI_CS_PIN = 10;            //CAN chip select pin

MCP_CAN CAN(SPI_CS_PIN);              //init CAN object

// CAN IDs
unsigned long MC_ID = 0x300;          //Motor controller ID
unsigned long Driver_ID = 0x200;      //Driver ID
unsigned long LHCU_ID = 0x30c;        //Lights and Horns ID
unsigned long Brake_ID = 0x1F4;       //Brake message ID

//CAN transmitting message buffers
float CAN_MotorDrive[2];              //Motor drive message buffer
float CAN_MotorPower[2];              //Motor power message buffer
byte LHCUSend[6];                     //Lights and Horns message buffer

//CAN receiveing message buffers
byte Brakes[8];                       //Brake message buffer (DCU)
byte VelocityBuf[8];                            //Velocity buffer from motor controller

//init pins
const int hazard = 2;                 //INPUT: Hazard toggle button
const int right = 3;                  //INPUT: Right turn switch
const int left = 4;                   //INPUT: Left turn switch
const int cruiseDecSpeed = 5;         //INPUT: Decrease cruise control speed button
const int cruiseOnOff = 6;            //INPUT: Cruise control on/off toggle button
const int headLight = 7;              //INPUT: Headlight toggle button
const int horn = 8;                   //INPUT: Horn button
const int cruiseIncSpeed = 9;         //INPUT: Increase cruise control speed button
const int reverse = A0;               //INPUT: Reverse Switch
const int regenBrake = A1;            //INPUT: Regen Braking On/Off
const int testLED1 = A2;              //OUTPUT: Test LED #1
const int testLED2 = A3;              //OUTPUT: Test LED #2
const int testLED3 = A4;              //OUTPUT: Test LED #3
const int acceleration = A5;          //INPUT: Acceleration potentiometer

//time variables
uint32_t tNow = 0;                    //Current time
uint32_t tPrev = 0;                   //Previous time
unsigned long CAN_Millis=0;           //CAN message time
//unsigned long serialTime=0;           //Delay length for serial monitor printing

//Lights and Horns variables
bool hazardState(0), hornState, leftState, rightState;                                    //LHCU Current states
bool xHazardState(LOW), xHornState(LOW), xLeftState(LOW), xRightState(LOW);                        //LHCU Previous states
bool headLightSig(0), hazardSig(0), hornSig(0), leftSig(0), rightSig(0), revSig(0);   //LHCU signals
long hazardDebounce(0), hornDebounce(0), leftDebounce(0), rightDebounce(0);                        //LHCU button debounces
bool lastLeft(0), lastRight(0);
    
//Motor Controller variables
bool cruiseOnOffState(0), cruiseIncSpeedState, cruiseDecSpeedState;                   //MCU Current states
bool xCruiseOnOffState(LOW), xCruiseIncSpeedState(LOW), xCruiseDecSpeedState(LOW);    //MCU previous states
bool cruiseOnOffSig(0), cruiseIncSpeedSig, cruiseDecSpeedSig, regenBrakeSig(0);             //MCU signals
long cruiseOnOffDebounce(0), cruiseIncSpeedDebounce(0), cruiseDecSpeedDebounce(0);    //MCU button debounces

//SCU potentiometer variables
int accRaw;                                                                           //Acceleration potentiometer input
int accPotMin(560);                                                                   //Minimum acceleration value
int accPotMax(955);                                                                   //Maximum acceleration value
float accFloat;                                                                       //Acceleration value

long debounceDelay = 50;                                                              //button debounce delay

int currVehVel = 0;                                                                   //Current vehicle velocity 
int currMotorRPM = 0;                                                                 //Current motor RPM

int vehicleVelocity = 0;                                                              //Vehicle velocity for cruise control 

void setup() {
  Serial.begin(9600);

//Lights and Horns pinmodes
  pinMode(left, INPUT);                               //Left turn signal input
  pinMode(right, INPUT);                              //Right turn signal input
  pinMode(headLight, INPUT);                          //Headlight button input
  pinMode(hazard, INPUT);                             //Hazard button input
  pinMode(horn, INPUT);                               //Horn button input
  pinMode(reverse, INPUT);                            //Reverse Switch input

//Motor Controller pinmodes
  pinMode(cruiseOnOff, INPUT);                        //Cruise Control on/off button input
  pinMode(cruiseIncSpeed,INPUT);                      //Cruise Control increase button input
  pinMode(cruiseDecSpeed, INPUT);                     //Cruise Control decrease button input
  pinMode(acceleration, INPUT);                       //Accleration pedal potentiometer input
  pinMode(regenBrake, INPUT);                         //Regen Braking on/off switch input

//Test LED pinmodes
  pinMode(testLED1, OUTPUT);                          //Test LED #1 output
  pinMode(testLED2, OUTPUT);                          //Test LED #2 output
  pinMode(testLED3, OUTPUT);                          //Test LED #3 output

//Initialize CAN bus
  CAN.begin(CAN_500KBPS);                             //Init can bus : baudrate = 500k


//initialize CAN masks
  CAN.init_Mask(0,0,0x7ff);                           //Mask 1: 0111 1111 1111
  CAN.init_Mask(1,0,0x7ff);                           //Mask 2: 0111 1111 1111

//initialize CAN filters
  CAN.init_Filt(0,0,0x1F4);                           //Filter 1: 0001 1111 0100
  CAN.init_Filt(1,0,MC_ID+3);                         //Filter 2: 0111 1101 0001
  CAN.init_Filt(2,0,0x7D1);                           //Filter 3: 0111 1101 0001
  CAN.init_Filt(3,0,0x7D1);                           //Filter 4: 0111 1101 0001
  CAN.init_Filt(4,0,0x7D1);                           //Filter 5: 0111 1101 0001
  CAN.init_Filt(5,0,0x7D1);                           //Filter 6: 0111 1101 0001

//Setup Motor Power Command
  CAN_MotorPower[0] = (float)0;                       //These bits are reserved
  CAN_MotorPower[1] = (float)1.0;                     //Set controller to use 100% of the available BUS current 
}

void loop() {

//Setup variables for receiving CAN messages
  unsigned char lenReceive = 0;                         
  unsigned char bufReceive[8];
  unsigned char canID=0;

//read steering wheel inpnuts
  headLightSig=digitalRead(headLight);                                                                              //read headlight switch
  togReadButtn(hazard, xHazardState, hazardState, hazardSig, hazardDebounce);                                       //read hazard button (toggle)
  readButtn(horn, xHornState, hornState, hornSig, hornDebounce);                                                    //read horn button 
  togReadButtn(left, xLeftState, leftState, leftSig, leftDebounce);                                                 //read left turn signal switch
  if(!lastLeft&&leftSig&&rightSig){
    rightSig=0;
  }
  lastLeft=leftSig;
  
  togReadButtn(right, xRightState, rightState, rightSig, rightDebounce);                                            //read right turn signal switch
  if(!lastRight&&rightSig&&leftSig){
    leftSig=0;
  }
  lastRight=rightSig;
  
  revSig=digitalRead(reverse);                                                                                      //read reverse switch
  togReadButtn(cruiseOnOff, xCruiseOnOffState, cruiseOnOffState, cruiseOnOffSig, cruiseOnOffDebounce);              //read on/off cruise control button (toggle)
  regenBrakeSig=digitalRead(regenBrake);                                                                            //read Regen Brake on/off switch

  if(hazardSig){
    leftSig=0;
    rightSig=0;
  }

/*if(millis()-serialTime>50){

  serialTime=millis();
  
  if(headLightSig){
    Serial.println("Headlights On!");
  }else {
    Serial.println("Headlights Off");
 

   if(hazardSig){
    Serial.println("Hazards On!");
  }else Serial.println("Hazards Off");

   if(hornSig){
    Serial.println("Horns On!");
  }else Serial.println("Horns Off");

   if(leftSig){
    Serial.println("Left Signal On!");
  }else Serial.println("Left Signal Off");

   if(rightSig){
    Serial.println("Right Signal On!");
  }else Serial.println("Right Signal Off");

  Serial.println();
}*/
  
//Sending message 
  if (millis() - tNow > 50) {                     //Check if enough time has passes since sending last lights and horns message
    tNow = millis();                              //Save last time lights and horns message was sent

//Setting up Lights and Horns buffer for message transmission
    LHCUSend[0] = headLightSig;                   //Index 0 = Headlights
    LHCUSend[1] = hazardSig;                      //Index 1 = Hazards
    LHCUSend[2] = hornSig;                        //Index 2 = Horns
    LHCUSend[3] = leftSig;                        //Index 3 = Left turn
    LHCUSend[4] = rightSig;                       //Index 4 = Right turn
    LHCUSend[5] = revSig;                         //Index 5 = Reverse

    /*Serial.print("Lights and Horns Buf: ");
    for(int i=0; i<5;i++){
      Serial.print(LHCUSend[i]);
    }Serial.println();*/

    CAN.sendMsgBuf(LHCU_ID, 0, 8, LHCUSend);      //Sending Lights and Horns message buffer over CAN
  }

//Receiving CAN messages
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&lenReceive, bufReceive);      //Read data,  lenReceive: data length, bufReceive: data buffer
    canID = CAN.getCanId();                       //Read the message id associated with this CAN message (also called a CAN frame)
    
    /*Serial.print("Sensor Value from ID: ");       //Print the ID of the CAN message
    Serial.print(canID);
    Serial.print(" = ");
    
    for (int i = 0; i < lenReceive; i++){         //Print the data from the CAN message
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }
    Serial.println();*/

//Processing received messages
    if(canID == Brake_ID){                        //Check if message from brakes
      for(unsigned i=0; i<8; i++){
        Brakes[i]=bufReceive[i];                  //Load receive buffer into brake buffer
      }
    }

    if(canID=MC_ID+3){                            //Check if message id is motor velocity
      for(unsigned i=0; i<8; i++){
        VelocityBuf[i]=bufReceive[i];             //Load receive buffer into velocity buffer
      }
    }

currMotorRPM = (int)unpackFloat(VelocityBuf[4]);
  currVehVel = (int)unpackFloat(VelocityBuf[0]);
  
if(!cruiseOnOff){
  vehicleVelocity=currVehVel;
}
  }

//MotorDrive buffer code  
  if(Brakes[0]){                                                                                                      //Check if Brakes are on
    CAN_MotorDrive[0]=(float)0;
    CAN_MotorDrive[1]=(float)0;
    if (cruiseOnOffSig){
    cruiseOnOffSig = 0;
    }
  }
  else if(cruiseOnOffSig){                                                                                               //Check if cruise control is on
    CAN_MotorDrive[0]=vehicleVelocity;            
    CAN_MotorDrive[1]=(float)1.0;
    readButtn(cruiseIncSpeed, xCruiseIncSpeedState, cruiseIncSpeedState, cruiseIncSpeedSig, cruiseIncSpeedDebounce);  //read increase cruise control button
    readButtn(cruiseDecSpeed, xCruiseDecSpeedState, cruiseDecSpeedState, cruiseDecSpeedSig, cruiseDecSpeedDebounce);  //read decrease cruise controll button
    
    if(cruiseIncSpeedSig){                                                                                            //Check if 
      vehicleVelocity++;    
    }
    else if(cruiseDecSpeedSig){
      vehicleVelocity--;
    }
  }
  else if(revSig){                                                                                                     //Check if Reverse switch is on
    potRead(acceleration, accRaw, accFloat, accPotMin, accPotMax);                                                     //read acceleration potentiometer  
    CAN_MotorDrive[0]=(float)-100;
    CAN_MotorDrive[1]=accFloat;
  }
  else{
    potRead(acceleration, accRaw, accFloat, accPotMin, accPotMax);                                                      //read acceleration potentiometer  
    CAN_MotorDrive[0]=(float)100;
    CAN_MotorDrive[1]=accFloat;
  }
  

//send message to motor controller
  /*if(millis()-CAN_Millis>5){
    CAN_Millis=millis();
    
    CAN.sendMsgBuf(Driver_ID+1, 0, 8, (unsigned char *)CAN_MotorDrive);                                                 
    CAN.sendMsgBuf(Driver_ID+2, 0, 8, (unsigned char *)CAN_MotorPower);
  }*/
}
