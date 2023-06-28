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

enum state {INIT, AIRWAY, BREATHING, CIRCULATION, PAIN_RESPONSE, IDLE};

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

/*Functions*/
void init_eyes();

//void set_eyelid_motor_speed(char eye, char dir, int speed);
int set_eyelid_position(char eye, int pos, int speed = 150);
int get_eyelid_position(char eye);
int find_eyelid_limit(char eye, char dir);
void close_eye(char eye, int speed = 150);
void open_eye(char eye, int speed = 150);

void fast_eyelid_stop(char eye = 'B');

void printState(int state);

