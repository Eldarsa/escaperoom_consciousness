#include "defines.h"




void print_state(int state)
{
  if(state == 0)
  {
    Serial.println("Init");
  }
  else if(state == 1)
  {
    Serial.println("Airway");
  }
  else if(state == 2)
  {
    Serial.println("Breathing");
  }
  else if(state == 3)
  {
    Serial.println("Circulation");
  }
  else if(state == 4)
  {
    Serial.println("Pain response");
  }
  else if(state == 5)
  {
    Serial.println("Idle");
  }

}


// Check if the current time exceeds a certain interval from a given timestamp
bool time_interval_passed(unsigned long timestamp, unsigned long target_interval){
    return (millis() - timestamp >= target_interval);
}
