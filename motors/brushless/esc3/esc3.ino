
// MYESC3: BRUSHLESS MOTOR CONTROLLER WITH L293D

#include "esc3_base.h"

#define NANO_BOARD_LED 13


bool led_state = false;
void toggleLed();

void toggleLed() {
  led_state = !led_state;
  digitalWrite(NANO_BOARD_LED, led_state?(HIGH):(LOW));
}

volatile long long last_zero_cross_us = 0;
volatile long long last_dt_us = 0; // time (us) per ESC state

volatile long long wait_dt_us = 0;
volatile bool switch_pending = false;
volatile bool sense0_isr_pending = false;
volatile bool sense0_isr_flag = false;

volatile int stepCounter = 0;

volatile bool free_run = false;

void sense0_isr() {
  if(free_run) return;
  //toggleLed();
  //if(sense0_isr_pending) return;
  //sense0_isr_pending = true;
  sense0_isr_flag = true;
  // -----
  esc_step();
  stepCounter++;

  unsigned long long now = micros();
  long long dt_us = now - last_zero_cross_us;
  //if(dt_us < 50) return; // debounce(?)
  
  last_dt_us = dt_us;
  //wait_dt_us = last_dt_us/2;
  //wait_dt_us = 1;
  //switch_pending = true;
  last_zero_cross_us = now;

  //sense0_isr_pending = false;
}
bool en_serial = true;

#define ESC_FREE_RUN_PIN 12

void setup() {
  pinMode(NANO_BOARD_LED, OUTPUT);
  pinMode(ESC_FREE_RUN_PIN, INPUT);
  if(en_serial) Serial.begin(9600);
  setup_esc();
}



void print_info() {
  if(!en_serial) return;
  const int CYCLE_PER_REV = 4;
  long long US_PER_REV = last_dt_us*6*CYCLE_PER_REV;
  //long long MS_PER_REV = US_PER_REV/1000;
  Serial.println((unsigned int)US_PER_REV);
}

void loop() {

  char free_val = digitalRead(ESC_FREE_RUN_PIN);

  if(free_val && !free_run) {
    free_run = true;
    esc_set_free_run();
  } else if(!free_val && free_run) {
    free_run = false;
    esc_step();
  }

  if(sense0_isr_flag) {
    sense0_isr_flag = false;
    if(0==(stepCounter%240)) {
      
      
      toggleLed();
      print_info();
    }
  }
  
  /*if(switch_pending) {
    unsigned long long now = micros();
    unsigned long long dt_us = (now - last_zero_cross_us);
    if((dt_us < wait_dt_us) && !(dt_us > 250000)) return;
    
    esc_step();
    stepCounter++;
    if(0==(stepCounter%240)) {toggleLed();}
    switch_pending = false;
  }*/
}
