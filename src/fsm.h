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
#define RIGHT_ARM_SIGNAL 32
#define PNEUMOTHORAX_IN 26

enum state {INIT, AIRWAY, BREATHING, CIRCULATION, PAIN_RESPONSE, IDLE};

/* System parameters, to be adjusted accourding to function behaviour*/
const uint16_t IDLE_RESPIRATORY_RATE = 13; // Denoted in cycles per minute
const uint16_t PAIN_RESPIRATORY_RATE = 35; // Denoted in cycles per minute
const uint16_t RESPIRATORY_INHALATION_SPEED = 750;  // Time to fill the lung / bladders
const uint16_t PAIN_INHALATION_SPEED = 500;
const uint16_t PRESSURE_TRESHHOLD = 6;
const uint32_t PAIN_STATE_TIME = 30UL*1000UL; // Duration of PAIN RESPONSE STATE in seconds * 1000
const uint32_t DELTA_T_PRESSURE_MS = 250;
const uint32_t DELTA_P_PRESSURE = 10;
const uint32_t DELTA_T2 = 50;
const uint32_t BLINKING_FREQ = 10; // Eye blink per minute

/* Convert parameters to more suitable units */
const uint32_t IDLE_RESPIRATORY_RATE_MS = (60UL*1000UL) / IDLE_RESPIRATORY_RATE; //ms between each breath in idle state
const uint32_t PAIN_RESPIRATORY_RATE_MS = (60UL*1000UL) / PAIN_RESPIRATORY_RATE; //ms between each breath in pain response state
const uint32_t BLINKING_FREQ_MS = (60UL*1000UL) / BLINKING_FREQ;

/* System functions */

// Stops the motion of the eye(s) by disabling PWM signals to the motors
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
void eyelid_stop(char eye);

// Immediately stops the motion of the eye(s) by applying a brake using the H-Bridge.
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
void fast_eyelid_stop(char eye);

// Identifies the physical limits of the eyelid motor in a given direction by driving the motor until it stops moving.
// Parameter 'eye' can be 'R' for Right or 'L' for Left.
// Parameter 'dir' can be 'U' for Up or 'D' for Down.
int find_eyelid_limit(char eye, char dir);

// Moves the eyelid(s) to a specified position at a defined speed.
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
// Parameter 'targetPos' is the desired position (0-100).
// Parameter 'speed' is the speed of motion (PWM duty cycle, 0-255).
int set_eyelid_position(char eye, int targetPos, int speed);

// Helper function for set_eyelid_position that sets the motor signals-
// Returns true if the desired position has been reached, false otherwise.
// Stops the motor once target position is achieved.
bool drive_eyelid_to_position(char eye, int currentPos, int targetPos, int speed, int upDrivePin, int downDrivePin);

// Returns the current position of an eyelid as a percentage.
// Parameter 'eye' can be 'R' for Right or 'L' for Left.
int get_eyelid_position(char eye);

// Closes the specified eye(s) at a defined speed.
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
// Parameter 'speed' is the speed of motion (PWM duty cycle, 0-255).
void close_eye(char eye, int speed);

// Opens the specified eye(s) at a defined speed.
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
// Parameter 'speed' is the speed of motion (PWM duty cycle, 0-255).
void open_eye(char eye, int speed);

// Makes the eye(s) blink once. Closes the eye(s), waits, and then opens the eye(s).
// Parameter 'openDelay' specifies the delay time in milliseconds before the eye(s) are re-opened.
void blink(int openDelay = 20);

// Initializes and calibrates the eyelid mechanism.
void init_eyes();

void startInhaling(char chest = 'B');

void stopInhaling(char chest = 'B');

bool checkForPainResponse();

void triggerRightArm();

void print_state(int state);