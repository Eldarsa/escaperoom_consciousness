
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

/**
 * @brief Starts the inhaling by opening valve
 * 
 * @param chest Character that represents which chest to inhale ('R' for right, 'L' for left, 'B' for both)
 */
void startInhaling(char chest = 'B');

/**
 * @brief Stops the inhaling by closing the valve
 * 
 * @param chest Character that represents which chest to stop inhaling ('R' for right, 'L' for left, 'B' for both)
 */
void stopInhaling(char chest = 'B');

/**
 * @brief Begins normal breathing
 * 
 * @param triggerSound Boolean that represents if sound should be triggered (default false)
 */
void breatheNormally(bool triggerSound = false);

/**
 * @brief Begins pain response breathing
 */
void breatheInPain();

/**
 * @brief Begins bilateral breathing
 */
void breatheBilaterally();