#ifndef GENERALCONTROLUNIT_H
#define GENERALCONTROLUNIT_H

#include "mcp_can.h"
#include <SPI.h>

// General control unit for common functionality across various control units
// designed to be inheritted for use in other control units, not to be directly instantiated in a .ino
class GeneralControlUnit {
  public:
    // CAN message 
    struct MessageFrame {
      unsigned char dataLen = 0;
      unsigned char receiveBuffer[8];
      unsigned long canID = -1;
    };

  protected:
    void MCP_CAN::initialize(INT8U baudRate);
    bool hasMessage();
    struct MessageFrame receiveMsg();
    void filterRange(start, end, ext, ulData);
    void maskRange(start, end, ext, ulData);

};

#endif
