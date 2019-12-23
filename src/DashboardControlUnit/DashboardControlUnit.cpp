#include"DashboardControlUnit.h"

//check to send brakes
void checkSend(){
  if (digitalRead(brakes)) {                          //Check if brakes are active
    BrakeSend[0] = true;                              //Set brake state to on
  } 
  else 
    BrakeSend[0] = false; 
}
//sends brake message
void brakeSend(){
  if ((millis() - brakeTime) > 50) {                        //Check if enough time has passed since last brake message
    brakeTime = millis();                             //Save the last time brake message was sent
    CAN.sendMsgBuf(Brake_ID, 0, 8, BrakeSend);      //Send Brake message
  }
}