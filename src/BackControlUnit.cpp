#include "BackControlUnit.h"
//turning head lights on or off
void activateBrakeLights(bool brake){
  if(brake){
    digitalWrite(brakes, HIGH); 
  }
    else digitalWrite(brakes, LOW); 
}
//turning off and on horn
void activateReverseLights(bool lights){
  if(lights){
    digitalWrite(lights, HIGH); 
  }
    else digitalWrite(lights, LOW); 
}
