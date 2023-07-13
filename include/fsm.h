#include <Arduino.h>

enum state {INIT, AIRWAY, BREATHING, CIRCULATION, PAIN_RESPONSE, IDLE};

/* System parameters, to be adjusted accourding to function behaviour*/
const uint32_t PAIN_STATE_TIME = 15UL*1000UL; // Duration of PAIN RESPONSE STATE in seconds * 1000
const uint32_t DELTA_T_PRESSURE_MS = 250;
const uint32_t DELTA_P_PRESSURE = 10;
const uint32_t DELTA_T2 = 50;
const uint16_t PRESSURE_TRESHHOLD = 6;

const uint16_t FX_AIRWAY_CHOKING_SOUND_FREQUENCY = 4; // Denoted in cycles per minute
const uint32_t FX_AIRWAY_CHOKING_RATE_MS = (60UL*1000UL) / FX_AIRWAY_CHOKING_SOUND_FREQUENCY; //ms between each


/* System functions */


bool checkForPainResponse();

void triggerRightArm();
