#include "defines.h"
#include "breathing.h"
#include "utils.h"

// Starts the inhaling process
void startInhaling(char chest) {
  //Serial.println("Inhaling start");

  if(chest == 'R' || chest == 'B')
  {
  analogWrite(CHEST_RIGHT, 255);
  }
  if(chest == 'L' || chest == 'B')
  {
    analogWrite(CHEST_LEFT, 255);
  }
  else if(chest != 'R' && chest != 'B')
  {
    Serial.println("Invalid eye parameter. Use 'R' for right, 'L' for left, or 'B' for both.");
  }

  inhaling_time_stamp = millis();
  inhaling = true;
}

// Stops the inhaling process
void stopInhaling(char chest) {
  //Serial.println("Inhaling stop");

  if(chest == 'R' || chest == 'B')
  {
  analogWrite(CHEST_RIGHT, 0);
  }
  if(chest == 'L' || chest == 'B')
  {
    analogWrite(CHEST_LEFT, 0);
  }
  else if(chest != 'R' && chest != 'B')
  {
    Serial.println("Invalid eye parameter. Use 'R' for right, 'L' for left, or 'B' for both.");
  }

  inhaling = false;
}

void breatheNormally(unsigned long current_time)
{
    if(time_interval_passed(inhaling_time_stamp, current_time, IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
    {
        startInhaling();
        return;
    }

    if(time_interval_passed(inhaling_time_stamp, current_time, RESPIRATORY_INHALATION_SPEED) && inhaling == true){
        stopInhaling();
        return;
    }
}

void breatheInPain(unsigned long current_time)
{
      if(time_interval_passed(inhaling_time_stamp, current_time, PAIN_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling();
        return;
      }
      else if(time_interval_passed(inhaling_time_stamp, current_time, PAIN_INHALATION_SPEED) && inhaling == true)
      {
        stopInhaling();
        return;
      }
}

void breatheBilaterally(unsigned long current_time)
{
      if(time_interval_passed(inhaling_time_stamp, current_time, IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling('L');
        return;
      }
      else if(time_interval_passed(inhaling_time_stamp, current_time, PAIN_INHALATION_SPEED) && inhaling == true)
      {
        stopInhaling('L');
        return;
      }
}