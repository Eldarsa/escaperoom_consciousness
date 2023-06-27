#include <Arduino.h>
#include <unity.h>
#include "../src/fsm.h"

#define EYE_POSITION_TOLERANCE 10

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}


void test_pressure_sensor()
{
  int readVal = analogRead(PRESSURE_SENS);
  Serial.print("Sensor: ");
  Serial.println(readVal);
}

void test_right_eye()
{
  // Calibrate the right eye
  right_eye_lim[0] = analogRead(EYE_LID_POS_RIGHT); // Assume this is "closed"
  Serial.print("Right Eye Calibration (Closed): ");
  Serial.println(right_eye_lim[0]);

  // Make the eye "open" and read the position
  right_eye_lim[1] = analogRead(EYE_LID_POS_RIGHT); // Assume this is "open"
  Serial.print("Right Eye Calibration (Open): ");
  Serial.println(right_eye_lim[1]);

  // Make the eye blink
  // Depending on how you make the doll blink, insert that function here

  // Check the position after blink
  right_eye_pos = analogRead(EYE_LID_POS_RIGHT);
  Serial.print("Right Eye Position After Blink: ");
  Serial.println(right_eye_pos);

  // Assert the position is within tolerance
  TEST_ASSERT_GREATER_OR_EQUAL(right_eye_lim[0] - EYE_POSITION_TOLERANCE, right_eye_pos);
  TEST_ASSERT_LESS_OR_EQUAL(right_eye_lim[1] + EYE_POSITION_TOLERANCE, right_eye_pos);
}

void test_left_eye()
{

}

void setup() 
{

  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  pinMode(LED_BUILTIN, OUTPUT);

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_pressure_sensor);

  init_eyes();


  RUN_TEST(test_right_eye);
  RUN_TEST(test_left_eye);
}

void loop() 
{

}