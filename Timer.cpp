
#include "Timer.h"
#include "Arduino.h"

// constructor
Timer::Timer()
{
}

Timer::Timer(long interval)
{ 
 _startTime = millis();
 _interval = interval ;
}

void Timer::Interval(long i){
   _interval = i ;
}


void Timer::Interval(long i, boolean noDelay){
   _interval = i ;
   if(noDelay){
     // first time the trigger will be immediate
      _startTime = millis() + i;

   }
}

void Timer::Reset()
{ 
 _startTime = millis();
}

boolean Timer::isFinished(){
if(millis() - _startTime > _interval) {
    _startTime = millis();   
    return true;
  } 
  return false;
}


boolean Timer::isFinishedNoRestart(){
if(millis() - _startTime > _interval) {
    
    return true;
  } 
  return false;
}


