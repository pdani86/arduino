
// MYESC2: BRUSHLESS MOTOR CONTROLLER WITH L293D

#define NANO_BOARD_LED 13

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
//#define ESC_STATE_PORT_MASK (~((1<<PIN_ESC_AH) | (1<<PIN_ESC_AL) | (1<<PIN_ESC_BH) | (1<<PIN_ESC_BL) | (1<<PIN_ESC_CH) | (1<<PIN_ESC_CL) ))

// SEQUENCE: [[H,L,Z],[H,L,Z],...]
unsigned char ESC_SEQUENCE[] = {
  PIN_ESC_ADIR, PIN_ESC_BDIR, PIN_ESC_CEN,
  PIN_ESC_ADIR, PIN_ESC_CDIR, PIN_ESC_BEN,
  PIN_ESC_BDIR, PIN_ESC_CDIR, PIN_ESC_AEN,
  PIN_ESC_BDIR, PIN_ESC_ADIR, PIN_ESC_CEN,
  PIN_ESC_CDIR, PIN_ESC_ADIR, PIN_ESC_BEN,
  PIN_ESC_CDIR, PIN_ESC_BDIR, PIN_ESC_AEN
};


void esc_step(bool dir = true);

bool led_state = false;
bool skip_next_delay = false;
void sense0_isr() {
  //bitRead(PORTD, 13)
  led_state = !led_state;
  digitalWrite(NANO_BOARD_LED, (led_state) ? (HIGH) : (LOW));
  skip_next_delay = true;
  esc_step();
}

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
}

char esc_state = 0;

void esc_step(bool dir = true) {
  if (dir) {
    esc_state++;
    if (esc_state > 5) esc_state = 0;
  } else {
    if (esc_state <= 0) esc_state = 5;
    else esc_state--;
  }

  int pinH = ESC_SEQUENCE[esc_state * 3];
  int pinL = ESC_SEQUENCE[(esc_state * 3) + 1];
  int pinZ = ESC_SEQUENCE[(esc_state * 3) + 2];

  // switch FLOATING wire
  digitalWrite(pinZ, LOW);

  // switch 
  digitalWrite(pinH, HIGH); // SOURCE
  digitalWrite(pinL, LOW); // DRAIN

  // activate NON-FLOATING wire(s)
  digitalWrite(PIN_ESC_AEN, pinZ!=PIN_ESC_AEN);
  digitalWrite(PIN_ESC_BEN, pinZ!=PIN_ESC_BEN);
  digitalWrite(PIN_ESC_CEN, pinZ!=PIN_ESC_CEN);
  
  
}

void setup() {
  pinMode(NANO_BOARD_LED, OUTPUT);
  setup_esc();
}

int lastSensorValue = 0;
int loopCounter = 0;
int stepTimeMs = 1000;

void loop() {
  //digitalWrite(NANO_BOARD_LED,(sensorValue<511)?(LOW):(HIGH));
  loopCounter++;
  if(0==loopCounter%12) {
    int sensorValue = analogRead(A0);
    lastSensorValue = sensorValue;
    stepTimeMs = (sensorValue >> 3) + 1;
  }
  
  if(!skip_next_delay) {
    esc_step();
    
    delay(stepTimeMs);
  } else {
    skip_next_delay = false;
     delay(stepTimeMs/2);
  }
}
