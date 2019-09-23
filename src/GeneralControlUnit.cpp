#include "GeneralControlUnit.h"

// initializes the CAN BUS from the control unit with an inputted baud rate
void MCP_CAN::initialize(INT8U baudRate) {
  while (CAN.begin(baudRate) != CAN_OK) {
    Serial.println("CAN BUS initialization failed, reattempting...");
    delay(100);
  }

  #if DEBUG_MODE
    Serial.println("CAN BUS initialization successful");
  #endif
}

// checks if the CAN bus has a message to receive
bool MCP_CAN::hasMessage(void) {
  return CAN.checkReceive() == CAN_MSGAVAIL;
}

// receive a message from the CAN bus.
// it is the responsibility of the user to check if the CAN bus has a message to
// receive first, this can be checked by calling hasMessage() from an if statement
// outside of where receiveMsg will be called.
struct MessageFrame receiveMsg(MCP_CAN CAN) {
  struct MessageFrame message;

  // store the data length, receive buffer, and CAN ID into the message struct
  CAN.readMsgBuf(message.&dataLen, message.receiveBuffer);
  message.canID = CAN.getCanId();
    
  // print to the Serial communication the information about the message
  #if DEBUG_MODE
    Serial.print("Sensor Value from ID: " + message.canID + " = ");

    // TODO: this can be replaced by using the Stream lib
    for (int i = 0; i < message.dataLen; i++) {
      Serial.print(receiveBuffer[i] + "\t");
    }
    Serial.println();
  #endif

  return message;
}

// sets a range of acceptance filter registers (upto 6) to update to ulData (the new filter)
// registers MCP_RXF0SIDH - MCP_RXF5SIDH
void MCP_CAN::filterRange(lowerBound, upperBound, ext, ulData) {
  // catch erroneous lower and upper bound inputs
  if ((lowerBound < 0) || lowerBound > 5 || upperBound <= 0 || upperBound > 5 ) {
    #if DEBUG_MODE
      Serial.println("ERROR: Invalid range for filter acceptance registers... " + "Lower bound: " + lowerBound + ", Upper bound: " + upperBound + ".");
    #endif

    return; // TODO: return 1 if successful, -1 if unsuccessful
  }

  // set the acceptance filter register(s)
  for (int i = lowerBound; i <= upperBound; i++) {
    MCP_CAN.init_Filter(i, ext, ulData);
  }
}

// sets a range of acceptance mask registers (upto 2) to update to ulData (the new filter)
// registers MCP_RXM0SIDH and MCP_RXM1SIDH
void MCP_CAN::maskRange(lowerBound, upperBound, ext, ulData) {
  // catch erroneous lower and upper bound inputs
  if ((lowerBound < 0) || lowerBound > 1 || upperBound <= 0 || upperBound > 5 ) {
    #if DEBUG_MODE
      Serial.println("ERROR: Invalid range for mask acceptance registers... " + "Lower bound: " + lowerBound + ", Upper bound: " + upperBound + ".");
    #endif
    return; // TODO: return 1 if successful, -1 if unsuccessful
  }

  // set the acceptance mask register(s)
  for (int i = lowerBound; i <= upperBound; i++) {
    MCP_CAN.init_Mask(i, ext, ulData);
  }
}
