#include "FrontControlUnit.h"
//turning head lights on or off


void activateFrontLeftIndicator()
{ 
  digitalWrite(kFrontRightLED, LOW);
  if(millis() - prevFrontLeftMillis > kFrontBlinkInterval){
    prevFrontLeftMillis = millis();
    if(leftFrontState == 0){
      digitalWrite(kFrontLeftLED, HIGH);
      leftFrontState = 1;
    }
    else{
      digitalWrite(kFrontLeftLED, LOW);
      leftFrontState = 0;
    }
  }
  
}

void activateFrontRightIndicator()
{ 
  digitalWrite(kFrontLeftLED, LOW);
  if(millis() - prevFrontRightMillis > kFrontBlinkInterval){
    prevFrontRightMillis = millis();
    if(rightFrontState == 0){
      digitalWrite(kFrontRightLED, HIGH);
      rightFrontState = 1;
    }
    else{
      digitalWrite(kFrontRightLED, LOW);
      rightFrontState = 0;
    }
  }
  
}

void activateFrontHazards()
{
  if ((millis() - prevFrontHazMillis) >= kFrontBlinkInterval) {   //checking if blink interval is over                 
    prevFrontHazMillis = millis();                   //setting the new prevHazMillis                    
    if (leftFrontState != rightFrontState){               //if states of either blinkers are not the same reset them                     
        rightFrontState = leftFrontState;                                       
        if (leftFrontState == 0) {                                           
          digitalWrite(kFrontRightLED, LOW);                               
        }  
        else digitalWrite(kFrontRightLED, HIGH);                        
    }
      //if the lights are off turn them on
    if (leftFrontState == 0)   {                                             
      digitalWrite(kFrontLeftLED, HIGH);
      digitalWrite(kFrontRightLED, HIGH);
      leftFrontState = 1;
      rightFrontState = 1;
    }
    //if lights are on turn them off
    else 
      FrontControlUnit::turnOffFrontBlinkers();  
  }   
}

void FrontControlUnit::turnOffFrontBlinkers(){
  digitalWrite(kFrontLeftLED, LOW);
  digitalWrite(kFrontRightLED, LOW);
  leftFrontState = 0;
  rightFrontState = 0;
}

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

