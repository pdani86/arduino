
// ESC4: BRUSHLESS MOTOR CONTROLLER WITH L293D

#include "esc4_base.h"

#define NANO_BOARD_LED 13

#define ESC_FREE_RUN_PIN 12

bool led_state = false;
void toggleLed();

void toggleLed() {
  led_state = !led_state;
  digitalWrite(NANO_BOARD_LED, led_state?(HIGH):(LOW));
}

bool en_serial = true;

void setup() {
  pinMode(NANO_BOARD_LED, OUTPUT);
  pinMode(ESC_FREE_RUN_PIN, INPUT);
  if(en_serial) Serial.begin(9600);
  setup_esc();
}

void print_info() {
  if(!en_serial) return;
  /*const int CYCLE_PER_REV = 4;
  long long US_PER_REV = last_dt_us*6*CYCLE_PER_REV;
  //long long MS_PER_REV = US_PER_REV/1000;
  Serial.println((unsigned long)US_PER_REV);*/
}

void loop() {
  esc_check_phases();

  /*
  digitalRead(ESC_FREE_RUN_PIN);
  char free_val = digitalRead(ESC_FREE_RUN_PIN);

  if(free_val && !free_run) {
    free_run = true;
    esc_set_free_run();
  } else if(!free_val && free_run) {
    free_run = false;
    esc_step();
  }*/

}
