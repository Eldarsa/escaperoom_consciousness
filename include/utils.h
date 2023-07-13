#include <Arduino.h>

void print_state(int state);
bool time_interval_passed(unsigned long timestamp, unsigned long target_interval);

void trigger_sound(int fx_pin);