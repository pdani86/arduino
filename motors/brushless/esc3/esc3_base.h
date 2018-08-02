


#define PIN_ESC_SENSE0 2
#define PIN_ESC_SENSE1 3

#define PIN_ESC_EN 10

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
  pinMode(PIN_ESC_EN, OUTPUT);

  pinMode(PIN_ESC_AEN, OUTPUT);
  pinMode(PIN_ESC_ADIR, OUTPUT);
  pinMode(PIN_ESC_BEN, OUTPUT);
  pinMode(PIN_ESC_BDIR, OUTPUT);
  pinMode(PIN_ESC_CEN, OUTPUT);
  pinMode(PIN_ESC_CDIR, OUTPUT);
  
  pinMode(PIN_ESC_SENSE0, INPUT);
  pinMode(PIN_ESC_SENSE1, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PIN_ESC_SENSE0), sense0_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_ESC_SENSE0), sense0_isr, CHANGE);

  esc_step();
}

void esc_set_free_run() {
  digitalWrite(PIN_ESC_AEN, false);
  digitalWrite(PIN_ESC_BEN, false);
  digitalWrite(PIN_ESC_CEN, false);
}

volatile char esc_state = 0;

void esc_step(bool dir = true) {
  if (dir) {
    esc_state++;
    if (esc_state > 5) esc_state = 0;
  } else {
    if (esc_state <= 0) esc_state = 5;
    else esc_state--;
  }
  
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
