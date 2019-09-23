#include "AuxControlUnit.h"
#include <Stream.h>

MCP_CAN CAN(kSPIpin);

void setup() {
  // begin serial communication at 115200 bits per second and initialize the CAN bus at 1000 kilobits per second
  Serial.begin(115200);
  CAN.initialize(CAN_1000KBPS);

  // set the Main and Supplementary switched MOSFET gates to output
  pinMode(kMainSwGate, OUTPUT);
  pinMode(kSuppSwGate, OUTPUT);   

  // set CAN mask and filter acceptance registers for receiving messages
  // information on CAN masks and filters can be found on the wiki
  CAN.maskRange(0, 1, 0, 0x7ff);    // Mask: 0111 1111 1111
  CAN.filterRange(0, 5, 0, 0x7D1);  // Filter: 0111 1101 0001

  // enable the Supplementary-switched MOSFET
  digitalWrite(kSuppSwGate, HIGH);
}

void loop() {
  // check if the CAN bus has a message to be read
  if (CAN.hasMessage()) {
    // store the message from the CAN bus
    struct MessageFrame message = receiveMsg(CAN);
    
    // if the message is from the BPS then load the receive buffer into BattOk and change the power source if necessary
    if (message.canID == kBatteryOkID) {
      // BattOk used to store the receive buffer if the message is relevant
      byte BattOk[8];
      message.receiveBuffer.readBytes(BattOk, 8);
      
      // update the power source
      switchPower(battOk[0]);
    }
  }
}
