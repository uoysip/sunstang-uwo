#ifndef AUXCONTROLUNIT_H
#define AUXCONTROLUNIT_H

#include "mcp_can.h"
#include <SPI.h>

static const int kMainSwGate = 7;
static const int kSuppSwGate = 8;
static const int kSPIpin = 10;
static const unsigned long kBatteryOkID = 0x10; // TODO: can this become an unsigned int instead?

class AuxControlUnit {
  public: 
    static void switchPower(bool batteryStatus);    // switches power to supp if batteryStatus is false, switches to main if true.
};

#endif
