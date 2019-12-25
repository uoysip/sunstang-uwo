#include "SteeringControlUnit.h"

MCP_CAN CAN(SPI_CS_PIN);              //init CAN object

void setup() {
  Serial.begin(9600);

//Lights and Horns pinmodes
  pinMode(kLeftTurn, INPUT);                               //Left turn signal input
  pinMode(kRightTurn, INPUT);                              //Right turn signal input
  pinMode(kHeadlightToggle, INPUT);                          //Headlight button input
  pinMode(kHazardLightToggle, INPUT);                             //Hazard button input
  pinMode(kHorn, INPUT);                               //Horn button input
  pinMode(kReverse, INPUT);                            //Reverse Switch input

//Motor Controller pinmodes
  pinMode(kCruiseToggle, INPUT);                        //Cruise Control on/off button input
  pinMode(kCruseSpeedUp ,INPUT);                      //Cruise Control increase button input
  pinMode(kCruseSpeedDown, INPUT);                     //Cruise Control decrease button input
  pinMode(kAcceleration, INPUT);                       //Accleration pedal potentiometer input
  pinMode(kRegenBrakeToggle, INPUT);                         //Regen Braking on/off switch input

//Test LED pinmodes
  pinMode(kLED1, OUTPUT);                          //Test LED #1 output
  pinMode(kLED2, OUTPUT);                          //Test LED #2 output
  pinMode(kLED3, OUTPUT);                          //Test LED #3 output

//Initialize CAN bus
  CAN.begin(CAN_1000KBPS);                             //Init can bus : baudrate = 1000k

  CAN.maskRange(0, 1, 0, 0x7ff);

  CAN.init_Filt(0, 0, 0x1F4);                           //Filter 1: 0001 1111 0100
  CAN.init_Filt(1, 0, MC_ID + 3);                         //Filter 2: 0111 1101 0001
  CAN.filterRange(2,5, 0, 0x7D1)

//Setup Motor Power Command
  motorPower[0] = (float) 0;                       //These bits are reserved
  motorPower[1] = (float) 1.0;                     //Set controller to use 100% of the available BUS current 
}

void loop() {
//read steering wheel inpnuts
  headLightSig = digitalRead(headLight);           
                                                                     //read headlight switch
  //! these functions do not exist
  //! [1] refactor here to...
  togReadButtn(kHazardLightToggle, xHazardState, hazardState, hazardSig, hazardDebounce);                                       //read hazard button (toggle)
  readButtn(kHorn, xHornState, hornState, hornSig, hornDebounce);                                                    //read horn button 
  togReadButtn(kLeftTurn, xLeftState, leftState, leftSig, leftDebounce);                                                 //read left turn signal switch
  
  // maybe extract these out
  if(!lastLeft && leftSig && rightSig){
    rightSig = 0;
  }
  lastLeft = leftSig;
  
  togReadButtn(kRight, xRightState, rightState, rightSig, rightDebounce);                                            //read right turn signal switch
  if(!lastRight && rightSig && leftSig){
    leftSig = 0;
  }
  lastRight = rightSig;
  
  revSig = digitalRead(reverse);                                                                                      //read reverse switch
  togReadButtn(cruiseOnOff, xCruiseOnOffState, cruiseOnOffState, cruiseOnOffSig, cruiseOnOffDebounce);              //read on/off cruise control button (toggle)
  regenBrakeSig = digitalRead(regenBrake);                                                                            //read Regen Brake on/off switch

  if (hazardSig) {
    leftSig=0;
    rightSig=0;
  }
  
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

    CAN.sendMsgBuf(LHCU_ID, 0, 8, LHCUSend);      //Sending Lights and Horns message buffer over CAN
  }

  // [1]...here

if (CAN.hasMessage()) {
  struct MessageFrame message = recieveMsg(CAN);

  if (message.canID == Brake_ID) 
    message.receiveBuffer.readBytes(Brakes, 8);

  if (message.canID == kMotorid) 
    message.receiveBuffer.readBytes(VelocityBuf, 8);

  currMotorRPM = (int) unpackFloat(VelocityBuf[4]);
  currVehVel = (int) unpackFloat(VelocityBuf[0]);

  if (!kCruiseToggle)
    vehicleVelocity = currVehVel;

}


//MotorDrive buffer code  
//if the brakes are on then set the motordrive
  if (Brakes[0]) {
    motorDrive[0] = (float) 0;
    motorDrive[1] = (float) 0;

    if (cruiseOnOffSig)
      cruiseOnOffSig = 0;

  }
  else if (cruiseOnOffSig) {                                                                                               //Check if cruise control is on
    motorDrive[0] = (float) vehicleVelocity;            
    motorDrive[1] = 1.0;
    readButtn(cruiseIncSpeed, xCruiseIncSpeedState, cruiseIncSpeedState, cruiseIncSpeedSig, cruiseIncSpeedDebounce);  //read increase cruise control button
    readButtn(cruiseDecSpeed, xCruiseDecSpeedState, cruiseDecSpeedState, cruiseDecSpeedSig, cruiseDecSpeedDebounce);  //read decrease cruise controll button
    
    if (cruiseIncSpeedSig) {                                                                                            //Check if 
      vehicleVelocity++;    
    } 
    else if (cruiseDecSpeedSig) {
      vehicleVelocity--;
    }
  }
  else if (revSig) {  //Check if Reverse switch is on
    potRead(acceleration, accRaw, accFloat, accPotMin, accPotMax);                                                     //read acceleration potentiometer  
    motorDrive[0] = (float) -100;
    motorDrive[1] = accFloat;
  }
  else {
    potRead(acceleration, accRaw, accFloat, accPotMin, accPotMax);                                                      //read acceleration potentiometer  
    CAN_MotorDrive[0] = (float) 100;
    CAN_MotorDrive[1] = accFloat;
  }
}