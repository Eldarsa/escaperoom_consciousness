#include <Arduino.h>
#include "fsm.h"

static state current_state;
static unsigned long previous_time_stamp{0};
static bool inhaling;
static unsigned long inhaling_time_stamp{0};
unsigned long current_time;
uint16_t current_pressure;
static unsigned long pressure_time_stamp{0};
static unsigned long blink_time_stamp{0};
static unsigned long right_eye_pos{0};
static unsigned long left_eye_pos{0};
static unsigned long right_eye_lim[2];
static unsigned long left_eye_lim[2];

void eyelid_stop(char eye)
{
  // 'B' for 'Both'

  // Right eye
  if(eye == 'R' || eye == 'B'){ 
    digitalWrite(ENABLE_R, HIGH);
    analogWrite(ONE_A, 0);
    analogWrite(TWO_A, 0);
  }

  // Left eye
  if(eye == 'L' || eye == 'B'){
    digitalWrite(ENABLE_L, HIGH);
    analogWrite(ONE_B, 0);
    analogWrite(TWO_B, 0);
  }
}

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


///   pos = reference as a value between 0 and 100
///     - 100 => 100% open
///     - 0   => 0% open
///   speed = duty cycle value between 0 and 255
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

/* Helper function for set_eyelid_position that sets the motor signals-
 *
 * Returns:
 * true if the desired position has been reached, false otherwise.
 * Stops the motor once target position is achieved.
 */
bool drive_eyelid_to_position(char eye, int currentPos, int targetPos, int speed, int upDrivePin, int downDrivePin)
{
  int epsilon = 5; // Tolerance for determining when the desired position is reached.
  
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
    eyelid_stop(eye);
    Serial.println("Position found!");
    return true;
  }
}

///  Returns positions as a value between 0 and 100
///     - 100 => 100% open
///     - 0   => 0% open
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

// Convenience function
void close_eye(char eye, int speed)
{
  set_eyelid_position(eye, 0, speed);   // '0' implies 0% open
}

// Convenience function
void open_eye(char eye, int speed)
{
  set_eyelid_position(eye, 100, speed);  // '100' implies 100% open
}

void blink(int openDelay)
{
  set_eyelid_position('B', 0, 255);
  
  // Delay before re-opening eyes
  delay(openDelay);

  set_eyelid_position('B', 100, 255);
}

void init_eyes()
{
  Serial.println("Initializing eyes... ");

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

void setup() {
  TCCR2B = TCCR2B & (B11111000 | B00000001); // Set D9 and D10 pin to 30 kHz
  TCCR1B = TCCR1B & (B11111000 | B00000001); // Set D11 and D12 pin to 30 kHz
  TCCR4B = TCCR4B & (B11111000 | B00000001);   // Set 6, 7, 8 pin to 30 kHz
  pinMode(BUILT_IN_LED, OUTPUT); // Bulit in LED should blink at startup to verify flashing
  pinMode(CHEST_LEFT, OUTPUT);
  pinMode(CHEST_RIGHT, OUTPUT);
  pinMode(PRESSURE_SENS, INPUT);
  pinMode(ENABLE_R, OUTPUT);
  pinMode(ENABLE_L, OUTPUT);
  pinMode(ONE_A, OUTPUT);
  pinMode(TWO_A, OUTPUT);
  pinMode(ONE_B, OUTPUT);
  pinMode(TWO_B, OUTPUT);

  current_state = INIT;
  previous_time_stamp = millis();
  current_pressure = analogRead(A0);
  
  Serial.begin(9600); //For debugging

  Serial.println("Restarting program.. ");

  /* Blink LED to verify setup complete */
  for(int i = 0; i < 10; i++)
  {
    digitalWrite(BUILT_IN_LED, HIGH);
    delay(250);
    digitalWrite(BUILT_IN_LED, LOW);
    delay(250);
  }

  Serial.println("Program Start \n");
}


// Check if the current time exceeds a certain interval from a given timestamp
bool check_time_lapse(unsigned long timestamp, unsigned long interval){
    return (current_time - timestamp >= interval);
}

void loop() {

  current_time = millis();
  right_eye_pos = analogRead(EYE_LID_POS_RIGHT);
  left_eye_pos = analogRead(EYE_LID_POS_LEFT);
  
  print_state(current_state);

  switch(current_state){
    case INIT:
      init_eyes();
      current_state = IDLE;
      break;

    case AIRWAY:
      // STATE sensor comparisons and set actuators
      if(false) // State transition statement
      {

      }
      break;

    case BREATHING:
      // STATE sensor comparisons and set actuators
      if(false) // State transition statement
      {

      }
      break;

    case CIRCULATION:
      // STATE sensor comparisons and set actuators
      if(false) // State transition statement
      {

      }
      break;

    case PAIN_RESPONSE:

      // State transition condition
      if(check_time_lapse(blink_time_stamp, BLINKING_FREQ_MS))
      {
        Serial.println("Blink");
        Serial.println(right_eye_pos);

      }
        
      if(check_time_lapse(previous_time_stamp, PAIN_STATE_TIME)) 
      {
        current_state = IDLE;
        previous_time_stamp = millis();
        break;
      }

      // STATE sensor comparisons and set actuators
      if(check_time_lapse(inhaling_time_stamp, PAIN_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling();
        break;
      }
      else if(check_time_lapse(inhaling_time_stamp, PAIN_INHALATION_SPEED) && inhaling == true)
      {
        stopInhaling();
        break;
      }

    case IDLE:
    
      /* STATE sensor comparisons and set actuators
      Breathing: Normal and bilateral breathing
      */

      Serial.println("In IDLE state \n");

      if(check_time_lapse(inhaling_time_stamp, IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        startInhaling();
        break;
      }

      if(check_time_lapse(inhaling_time_stamp, RESPIRATORY_INHALATION_SPEED) && inhaling == true){
        stopInhaling();
      }

      if(check_time_lapse(pressure_time_stamp, DELTA_T_PRESSURE_MS)){
        pressure_time_stamp = millis();
        current_pressure = analogRead(A0);
      }

      if(check_time_lapse(pressure_time_stamp, DELTA_T2)) {
        checkForPainResponse();
      }
      break;

    default:
      break;
  }
}

// Starts the inhaling process
void startInhaling() {
  analogWrite(CHEST_LEFT, 255);
  analogWrite(CHEST_RIGHT, 255);
  inhaling_time_stamp = millis();
  inhaling = true;
}

// Stops the inhaling process
void stopInhaling() {
  analogWrite(CHEST_LEFT, 0);
  analogWrite(CHEST_RIGHT, 0);
  inhaling = false;
}

// Checks for pain response and transitions state if necessary
void checkForPainResponse() {
  if((uint16_t)(analogRead(PRESSURE_SENS)) >= (current_pressure + DELTA_P_PRESSURE)) {
    current_state = PAIN_RESPONSE;
    previous_time_stamp = millis();
  }
}

void print_state(int state)
{
  if(state == 0)
  {
    Serial.println("Airway");
  }
  else if(state == 1)
  {
    Serial.println("Breathing");
  }
  else if(state == 2)
  {
    Serial.println("Circulation");
  }
  else if(state == 3)
  {
    Serial.println("Pain response");
  }
  else if(state == 4)
  {
    Serial.println("Idle");
  }

}