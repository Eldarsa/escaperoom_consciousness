#include <Arduino.h>

const uint32_t BLINKING_FREQ = 10; // Eye blink per minute

/* Convert parameters to more suitable units */
const uint32_t BLINKING_FREQ_MS = (60UL*1000UL) / BLINKING_FREQ;


// Stops the motion of the eye(s) by disabling PWM signals to the motors
// Parameter 'eye' can be 'R' for Right, 'L' for Left, or 'B' for Both.
void eyelid_stop(char eye, bool fast = true);

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
void blink(int openDelay = 250);

// Initializes and calibrates the eyelid mechanism.
void init_eyes();

void test_eyes();