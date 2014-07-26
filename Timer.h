


#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer
{
public:
  Timer();
  Timer(long interval);
  boolean isFinished();
  boolean isFinishedNoRestart();
  void Interval(long i);
  void Interval(long i,boolean noDelay);
  void Reset();
  
private:

  long _startTime;        // will store last time LED was updated
  long _interval;
};

#endif

