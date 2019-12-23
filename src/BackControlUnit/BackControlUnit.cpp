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
    digitalWrite(reverseLight, HIGH); 
  }
    else digitalWrite(reverseLight, LOW); 
}

void activateBackLeftIndicator()
{ 
  digitalWrite(kBackRightLED, LOW);
  if(millis() - prevBackLeftMillis > kBackBlinkInterval){
    prevBackLeftMillis = millis();
    if(leftBackState == 0){
      digitalWrite(kBackLeftLED, HIGH);
      leftBackState = 1;
    }
    else{
      digitalWrite(kBackLeftLED, LOW);
      leftBackState = 0;
    }
  }
  
}

void activateBackRightIndicator()
{ 
  digitalWrite(kBackLeftLED, LOW);
  if(millis() - prevBackRightMillis > kBackBlinkInterval){
    prevBackRightMillis = millis();
    if(rightBackState == 0){
      digitalWrite(kBackRightLED, HIGH);
      rightBackState = 1;
    }
    else{
      digitalWrite(kBackRightLED, LOW);
      rightBackState = 0;
    }
  }
  
}
void turnOffBackBlinkers(){
  digitalWrite(kBackLeftLED, LOW);
  digitalWrite(kBackRightLED, LOW);
  leftBackState = 0;
  rightBackState = 0;
}

void activateBackHazards()
{
  if ((millis() - prevBackHazMillis) >= kBackBlinkInterval) {   //checking if blink interval is over                 
    prevBackHazMillis = millis();                   //setting the new prevHazMillis                    
    if (leftBackState != rightBackState){               //if states of either blinkers are not the same reset them                     
        rightBackState = leftBackState;                                       
        if (leftBackState == 0) {                                           
          digitalWrite(kBackRightLED, LOW);                               
        }  
        else digitalWrite(kBackRightLED, HIGH);                        
    }
      //if the lights are off turn them on
    if (leftBackState == 0)   {                                             
      digitalWrite(kBackLeftLED, HIGH);
      digitalWrite(kBackRightLED, HIGH);
      leftBackState = 1;
      rightBackState = 1;
    }
    //if lights are on turn them off
    else  turnOffBackBlinkers();  
  }   
}

