/**
 * @file    eyes.cpp
 * @author  Eldar Sandanger
 * @email   Eldar.sandanger@gmail.com
 * @date    July 31, 2023
 * @brief   This file implements the functionalities related to the eyes of the Escape Room patient,
 *          which include the calibration and movement of the eye lids.
 */

#include "defines.h"
#include "eyes.h"

static unsigned long right_eye_pos{0};
static unsigned long left_eye_pos{0};
static unsigned long right_eye_lim[2];
static unsigned long left_eye_lim[2];

/**
 * @brief Stop the movement of eyelid(s)
 * 
 * @param eye Character that represents which eye to stop ('R' for right, 'L' for left, 'B' for both)
 * @param fast Boolean that represents if stop is fast or not
 */
void eyelid_stop(char eye, bool fast)
{
  // 'B' for 'Both'

  int stopVal;
  if(fast == true)
  { stopVal = 255; }
  else {
    stopVal = 0;
  }

  // Right eye
  if(eye == 'R' || eye == 'B'){ 
    digitalWrite(ENABLE_R, HIGH);
    analogWrite(ONE_A, stopVal);
    analogWrite(TWO_A, stopVal);
  }

  // Left eye
  if(eye == 'L' || eye == 'B'){
    digitalWrite(ENABLE_L, HIGH);
    analogWrite(ONE_B, stopVal);
    analogWrite(TWO_B, stopVal);
  }
}

/**
 * @brief Fast stop of eyelid(s)
 * 
 * @param eye Character that represents which eye to stop ('R' for right, 'L' for left, 'B' for both)
 */
void fast_eyelid_stop(char eye)
{
  // 'B' for 'Both'

  // Right eye
  if(eye == 'R' || eye == 'B'){ 
    digitalWrite(ENABLE_R, HIGH);
    digitalWrite(ONE_A, HIGH);
    digitalWrite(TWO_A, HIGH);
  }

  // Left eye
  if(eye == 'L' || eye == 'B'){
    digitalWrite(ENABLE_L, HIGH);
    digitalWrite(ONE_B, HIGH);
    digitalWrite(TWO_B, HIGH);
  }
}

/**
 * @brief Find limit of eyelid(s) based on direction
 * 
 * @param eye Character that represents which eye to control ('R' for right, 'L' for left)
 * @param dir Character that represents direction ('U' for up, 'D' for down)
 * @return int Limit value found
 */
int find_eyelid_limit(char eye, char dir){

  bool limitFound;
  int limit;
  int readPin;

  // ONE and TWO are the PWM pins that control the motor speed in each direction.
  int upDrivePin, downDrivePin;
  int UP_SPEED; 
  int DOWN_SPEED; 
  
  // Set which eye to control
  if(eye == 'R')
  {
    upDrivePin = ONE_A;
    downDrivePin = TWO_A;
    readPin = EYE_LID_POS_RIGHT;
  }
  else if(eye == 'L')
  {
    upDrivePin = ONE_B;
    downDrivePin = TWO_B;
    readPin = EYE_LID_POS_LEFT;
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'R' for right or 'L' for left.");
    return -1;
  }

  // Set direction and speed
  if(dir == 'U')
  {
    UP_SPEED = 200;
    DOWN_SPEED = 0;
  }
  else if(dir == 'D')
  {
    UP_SPEED = 0;
    DOWN_SPEED = 200;
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'U' for up or 'D' for down.");
    return -1;
  }

  Serial.print("find_eyelid_limit: eye=");
  Serial.print(eye);
  Serial.print(", dir=");
  Serial.println(dir);

  int currentPos = analogRead(readPin);
  int lastPos;

  analogWrite(upDrivePin, UP_SPEED);
  analogWrite(downDrivePin, DOWN_SPEED);

  delay(250); // Just give the system some time to start moving

  int iterations_not_moved = 0;
  int iterations_treshold = 5;
  int epsilon = 3;

  while(!limitFound) {

    lastPos = currentPos;
    delay(40);
    currentPos = analogRead(readPin);

    // Check if there is a notable difference in movement from last iteration
    if(abs(currentPos - lastPos) <= epsilon){
      iterations_not_moved++;
    }else{
      iterations_not_moved = 0;
    }

    // If the number of iterations not moved exceed the threshold, stop.
    if(iterations_not_moved >= iterations_treshold){
      limitFound = true;
      eyelid_stop(eye);
    }
  }

  limit = analogRead(readPin);

  // When limit is found
  Serial.println("Limit found.");
  Serial.print("Limit value: ");
  Serial.println(limit);

  return limit;
}

/**
 * @brief Set the eyelid position
 * 
 * @param eye Character that represents which eye to control ('R' for right, 'L' for left, 'B' for both)
 * @param targetPos Target position value (between 0 and 100)
 * @param speed Duty cycle value (between 0 and 255)
 * @return int Status of setting position (0 indicates success)
 */
int set_eyelid_position(char eye, int targetPos, int speed)
{
  int readPinRight, readPinLeft; 
  int potMaxRight, potMinRight, potMaxLeft, potMinLeft;
  int upDrivePinRight, downDrivePinRight, upDrivePinLeft, downDrivePinLeft;
  
  bool rightEyeDone = false;
  bool leftEyeDone = false;

  // Set individual eye to control, or both eyes simulatenously
  if(eye == 'R' || eye == 'B')
  {
    upDrivePinRight = ONE_A;
    downDrivePinRight = TWO_A;
    readPinRight = EYE_LID_POS_RIGHT;
    potMaxRight = right_eye_lim[0];
    potMinRight = right_eye_lim[1];
  }
  if(eye == 'L' || eye == 'B')
  {
    upDrivePinLeft = ONE_B;
    downDrivePinLeft = TWO_B;
    readPinLeft = EYE_LID_POS_LEFT;
    potMaxLeft = left_eye_lim[0];
    potMinLeft = left_eye_lim[1];
  }
  else if(eye != 'R' && eye != 'B')
  {
    Serial.println("Invalid eye parameter. Use 'R' for right, 'L' for left, or 'B' for both.");
    return -1;
  }

  while(true)
  {
    if(eye == 'R' || eye == 'B')
    {
      if(!rightEyeDone)
      {
        int currentPosRight = map(analogRead(readPinRight), potMinRight, potMaxRight, 0, 100);
        rightEyeDone = drive_eyelid_to_position('R', currentPosRight, targetPos, speed, upDrivePinRight, downDrivePinRight);
      }
    }

    if(eye == 'L' || eye == 'B')
    {
      if(!leftEyeDone)
      {
        int currentPosLeft = map(analogRead(readPinLeft), potMinLeft, potMaxLeft, 0, 100);
        leftEyeDone = drive_eyelid_to_position('L', currentPosLeft, targetPos, speed, upDrivePinLeft, downDrivePinLeft);
      }
    }

    // Exit condition
    if((eye == 'R' && rightEyeDone) || (eye == 'L' && leftEyeDone) || (eye == 'B' && rightEyeDone && leftEyeDone))
    {
      break;
    }

    delay(10);  // 10ms delay to allow for ADC and motor response
  }

  return 0;
}


/**
 * @brief Drive eyelid to a given position
 * 
 * @param eye Character that represents which eye to control
 * @param currentPos Current position value
 * @param targetPos Target position value
 * @param speed Duty cycle value
 * @param upDrivePin Up driving pin number
 * @param downDrivePin Down driving pin number
 * @return bool Returns true if desired position has been reached, false otherwise
 */
bool drive_eyelid_to_position(char eye, int currentPos, int targetPos, int speed, int upDrivePin, int downDrivePin)
{
  int epsilon = 10; // Tolerance for determining when the desired position is reached.
  
  if(currentPos > targetPos + epsilon)
  {
    analogWrite(upDrivePin, 0);
    analogWrite(downDrivePin, speed);
    return false;
  }
  else if(currentPos < targetPos - epsilon)
  {
    analogWrite(upDrivePin, speed);
    analogWrite(downDrivePin, 0);
    return false;
  }
  else
  {
    // The position is correct
    eyelid_stop(eye, true);
    Serial.println("Position found!");
    return true;
  }
}

/**
 * @brief Get eyelid position
 * 
 * @param eye Character that represents which eye's position to get ('R' for right, 'L' for left)
 * @return int Position value between 0 and 100
 */
int get_eyelid_position(char eye)
{
  int readPin, potMin, potMax;

  // Set which eye to control
  if(eye == 'R')
  {
    readPin = EYE_LID_POS_RIGHT;
    potMax = right_eye_lim[0];
    potMin = right_eye_lim[1];
  }
  else if(eye == 'L')
  {
    readPin = EYE_LID_POS_LEFT;
    potMax = left_eye_lim[0];
    potMin = left_eye_lim[1];
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'R' for right or 'L' for left.");
    return -1;
  }

  return map(analogRead(readPin), potMin, potMax, 0, 100);
}

/**
 * @brief Close eye
 * 
 * @param eye Character that represents which eye to close ('R' for right, 'L' for left, 'B' for both)
 * @param speed Speed at which the eye should close
 */
void close_eye(char eye, int speed)
{
  set_eyelid_position(eye, 0, speed);   // '0' implies 0% open
}

/**
 * @brief Open eye
 * 
 * @param eye Character that represents which eye to open ('R' for right, 'L' for left, 'B' for both)
 * @param speed Speed at which the eye should open
 */
void open_eye(char eye, int speed)
{
  set_eyelid_position(eye, 100, speed);  // '100' implies 100% open
}

/**
 * @brief Perform blink action
 * 
 * @param openDelay Delay before reopening eyes
 */
void blink(int openDelay)
{
  set_eyelid_position('B', 0, 150);
  
  // Delay before re-opening eyes
  delay(openDelay);

  set_eyelid_position('B', 100, 150);
}

/**
 * @brief Initialize eyes
 */
void init_eyes()
{
  Serial.println("Initializing eyes... ");

  right_eye_pos = analogRead(EYE_LID_POS_RIGHT);
  left_eye_pos = analogRead(EYE_LID_POS_LEFT);

  // Set enable pins high
  digitalWrite(ENABLE_R, 1);
  digitalWrite(ENABLE_L, 1);
  analogWrite(ONE_A, 0);
  analogWrite(TWO_A, 0);
  analogWrite(ONE_B, 0);
  analogWrite(TWO_B, 0);

  // Calibrate eyes
  right_eye_lim[1] = find_eyelid_limit('R', 'D');
  right_eye_lim[0] = find_eyelid_limit('R', 'U');
  left_eye_lim[1] = find_eyelid_limit('L', 'D');
  left_eye_lim[0] = find_eyelid_limit('L', 'U');

  // Set default position
  open_eye('L', 200);
  open_eye('R', 200);

  Serial.println("Eyes initiated.");

}
