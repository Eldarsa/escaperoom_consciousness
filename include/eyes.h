/**
 * @file    eyes.cpp
 * @author  Eldar Sandanger
 * @email   Eldar.sandanger@gmail.com
 * @date    July 31, 2023
 * @brief   This file implements the functionalities related to the eyes of the Escape Room patient,
 *          which include the calibration and movement of the eye lids.
 */

#include <Arduino.h>


const uint32_t BLINKING_FREQ = 10; ///< Eye blink per minute frequency
const uint32_t BLINKING_FREQ_MS = (60UL*1000UL) / BLINKING_FREQ; ///< Converted blinking frequency in ms

/**
 * @brief Stops the motion of the eye(s) by disabling PWM signals to the motors
 * 
 * @param eye Character that represents which eye to stop ('R' for right, 'L' for left, 'B' for both)
 * @param fast Boolean that represents if stop is fast or not (default true)
 */
void eyelid_stop(char eye, bool fast = true);

/**
 * @brief Immediately stops the motion of the eye(s) by applying a brake using the H-Bridge
 * 
 * @param eye Character that represents which eye to stop ('R' for right, 'L' for left, 'B' for both)
 */
void fast_eyelid_stop(char eye);

/**
 * @brief Identifies the physical limits of the eyelid motor in a given direction
 * 
 * @param eye Character that represents which eye to control ('R' for right, 'L' for left)
 * @param dir Character that represents direction ('U' for up, 'D' for down)
 * @return int Limit value found
 */
int find_eyelid_limit(char eye, char dir);

/**
 * @brief Moves the eyelid(s) to a specified position at a defined speed
 * 
 * @param eye Character that represents which eye to control ('R' for right, 'L' for left, 'B' for both)
 * @param targetPos Target position value (between 0 and 100)
 * @param speed Duty cycle value (between 0 and 255)
 * @return int Status of setting position (0 indicates success)
 */
int set_eyelid_position(char eye, int targetPos, int speed);

/**
 * @brief Helper function for set_eyelid_position that sets the motor signals
 * 
 * @param eye Character that represents which eye to control
 * @param currentPos Current position value
 * @param targetPos Target position value
 * @param speed Duty cycle value
 * @param upDrivePin Up driving pin number
 * @param downDrivePin Down driving pin number
 * @return bool Returns true if desired position has been reached, false otherwise
 */
bool drive_eyelid_to_position(char eye, int currentPos, int targetPos, int speed, int upDrivePin, int downDrivePin);

/**
 * @brief Get eyelid position
 * 
 * @param eye Character that represents which eye's position to get ('R' for right, 'L' for left)
 * @return int Position value between 0 and 100
 */
int get_eyelid_position(char eye);

/**
 * @brief Close eye
 * 
 * @param eye Character that represents which eye to close ('R' for right, 'L' for left, 'B' for both)
 * @param speed Speed at which the eye should close
 */
void close_eye(char eye, int speed);

/**
 * @brief Open eye
 * 
 * @param eye Character that represents which eye to open ('R' for right, 'L' for left, 'B' for both)
 * @param speed Speed at which the eye should open
 */
void open_eye(char eye, int speed);

/**
 * @brief Perform blink action
 * 
 * @param openDelay Delay before reopening eyes (default 250ms)
 */
void blink(int openDelay = 250);

/**
 * @brief Initialize and calibrate the eyelid mechanism
 */
void init_eyes();