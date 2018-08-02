// ESC4: BRUSHLESS MOTOR CONTROLLER WITH L293D

#define PIN_ESC_AEN 4
#define PIN_ESC_ADIR 5
#define PIN_ESC_BEN 6
#define PIN_ESC_BDIR 7
#define PIN_ESC_CEN 8
#define PIN_ESC_CDIR 9
// DIR: 1: SOURCE, 0: DRAIN

unsigned char ESC_SEQUENCE_H[] = {
  PIN_ESC_ADIR,
  PIN_ESC_ADIR,
  PIN_ESC_BDIR,
  PIN_ESC_BDIR,
  PIN_ESC_CDIR,
  PIN_ESC_CDIR
};

unsigned char ESC_SEQUENCE_L[] = {
  PIN_ESC_BDIR,
  PIN_ESC_CDIR,
  PIN_ESC_CDIR,
  PIN_ESC_ADIR,
  PIN_ESC_ADIR,
  PIN_ESC_BDIR
};

unsigned char ESC_SEQUENCE_Z[] = {
  PIN_ESC_CEN,
  PIN_ESC_BEN,
  PIN_ESC_AEN,
  PIN_ESC_CEN,
  PIN_ESC_BEN,
  PIN_ESC_AEN
};

void sense0_isr();
void esc_step(bool dir = true);

void setup_esc()
{
  pinMode(PIN_ESC_AEN, OUTPUT);
  pinMode(PIN_ESC_ADIR, OUTPUT);
  pinMode(PIN_ESC_BEN, OUTPUT);
  pinMode(PIN_ESC_BDIR, OUTPUT);
  pinMode(PIN_ESC_CEN, OUTPUT);
  pinMode(PIN_ESC_CDIR, OUTPUT);

  esc_step();
}

void esc_set_free_run() {
  digitalWrite(PIN_ESC_AEN, false);
  digitalWrite(PIN_ESC_BEN, false);
  digitalWrite(PIN_ESC_CEN, false);
}

volatile char esc_state = 0;

void esc_set_state(char ix) {
  unsigned char pinH = ESC_SEQUENCE_H[esc_state];
  unsigned char pinL = ESC_SEQUENCE_L[esc_state];
  unsigned char pinZ = ESC_SEQUENCE_Z[esc_state];

// switch 
  digitalWrite(pinH, HIGH); // SOURCE
  digitalWrite(pinL, LOW); // DRAIN

  // switch FLOATING wire
  //digitalWrite(pinZ, LOW);

  // activate NON-FLOATING wire(s)
  digitalWrite(PIN_ESC_AEN, pinZ!=PIN_ESC_AEN);
  digitalWrite(PIN_ESC_BEN, pinZ!=PIN_ESC_BEN);
  digitalWrite(PIN_ESC_CEN, pinZ!=PIN_ESC_CEN);
}

void esc_step(bool dir = true) {
  if (dir) {
    esc_state++;
    if (esc_state > 5) esc_state = 0;
  } else {
    if (esc_state <= 0) esc_state = 5;
    else esc_state--;
  }
  
  esc_set_state(esc_state);
}


#define ESC_PHASE_A_SENSE_PIN 10
#define ESC_PHASE_B_SENSE_PIN 11
#define ESC_PHASE_C_SENSE_PIN 12

// ix: A-0, B-1, C-2
// false: -/lower than virtual zero, true: +/...
volatile bool PHASE_STATES[3] = {false, false, false};

void onZeroCross(char wire,bool rising) {
  //esc_state
  //esc_set_state(...);
}

unsigned long long LAST_CHANGE_US = 0;
long LAST_DT_US = 0;

void esc_check_phases() {
  // ix: A-0, B-1, C-2
  bool cur[3];
  bool changed[3];
  cur[0] = digitalRead(ESC_PHASE_A_SENSE_PIN);
  cur[1] = digitalRead(ESC_PHASE_B_SENSE_PIN);
  cur[2] = digitalRead(ESC_PHASE_C_SENSE_PIN);
  int nChanged = 0;
  for(int i=0;i<3;i++) {
    changed[i] = PHASE_STATES[i] != cur[i]; nChanged++;
  }
  if(nChanged==0) return;
  if(nChanged>1) {esc_step(); delayMicroseconds(40); return;}
  unsigned long long now = micros();
  long dt_us = now - LAST_CHANGE_US;
  // ...
  LAST_DT_US = dt_us;
  LAST_CHANGE_US = now;
  for(int i=0;i<3;i++) {if(changed[i]) {onZeroCross(i,cur[i]);}}
}


