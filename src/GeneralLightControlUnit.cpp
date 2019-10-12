#include "GeneralLightControlUnit.h"
//checks which blinker
void activateBlinkers (bool blinker, unsigned long *prevMillis, int rLED ,int lLED, bool *rState, bool *lState)
{
  //left blinker
  if(blinker){ 
      digitalWrite(rLED, LOW);
      flashBlinkers(&prevMillis, lLED, &lState);
  }
 //right blinker
  else {
    digitalWrite(lLED, LOW);
    flashBlinkers(&prevMillis, rLED, &rState);
  }    
}

//activate either left or right blinker
void flashBlinkers (unsigned long *prevMillis, int LED, bool *state)
{
  if((millis()-*prevMillis) >= kBlinkInterval){
    *prevMillis = millis();
    if(state == 0){
      digitalWrite(LED, HIGH);
      *state = 1;
    }
    else{
      digitalWrite(LED, LOW);
      *state = 0;
    }
  }
}

//turning on and off the hazards TODO try to improve readibility but ?
void activateHazards(unsigned long *prevMillis, int rLED, int lLED ,bool *rState, bool *lState )
{
  if ((millis() - *prevMillis) >= kBlinkInterval) {   //checking if blink interval is over                 
    *prevMillis = millis();                   //setting the new prevHazMillis                    
    if (*lState != *rState){               //if states of either blinkers are not the same reset them                     
        *rState = *lState;                                       
        if (*lState == 0) {                                           
          digitalWrite(rLED, LOW);                               
        }  
        else digitalWrite(rLED, HIGH);                        
    }
      //if the lights are off turn them on
    if (*lState == 0)   {                                             
      digitalWrite(lLED, HIGH);
      digitalWrite(rLED, HIGH);
      *lState = 1;
      *rState = 1;
    }
    //if lights are on turn them off
    else  turnOffBlinkers(rLED, lLED, &rState, &lState);  
  }   
}

//turning off the blinkers 
void turnOffBlinkers(int rLED, int lLED ,bool *rState, bool *lState){
  digitalWrite(lLED, LOW);
  digitalWrite(rLED, LOW);
  *lState = 0;
  *rState = 0;
}

