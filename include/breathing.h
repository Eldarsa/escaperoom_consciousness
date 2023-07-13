
#include <Arduino.h>

/* System parameters, to be adjusted accourding to function behaviour*/
const uint16_t IDLE_RESPIRATORY_RATE = 13; // Denoted in cycles per minute
const uint16_t PAIN_RESPIRATORY_RATE = 35; // Denoted in cycles per minute
const uint16_t RESPIRATORY_INHALATION_SPEED = 750;  // Time to fill the lung / bladders
const uint16_t PAIN_INHALATION_SPEED = 500;

/* Convert parameters to more suitable units */
const uint32_t IDLE_RESPIRATORY_RATE_MS = (60UL*1000UL) / IDLE_RESPIRATORY_RATE; //ms between each breath in idle state
const uint32_t PAIN_RESPIRATORY_RATE_MS = (60UL*1000UL) / PAIN_RESPIRATORY_RATE; //ms between each breath in pain response state

static bool inhaling = false;

// Starts the inhaling by opening valve
// Parameter 'chest' can be 'R' for Right, 'L' for Left, or 'B' for Both.
void startInhaling(char chest = 'B');

void stopInhaling(char chest = 'B');

void breatheNormally(bool triggerSound = false);

void breatheInPain();

void breatheBilaterally();


