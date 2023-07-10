#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>

#define PRESSURE_SENS A0
#define EYE_LID_POS_RIGHT A2
#define EYE_LID_POS_LEFT A1
#define CHEST_RIGHT 3
#define CHEST_LEFT 6
#define ONE_A 7 // Must be PWM
#define TWO_A 8 // Must be PWM
#define ONE_B 11 // Must be PWM
#define TWO_B 12 // Must be PWM
#define BUILT_IN_LED 13
#define ENABLE_R 22
#define ENABLE_L 24
#define RIGHT_ARM_SIGNAL 34
#define PNEUMOTHORAX_IN 26
#define AIRWAY_IN 25
#define BLEEDING_IN 24

#define FX_AIRWAY_OBSTRUCTION 27
#define FX_AIRWAY_DONE 29
#define FX_PNEUMOTHORAX_BREATHING 31
#define FX_PNEUMOTHORAX_DONE 33
#define FX_REGULAR_BREATHING 35
#define FX_BLEEDING 37
#define FX_06_UNUSED 39
#define FX_07_UNUSED 41
#define FX_08_UNUSED 43
#define FX_09_UNUSED 45
#define FX_10_UNUSED 47

#endif