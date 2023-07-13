#include <Arduino.h>
#include "fsm.h"
#include "eyes.h"
#include "breathing.h"
#include "utils.h"
#include "defines.h"

static state current_state;

unsigned long temp_timestamp;

uint16_t current_pressure;
static unsigned long pressure_time_stamp{0};
static unsigned long blink_time_stamp{0};
static unsigned long choking_sound_timestamp{0};

static bool triggerArmOnPainResponse = true;


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
  pinMode(RIGHT_ARM_SIGNAL, OUTPUT);
  pinMode(PNEUMOTHORAX_IN, INPUT); // NOT USED YET
  pinMode(AIRWAY_IN, INPUT);

  // Sound effect pins
  pinMode(FX_AIRWAY_OBSTRUCTION, OUTPUT);
  pinMode(FX_AIRWAY_DONE, OUTPUT);
  pinMode(FX_PNEUMOTHORAX_BREATHING, OUTPUT);
  pinMode(FX_PNEUMOTHORAX_DONE, OUTPUT);
  pinMode(FX_BLEEDING, OUTPUT);
  pinMode(FX_BREATHE_IN, OUTPUT);
  pinMode(FX_BREATHE_OUT, OUTPUT);
  pinMode(FX_07_UNUSED, OUTPUT);
  pinMode(FX_08_UNUSED, OUTPUT);
  pinMode(FX_09_UNUSED, OUTPUT);
  pinMode(FX_10_UNUSED, OUTPUT);

  // Set sound effects high to not play
  digitalWrite(FX_AIRWAY_OBSTRUCTION, HIGH);
  digitalWrite(FX_AIRWAY_DONE, HIGH);
  digitalWrite(FX_PNEUMOTHORAX_BREATHING, HIGH);
  digitalWrite(FX_PNEUMOTHORAX_DONE, HIGH);
  digitalWrite(FX_BLEEDING, HIGH);
  digitalWrite(FX_BREATHE_IN, HIGH), 
  digitalWrite(FX_BREATHE_OUT, HIGH);
  digitalWrite(FX_07_UNUSED, HIGH);
  digitalWrite(FX_08_UNUSED, HIGH);
  digitalWrite(FX_09_UNUSED, HIGH);
  digitalWrite(FX_10_UNUSED, HIGH);

  current_state = INIT;
  previous_time_stamp = millis();
  temp_timestamp = millis();
  choking_sound_timestamp = millis();
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

void loop() {


  switch(current_state){
    case INIT:
      init_eyes();
      current_state = AIRWAY;
      inhaling = false;
      Serial.println("Transitioning to Airway state \n");
      break;

    case AIRWAY:

      // Trigger choking sound
      if(time_interval_passed(choking_sound_timestamp, FX_AIRWAY_CHOKING_RATE_MS)) 
      {
        trigger_sound(FX_AIRWAY_OBSTRUCTION);
        choking_sound_timestamp = millis();
      }

      //Airway freed
      if(!digitalRead(AIRWAY_IN))
      {
        Serial.println("Transitioning to Breathing state \n");
        current_state = BREATHING;
        trigger_sound(FX_AIRWAY_DONE);
      }

      break;

    case BREATHING:

      breatheBilaterally();
      digitalWrite(FX_PNEUMOTHORAX_BREATHING, LOW);

        int buttonState = digitalRead(PNEUMOTHORAX_IN);
      //print_state(current_state);
      Serial.print("  PT1: "); Serial.print(buttonState);
      Serial.print("\n");

      // Pneumothorax correct
      if(digitalRead(PNEUMOTHORAX_IN))
      {
        digitalWrite(FX_PNEUMOTHORAX_BREATHING, HIGH);
        trigger_sound(FX_PNEUMOTHORAX_DONE);
        Serial.println("Transitioning to Circulation state \n");
        current_state = CIRCULATION;
      }
      break;

    case CIRCULATION:

      breatheNormally();
      digitalWrite(FX_BLEEDING, LOW);

      // Bleeding stopped
      //if(digitalRead(BLEEDING_IN))
      if(digitalRead(BLEEDING_IN))
      {
        digitalWrite(FX_BLEEDING, HIGH);
        Serial.println("Transitioning to IDLE state \n");
        current_state = IDLE;
      }
      break;

    case IDLE:  // DISABILITY and EXPOSURE

      breatheNormally(true);

      if(checkForPainResponse())
      {
          previous_time_stamp = millis();
          current_state = PAIN_RESPONSE;
          Serial.println("Transitioning to Pain Response state \n");
      }

      break;

      
    case PAIN_RESPONSE:

      digitalWrite(FX_PNEUMOTHORAX_BREATHING, LOW);

      breatheInPain();

      if(triggerArmOnPainResponse)
      {
        // Trigger right arm
        digitalWrite(RIGHT_ARM_SIGNAL, HIGH);
        delay(1000);
        digitalWrite(RIGHT_ARM_SIGNAL, LOW);

        // Set false to indicate already triggered
        triggerArmOnPainResponse = false; 
      }

      if(time_interval_passed(blink_time_stamp, BLINKING_FREQ_MS))
      {
        Serial.println("Blinking");
        blink();
        blink_time_stamp = millis();
      }
        
      if(time_interval_passed(previous_time_stamp, PAIN_STATE_TIME)) 
      {
        current_state = IDLE;
        previous_time_stamp = millis();
        triggerArmOnPainResponse = true; // Reset
        digitalWrite(FX_PNEUMOTHORAX_BREATHING, HIGH);
      }
      break;
      
    default:
      break;
  }
}

bool checkForPainResponse()
{

    if(time_interval_passed(pressure_time_stamp, DELTA_T_PRESSURE_MS)){
      pressure_time_stamp = millis();
      current_pressure = analogRead(A0);
    }

    if(time_interval_passed(pressure_time_stamp, DELTA_T2)) {
      if((uint16_t)(analogRead(PRESSURE_SENS)) >= (current_pressure + DELTA_P_PRESSURE)) {
        return true;
      }
    }

    return false;
}