#include <Arduino.h>

void print_state(int state);
bool time_interval_passed(unsigned long timestamp, unsigned long current_time, unsigned long target_interval);