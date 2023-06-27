#include <Arduino.h>
#include "fsm.h"


/* System functions */

///  eye = 'L' (Left) or 'R' (Right)
///  dir = 'D' (Down) or 'U' (Up)
///  speed = the duty cycle: between 0 (always off) and 255 (always on)
/* void set_eyelid_motor_speed(char eye, char dir, int speed)
{
  int ONE, TWO;
  
  if(eye == 'R')
  {
    ONE = ONE_A;
    TWO = TWO_A;
  }
  else if(eye == 'L')
  {
    ONE = ONE_B;
    TWO = TWO_B;
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'R' for right or 'L' for left.");
    return;
  }
  
  // There are two PWM signals controlling the speed in each direction. 
  // To run in one direction, we must also turn off the PWM in the other direction. 
  if(dir == 'U')
  {
    analogWrite(ONE, speed);
    analogWrite(TWO, 0);
  }
  else if(dir == 'D')
  {
    analogWrite(TWO, speed);
    analogWrite(ONE, 0);
  }
}
 */

void fast_eyelid_stop(char eye)
{
  // 'B' for 'Both'

  // Right eye
  if(eye == 'R' || 'B'){ 
    digitalWrite(ENABLE_R, HIGH);
    digitalWrite(ONE_A, HIGH);
    digitalWrite(TWO_A, HIGH);
  }

  // Left eye
  if(eye == 'L' || 'B'){
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
  int ONE, TWO;
  int ONE_SPEED; 
  int TWO_SPEED; 
  
  // Set which eye to control
  if(eye == 'R')
  {
    ONE = ONE_A;
    TWO = TWO_A;
    readPin = EYE_LID_POS_RIGHT;
  }
  else if(eye == 'L')
  {
    ONE = ONE_B;
    TWO = TWO_B;
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
    ONE_SPEED = 80;
    TWO_SPEED = 0;
  }
  else if(dir == 'D')
  {
    ONE_SPEED = 0;
    TWO_SPEED = 80;
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'U' for up or 'D' for down.");
    return -1;
  }

  int currentPos = analogRead(readPin);
  int lastPos;

  analogWrite(ONE, ONE_SPEED);
  analogWrite(TWO, TWO_SPEED);

  int iterations_not_moved = 0;
  int iterations_treshold = 2;
  int epsilon = 3;

  while(!limitFound) {

    lastPos = currentPos;
    delay(250);
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
      fast_eyelid_stop(eye);
    }
  }

  limit = analogRead(readPin);
  return limit;
}


///  eye = 'L' (left) or 'R' (right)
///   pos = reference as a value between 0 and 100
///     - 100 => 100% open
///     - 0   => 0% open
///   speed = duty cycle value between 0 and 255
///
///   The position values are mapped from the max- and min-reads of the potentiometer, 
///   which are found when the find_eyelid_limits is called during the setup.
int set_eyelid_position(char eye, int pos, int speed)
{
  int readPin; 
  int potMax, potMin;
  int ONE, TWO;
  
  int epsilon = 3;

  // Set which eye to control
  if(eye == 'R')
  {
    ONE = ONE_A;
    TWO = TWO_A;    
    readPin = EYE_LID_POS_RIGHT;
    potMax = right_eye_lim[0];
    potMin = right_eye_lim[1];
  }
  else if(eye == 'L')
  {
    ONE = ONE_B;
    TWO = TWO_B;
    readPin = EYE_LID_POS_LEFT;
    potMax = left_eye_lim[0];
    potMin = left_eye_lim[1];
  }
  else
  {
    Serial.println("Invalid eye parameter. Use 'R' for right or 'L' for left.");
    return -1;
  }

  int currentPos = map(analogRead(readPin), potMin, potMax, 0, 100); 
  int lastPos;
  char dir;

  unsigned long previousMillis = millis();
  int delayDuration = map(speed, 0, 255, 100, 10); // Maps speed to a delay duration between 100 and 10 ms

  while(true){

    unsigned long currentMillis = millis();
    
    // Control the number of checks per time frame
    if(currentMillis - previousMillis >= delayDuration)
    {
      previousMillis = currentMillis;

      lastPos = currentPos;
      currentPos = map(analogRead(readPin), potMin, potMax, 0, 100);

      // Check which direction to set the motor
      if(currentPos > pos + epsilon)
      {
        dir = 'D'; 
        analogWrite(ONE, speed);
        analogWrite(TWO, 0);
      }
      else if(currentPos < pos - epsilon)
      {
        dir = 'U';
        analogWrite(ONE, 0);
        analogWrite(TWO, speed);
      }
      else
      {
        // The position is correct
        fast_eyelid_stop(eye);
        return 0;
      }
    }
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

void init_eyes()
{
  Serial.println("Initializing eyes");

  // Set enable pins high
  digitalWrite(ENABLE_R, 1);
  digitalWrite(ENABLE_L, 1);

  right_eye_lim[0] = find_eyelid_limit('R', 'D');
  right_eye_lim[1] = find_eyelid_limit('R', 'U');

  left_eye_lim[0] = find_eyelid_limit('L', 'D');
  left_eye_lim[1] = find_eyelid_limit('L', 'U');

  close_eye('L', 150);
  close_eye('R', 150);
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

  /* Blink LED to verify setup complete */
  for(int i = 0; i < 20; i++)
  {
    digitalWrite(BUILT_IN_LED, HIGH);
    delay(250);
    digitalWrite(BUILT_IN_LED, LOW);
    delay(250);
  }

  Serial.println("Program Start \n");
}

void blink_eyes()
{

}

void loop() {

  current_time = millis();
  right_eye_pos = analogRead(EYE_LID_POS_RIGHT);
  left_eye_pos = analogRead(EYE_LID_POS_LEFT);
  
  printState(current_state);

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
      if(current_time - blink_time_stamp >= BLINKING_FREQ_MS)
      {
        Serial.println("Blink");
        Serial.println(right_eye_pos);
        if(right_eye_pos >= ((right_eye_lim[1] - right_eye_lim[0]) / 2)) // Right eye is open
        {
          set_eyelid_position('R', 80, 1);
          blink_time_stamp = millis();
        }
      }
      if((right_eye_pos < ((right_eye_lim[1] - right_eye_lim[0]) / 2)) && (millis() - blink_time_stamp >= 150))
      {
        
      }
      

      if(current_time - previous_time_stamp >= PAIN_STATE_TIME) 
      {
        current_state = IDLE;
        previous_time_stamp = millis();
        break;
      }

      // STATE sensor comparisons and set actuators
      if((current_time - inhaling_time_stamp >= PAIN_RESPIRATORY_RATE_MS) && inhaling == false)
      {
        analogWrite(CHEST_LEFT, 255);
        analogWrite(CHEST_RIGHT, 255);
        inhaling_time_stamp = millis();
        inhaling = true;
        break;
      }
      else if((current_time - (inhaling_time_stamp) >= PAIN_INHALATION_SPEED) && inhaling == true)
      {
        analogWrite(CHEST_LEFT, 0);
        analogWrite(CHEST_RIGHT, 0);
        inhaling = false;
        break;
      }



    case IDLE:
      
      /* FOR TESTING 
      test_pressure_sensor();  
      delay(1000);  
      break;
      */
     
    /* STATE sensor comparisons and set actuators
    Breathing: Normal and bilateral breathing
     */

    //Serial.println("In IDLE state \n");
    //test_eye_position();
      if(right_eye_pos > (right_eye_lim[1] + 20)) // If right eye is open, close it
      {
        close_eye('R');
      }else if(right_eye_pos <= (right_eye_lim[1] + 20))
      {
        fast_eyelid_stop('R');
      }
      if(left_eye_pos > (left_eye_lim[1] + 20))
      {
        close_eye('L', 40);
      }else if(left_eye_pos <= (left_eye_lim[1] + 20))
      {
        fast_eyelid_stop('L');
      }
      if((current_time - inhaling_time_stamp >= IDLE_RESPIRATORY_RATE_MS) && inhaling == false)
      {
      //Serial.println("In loop \n");
        analogWrite(CHEST_LEFT, 255);
        analogWrite(CHEST_RIGHT, 255);
        inhaling_time_stamp = millis();
        inhaling = true;
        break;
      }
      else if((current_time - (inhaling_time_stamp) >= RESPIRATORY_INHALATION_SPEED) && inhaling == true)
      {
      //Serial.println("In loop B \n");
        analogWrite(CHEST_LEFT, 0);
        analogWrite(CHEST_RIGHT, 0);
        inhaling = false;
        break;
      }

      if(current_time - pressure_time_stamp >= DELTA_T_PRESSURE_MS) // State transition statement
      {
        //Serial.print("State change \n");
        pressure_time_stamp = millis();
        current_pressure = analogRead(A0);
      }

      if(current_time - pressure_time_stamp >= DELTA_T2)
      {
        if((uint16_t)(analogRead(PRESSURE_SENS)) >= (current_pressure + DELTA_P_PRESSURE))
        {
          current_state = PAIN_RESPONSE;
          previous_time_stamp = millis();
          break;
        }
      }

    default:
      break;
  }
  
}


void printState(int state)
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