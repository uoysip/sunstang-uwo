#ifndef DASHBOARDCONTROLUNIT_H
#define DASHBOARDCONTROLUNIT_H

#include "GeneralLightControlUnit.h"
const int SPI_CS_PIN = 10;   
unsigned long Brake_ID = 0x1F4;            //Brake message ID
unsigned long MC_ID = 0x300;               //Motor controller ID
byte BrakeSend[8];                              //Brake transmitting message buffer
byte RelaySend[8];                              //Secondary relay control transmitting message buffer
byte VelocityBuf[8];                            //Velocity recieve buffer from motor controller
const int brakes = 2;                            //Brake lever switch pin
unsigned long brakeTime = 0;               //Last time brake message was sent
int vehicleVelocity = 0;                        //Vehicle velocity 
int motorRPM = 0;                               //Motor RPM
//Setup variables for receiving CAN messages
unsigned char lenReceive = 0;
unsigned char bufReceive[8];
unsigned char canID = 0;

class DashboardControllUnit : public GeneralLightControlUnit {
  protected:
    void checkSend();
    void brakeSend();
    
};
#endif