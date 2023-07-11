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

void breatheNormally()
{
    if(time_interval_passed(inhaling_time_stamp, IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
    {
        startInhaling();
        return;
    }

    if(time_interval_passed(inhaling_time_stamp, RESPIRATORY_INHALATION_SPEED) && inhaling == true){
        stopInhaling();
        return;
    }
}

void breatheInPain()
{
      if(time_interval_passed(inhaling_time_stamp, PAIN_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling();
        return;
      }
      else if(time_interval_passed(inhaling_time_stamp, PAIN_INHALATION_SPEED) && inhaling == true)
      {
        stopInhaling();
        return;
      }
}

void breatheBilaterally()
{
      if(time_interval_passed(inhaling_time_stamp, IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling('L');
        analogWrite(CHEST_RIGHT, 0);
        return;
      }

      else if(time_interval_passed(inhaling_time_stamp, PAIN_INHALATION_SPEED) && inhaling == true)
      {
        stopInhaling('L');
        return;
      }
}