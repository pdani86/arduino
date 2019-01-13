
#include <IRremote.h>
#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

#include "ir_cmds.h"
#include "car.h"
#include "radio_control.h"

#define RF24_CE_PIN A3
#define RF24_CS_PIN A4
#define RADIO_CHANNEL 29
#define RC_ADDRESS 0x0086102400

RF24 radio(RF24_CE_PIN,RF24_CS_PIN);

RADIO_CONTROL_PACKET last_rc_packet;


void setup_radio() {
  radio.begin();
  //radio.setPALevel(RF24_PA_LOW);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setAutoAck(false);
  //radio.setRetries(2,15);
  radio.setChannel(RADIO_CHANNEL);
  radio.openReadingPipe(1,RC_ADDRESS);
  radio.startListening();
}

using namespace IR_CMD; // constants for IR commands (buttons)

#define IR_RECV_PIN A2
#define REAR_IR_PIN 4

#define EN_RIGHT_SIDE_PIN 5
#define MOTOR_RIGHT_SIDE_1_PIN A0
#define MOTOR_RIGHT_SIDE_2_PIN A1
#define EN_LEFT_SIDE_PIN 6
#define MOTOR_LEFT_SIDE_1_PIN 7
#define MOTOR_LEFT_SIDE_2_PIN 8

#define STEERING_SERVO_PIN 10

#define LAMP_PIN 9

IRrecv irrecv(IR_RECV_PIN);
decode_results ir_results;

Car car(
  EN_LEFT_SIDE_PIN, MOTOR_LEFT_SIDE_1_PIN, MOTOR_LEFT_SIDE_2_PIN,
  EN_RIGHT_SIDE_PIN, MOTOR_RIGHT_SIDE_1_PIN, MOTOR_RIGHT_SIDE_2_PIN,
  STEERING_SERVO_PIN
  );

enum MOTION_STATE
{
  FREE_RUN,
  BRAKE,
  FORWARD,
  BACKWARD,
  TURN_RIGHT,
  TURN_LEFT
};

struct SETTINGS
{
  byte pwm;
  byte brakePwm;
  bool obstacleIgnoring;
  bool smoothTorque;
  byte servo_state;

  MOTION_STATE motion_state;

  inline SETTINGS() {
    pwm = 255;
    brakePwm = 255;
    motion_state = BRAKE;
    obstacleIgnoring = false;
    smoothTorque = false;
    servo_state = 90;
  }
};

SETTINGS settings;
Servo& myservo = car.getSteeringServo();

unsigned long last_rc_cmd_timems = 0;

void handleRadioControlPacket(RADIO_CONTROL_PACKET* packet) {
  //byte servo_val = map(packet->steer,0,255,5,175);
  //byte servo_val = map(packet->steer,-128,127,5,175);
  byte servo_val = map(packet->steer,-128,127,45,135);
  servo_val = 180-servo_val;
  settings.servo_state = servo_val;
  myservo.write(servo_val);
  settings.pwm = packet->throttle;
  const byte MIN_THROTTLE = 80;

  if(packet->throttle>=MIN_THROTTLE) {
    if(packet->gear>0) settings.motion_state = FORWARD;
    if(packet->gear<0) settings.motion_state = BACKWARD;
  } else {
    settings.motion_state = BRAKE;
    settings.pwm = 0;
  }
  if(abs(packet->steer) >= MIN_THROTTLE/2
    && digitalRead(LAMP_PIN) // HACK!!!
    ) {
    settings.motion_state = (packet->steer>0)?(TURN_LEFT):(TURN_RIGHT);
    settings.pwm = (packet->steer-1)*2;
  }
}

bool poll_radio() {
  if(radio.available()) {
    radio.read(&last_rc_packet,sizeof(RADIO_CONTROL_PACKET));
    last_rc_cmd_timems = millis();
    handleRadioControlPacket(&last_rc_packet);
    return true;
  }
  return false;
}


unsigned long last_ir_value = 0;


int pwmStep = 12;
byte servoStep = 6;

void handleIRcommand() {
  unsigned long val = ir_results.value;
  if(val==0xffffffff) val = last_ir_value;
  else last_ir_value = val;

  last_rc_cmd_timems = millis();

//Serial.println(val,HEX);
  
  switch(val) {
    case IRCMD::FORWARD:
      settings.motion_state = FORWARD;
    break;
    case IRCMD::BACKWARD:
      settings.motion_state = BACKWARD;
    break;
    case IRCMD::BRAKE:
      settings.motion_state = BRAKE;
    break;
    case IRCMD::FREE_RUN:
      settings.motion_state = FREE_RUN;
    break;
    case IRCMD::TURN_RIGHT:
      settings.motion_state = TURN_RIGHT;
    break;
    case IRCMD::TURN_LEFT:
      settings.motion_state = TURN_LEFT;
    break;
    case IRCMD::PWM_INC:
      settings.pwm = (settings.pwm>=255-pwmStep)?(255):(settings.pwm+pwmStep);
    break;
    case IRCMD::PWM_DEC:
      settings.pwm = (settings.pwm<=pwmStep)?(0):(settings.pwm-pwmStep);
    break;
    case IRCMD::TOGGLE_OBSTACLE_IGNORING:
      settings.obstacleIgnoring = !settings.obstacleIgnoring;
    break;
    case IRCMD::TOGGLE_SMOOTH_TORQUE:
     settings.smoothTorque = !settings.smoothTorque;
    break;
    case IRCMD::TOGGLE_LAMPS:
      digitalWrite(LAMP_PIN,!digitalRead(LAMP_PIN));
    break;
    case IRCMD::SERVO_CENTER:
      settings.servo_state = 90;
      myservo.write(settings.servo_state);
    break;
    case IRCMD::SERVO_PLUS:
      
      if(settings.servo_state<180-servoStep) settings.servo_state += servoStep;
      else settings.servo_state = 180;
      myservo.write(settings.servo_state);
    break;
    case IRCMD::SERVO_MINUS:
      if(settings.servo_state>servoStep) settings.servo_state -= servoStep;
      else settings.servo_state = 0;
      myservo.write(settings.servo_state);
    break;
    default:
    break;
  }
}

bool pollIRremote() {
  if(irrecv.decode(&ir_results)) {
    handleIRcommand(); // (ir_results is global variable)
    irrecv.resume();
    return true;
  }
  return false;
}
float _smoothPwm = 0.0;
unsigned long _lastSmoothTimeMs = 0;

void updateMotorControl() {
  byte pwm = settings.pwm;
  if(settings.motion_state == BRAKE) _smoothPwm = 0.0;
  if(settings.smoothTorque) {
    unsigned long now = millis();
    unsigned long dtms = now - _lastSmoothTimeMs;
    if(dtms!=0) {
      float diff = pwm - _smoothPwm;
      _smoothPwm = _smoothPwm + diff*dtms*0.001f;
      if(_smoothPwm > 255) pwm = 255;
      else if(_smoothPwm<0) pwm = 0;
      else pwm = _smoothPwm;
      _lastSmoothTimeMs = now;
    } else {
      pwm = _smoothPwm;
    }
  }
  
  switch(settings.motion_state) {
    case FREE_RUN: car.freeRun(); break;
    case FORWARD: car.forward(pwm); break;
    case TURN_RIGHT: car.turn(pwm,true); break;
    case TURN_LEFT: car.turn(pwm,false); break;
    case BRAKE: car.brake(settings.brakePwm); break;
    case BACKWARD: car.backward(pwm); break;
  }
}


void setup() {
  pinMode(REAR_IR_PIN,INPUT);
  pinMode(LAMP_PIN,OUTPUT);
  irrecv.enableIRIn();
  myservo.attach(10);
  myservo.write(settings.servo_state);
  //Serial.begin(9600);
  setup_radio();
  car.init();
}

void loop() {
  bool cmdReceived = pollIRremote() || poll_radio();
  unsigned long long dtms = millis() - last_rc_cmd_timems;
  if(dtms>5000 || (dtms>500 && settings.motion_state!=FREE_RUN)) settings.motion_state = BRAKE;
  bool obstacleRear = !digitalRead(REAR_IR_PIN);
  if(!settings.obstacleIgnoring && obstacleRear && settings.motion_state!=FORWARD) {
    settings.motion_state = BRAKE;
  }
  updateMotorControl();
  delay(5);
}

