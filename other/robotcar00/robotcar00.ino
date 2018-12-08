
#include <IRremote.h>
#include "ir_cmds.h"
#include "car.h"

using namespace IR_CMD::REMOTE_TYPE_CARMP3; // constants for IR commands (buttons)

#define IR_RECV_PIN A2
#define REAR_IR_PIN 4

#define EN_RIGHT_SIDE_PIN 5

#define PWM_RIGHT_SIDE_1_PIN A0
#define PWM_RIGHT_SIDE_2_PIN A1

#define EN_LEFT_SIDE_PIN 6
#define PWM_LEFT_SIDE_1_PIN 7
#define PWM_LEFT_SIDE_2_PIN 8

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

  MOTION_STATE motion_state;

  inline SETTINGS() {pwm = 255; brakePwm = 255; motion_state = BRAKE;}
};

SETTINGS settings;


unsigned long last_ir_value = 0;
unsigned long last_ir_cmd_timems = 0;

void handleIRcommand() {
  unsigned long val = ir_results.value;
  if(val==0xffffffff) val = last_ir_value;
  else last_ir_value = val;

  last_ir_cmd_timems = millis();
  
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
      settings.pwm = (settings.pwm>=250)?(255):(settings.pwm+5);
    break;
    case IRCMD::PWM_DEC:
      settings.pwm = (settings.pwm<=5)?(0):(settings.pwm-5);
    break;
    default:
    break;
  }
}

bool pollIRremote() {
  unsigned long irValue = 0;
  if(irrecv.decode(&ir_results)) {
    handleIRcommand(); // (ir_results is global variable)
    irrecv.resume();
    return true;
  }
  return false;
}

void updateMotorControl() {
  switch(settings.motion_state) {
    case FREE_RUN: car.freeRun(); break;
    case FORWARD: car.forward(settings.pwm); break;
    case TURN_RIGHT: car.turn(settings.pwm,true); break;
    case TURN_LEFT: car.turn(settings.pwm,false); break;
    case BRAKE: car.brake(settings.brakePwm); break;
    case BACKWARD: car.backward(settings.pwm); break;
  }
}

void setup() {
  pinMode(REAR_IR_PIN,INPUT);
  car.init();
}

void loop() {
  bool cmdReceived = pollIRremote();
  unsigned long long dtms = millis() - last_ir_cmd_timems;
  if(dtms>5000 || (dtms>500 && settings.motion_state!=FREE_RUN)) settings.motion_state = BRAKE;
  bool obstacleRear = !digitalRead(REAR_IR_PIN);
  if(obstacleRear && settings.motion_state!=FORWARD) {
    settings.motion_state = BRAKE;
  }
  updateMotorControl();
  delay(10);
}

