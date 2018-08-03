
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

  bool cur[3];
  cur[0] = digitalRead(ESC_PHASE_A_SENSE_PIN);
  cur[1] = digitalRead(ESC_PHASE_B_SENSE_PIN);
  cur[2] = digitalRead(ESC_PHASE_C_SENSE_PIN);
  for(int i=0;i<3;i++) {Serial.print((cur[i])?("0"):("1"));};
  Serial.println();
}

long last_time = 0;

void loop() {
  if(esc_check_phases()) toggleLed();
  long now_ms = millis();
  long dt_ms = now_ms - last_time;
  if(dt_ms>200) {
    print_info();
    last_time = now_ms;
  }
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
