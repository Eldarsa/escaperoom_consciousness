#include "defines.h"
#include <Arduino.h>
#include "utils.h"

// Check if the current time exceeds a certain interval from a given timestamp
bool time_interval_passed(unsigned long timestamp, unsigned long target_interval){
    return (millis() - timestamp >= target_interval);
}

void trigger_sound(int fx_pin)
{
    digitalWrite(fx_pin, LOW);
    delay(250);
    digitalWrite(fx_pin, HIGH);
}