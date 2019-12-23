#include "AuxControlUnit.h"

AuxControlUnit::AuxControlUnit() {
  // constructor
}

// switches power between the Main-switched gate and the Supplementary-switched gate
static void AuxControlUnit::switchPower(bool batteryOK) {
  // TODO: set pinModes to OUTPUT in a fucntion.
  if (batteryOK) {
    digitalWrite(kMainSwGate, HIGH);   // enable Main-switched power
    digitalWrite(kSuppSwGate, LOW);   // disable Supplementary-switched power
  } else {
    digitalWrite(kSuppSwGate, HIGH);   // enable Supplementary-switched power
    digitalWrite(kMainSwGate, LOW);   // disable Main-switched power
  }
}
