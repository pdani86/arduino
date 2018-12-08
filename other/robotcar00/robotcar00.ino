
#include <IRremote.h>
#include "ir_cmds.h"
#include "car.h"

using namespace IR_CMD; // constants for IR commands (buttons)

#define IR_RECV_PIN A2
#define REAR_IR_PIN 4

#define EN_RIGHT_SIDE_PIN 5

#define PWM_RIGHT_SIDE_1_PIN A0
#define PWM_RIGHT_SIDE_2_PIN A1

#define EN_LEFT_SIDE_PIN 6
#define PWM_LEFT_SIDE_1_PIN 7
#define PWM_LEFT_SIDE_2_PIN 8

#define LAMP_PIN 9

IRrecv irrecv(IR_RECV_PIN);
decode_results ir_results;

Car car(
  EN_LEFT_SIDE_PIN, PWM_LEFT_SIDE_1_PIN, PWM_LEFT_SIDE_2_PIN,
  EN_RIGHT_SIDE_PIN, PWM_RIGHT_SIDE_1_PIN, PWM_RIGHT_SIDE_2_PIN
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

  MOTION_STATE motion_state;

  inline SETTINGS() {pwm = 255; brakePwm = 255; motion_state = BRAKE; obstacleIgnoring = false; smoothTorque = false;}
};

SETTINGS settings;


unsigned long last_ir_value = 0;
unsigned long last_ir_cmd_timems = 0;

int pwmStep = 12;

void handleIRcommand() {
  unsigned long val = ir_results.value;
  if(val==0xffffffff) val = last_ir_value;
  else last_ir_value = val;

  last_ir_cmd_timems = millis();

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
  /*
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
  }*/
  
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
  //Serial.begin(9600);
  car.init();
}

void loop() {
  bool cmdReceived = pollIRremote();
  unsigned long long dtms = millis() - last_ir_cmd_timems;
  if(dtms>5000 || (dtms>500 && settings.motion_state!=FREE_RUN)) settings.motion_state = BRAKE;
  bool obstacleRear = !digitalRead(REAR_IR_PIN);
  if(!settings.obstacleIgnoring && obstacleRear && settings.motion_state!=FORWARD) {
    settings.motion_state = BRAKE;
  }
  updateMotorControl();
  delay(5);
}

