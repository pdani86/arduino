
#define NANO_BOARD_LED 13

#define PIN_ESC_SENSE0 2
#define PIN_ESC_SENSE1 3

#define PIN_ESC_EN 10

#define PIN_ESC_AH 4
#define PIN_ESC_AL 5
#define PIN_ESC_BH 6
#define PIN_ESC_BL 7
#define PIN_ESC_CH 8
#define PIN_ESC_CL 9

#define ESC_STATE_PORT_MASK (~((1<<PIN_ESC_AH) | (1<<PIN_ESC_AL) | (1<<PIN_ESC_BH) | (1<<PIN_ESC_BL) | (1<<PIN_ESC_CH) | (1<<PIN_ESC_CL) ))

unsigned char ESC_SEQUENCE[] = {
  PIN_ESC_AH, PIN_ESC_BL,
  PIN_ESC_AH, PIN_ESC_CL,
  PIN_ESC_BH, PIN_ESC_CL,
  PIN_ESC_BH, PIN_ESC_AL,
  PIN_ESC_CH, PIN_ESC_AL,
  PIN_ESC_CH, PIN_ESC_BL
};


void esc_step(bool dir=true);

bool led_state = false;
void sense0_isr() {
  //bitRead(PORTD, 13)
  led_state = !led_state;
  digitalWrite(NANO_BOARD_LED,(led_state)?(HIGH):(LOW));
  esc_step();
}

void setup_esc()
{
  pinMode(PIN_ESC_EN,OUTPUT);
  
  pinMode(PIN_ESC_AH,OUTPUT);
  pinMode(PIN_ESC_AL,OUTPUT);
  pinMode(PIN_ESC_BH,OUTPUT);
  pinMode(PIN_ESC_BL,OUTPUT);
  pinMode(PIN_ESC_CH,OUTPUT);
  pinMode(PIN_ESC_CL,OUTPUT);

  pinMode(PIN_ESC_SENSE0,INPUT);
  pinMode(PIN_ESC_SENSE1,INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_ESC_SENSE0), sense0_isr, FALLING);
}

char esc_state = 0;

void esc_step(bool dir=true) {
  if(dir) {
    esc_state++;
    if(esc_state>5) esc_state = 0;
  } else {
    if(esc_state<=0) esc_state = 5;
    else esc_state--;
  }
  
  int pinH = ESC_SEQUENCE[esc_state<<1];
  int pinL = ESC_SEQUENCE[(esc_state<<1)+1];
  /*PORTB &= 0b11111100;
  PORTD &= 0b00001111;
  digitalWrite(pinH,HIGH);
  digitalWrite(pinL,HIGH);*/

  // high side: low active
  char pb = PINB;
  char pd = PIND;
  pb &= 0b11111101; // PORT B LOW SIDE RESET
  pb |= 0b00000001; // PORT B HIGH SIDE RESET
  pd &= 0b01011111; // PORT D LOW SIDE RESET
  pd |= 0b01010000; // PORT D HIGH SIDE RESET
  PORTB = pb;
  PORTD = pd;
  digitalWrite(pinH,LOW);
  digitalWrite(pinL,HIGH);
}

void setup() {
  pinMode(NANO_BOARD_LED,OUTPUT);
  setup_esc();
}

void loop() {
  esc_step();
  int sensorValue = analogRead(A0);
  //digitalWrite(NANO_BOARD_LED,(sensorValue<511)?(LOW):(HIGH));
  delay((sensorValue>>3)+1);
}
