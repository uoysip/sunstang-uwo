#include "FrontControlUnit.h"
//turning head lights on or off
void activateHeadLights(bool lights){
  if(lights){
    digitalWrite(headLight, HIGH); 
  }
    else digitalWrite(headLight, LOW); 
}
//turning off and on horn
void activateHorn(bool Horn){
  if(Horn){
    digitalWrite(horn, HIGH); 
  }
    else digitalWrite(horn, LOW); 
}

