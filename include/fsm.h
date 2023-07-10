#include <Arduino.h>

enum state {INIT, AIRWAY, BREATHING, CIRCULATION, PAIN_RESPONSE, IDLE};

/* System parameters, to be adjusted accourding to function behaviour*/
const uint32_t PAIN_STATE_TIME = 30UL*1000UL; // Duration of PAIN RESPONSE STATE in seconds * 1000
const uint32_t DELTA_T_PRESSURE_MS = 250;
const uint32_t DELTA_P_PRESSURE = 10;
const uint32_t DELTA_T2 = 50;
const uint16_t PRESSURE_TRESHHOLD = 6;


/* System functions */


bool checkForPainResponse();

void triggerRightArm();
