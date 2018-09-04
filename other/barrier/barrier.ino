
#define LED_PIN 9
#define TRIGGER_IN_PIN 6
#define SERVO_PIN 4
void setup() {
  pinMode(TRIGGER_IN_PIN,INPUT);
  pinMode(LED_PIN,OUTPUT);
  pinMode(SERVO_PIN,OUTPUT);
  digitalWrite(SERVO_PIN,LOW);
}

unsigned long time = 0;

unsigned long last_servo_impulse = 0;

void servo_impulse(int us) {
  last_servo_impulse = millis();
  digitalWrite(SERVO_PIN,HIGH);
  delayMicroseconds(us);
  digitalWrite(SERVO_PIN,LOW);
}

int servo_state = 0;
unsigned long last_servo_switch = 0;

void switch_servo_state() {
  last_servo_switch = millis();
  servo_state = (servo_state+1)%3;
}

void loop() {
  time = millis();
  bool trigger_in = digitalRead(TRIGGER_IN_PIN);
  unsigned long servo_pulse_dtms = time - last_servo_impulse;
  unsigned long servo_dtms = time - last_servo_switch;

  unsigned char val = (time % 4096) >> 4;
  bool parity = 0==(time / 4096)%2;
  if(parity) val = 255-val;

  if(servo_dtms > 2000) switch_servo_state();
  
  if(servo_pulse_dtms >= 20) {
    int servo_val;
    //servo_val = 800 + 700*servo_state;
    /*servo_val = (int)(1400*((time % 4096)/4095.0f));
    if(parity) servo_val = 2200 - servo_val;
    else servo_val = 800 + servo_val;*/
    servo_val = (trigger_in)?800:2200;
    servo_impulse(servo_val);
  }
  
  analogWrite(LED_PIN,val);
}
