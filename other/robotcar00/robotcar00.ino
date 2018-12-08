#include <IRremote.h>

#define IR_RECV_PIN A2

IRrecv irrecv(IR_RECV_PIN);
decode_results ir_results;

class MyMotor
{
  byte enPin;
  byte in1Pin;
  byte in2Pin;
  bool _invertDir;
  
public:
  inline MyMotor(byte _enPin,byte _in1Pin,byte _in2Pin):enPin(_enPin),in1Pin(_in1Pin),in2Pin(_in2Pin) {_invertDir=false;}
  inline void init() {pinMode(enPin,OUTPUT); pinMode(in1Pin,OUTPUT); pinMode(in2Pin,OUTPUT);}
  
  void pwm(byte val=255,bool dir = true);
  void brake(byte val=255,bool dir = false);
  void enable(bool en);

  inline void go(byte val=255,bool dir = true) {pwm(val,dir);}
  inline void freeRun(bool free=true) {enable(!free);}
  inline void invertDir(bool inv) {_invertDir=inv;};
};

void MyMotor::enable(bool en) {
  digitalWrite(enPin,en);
}

void MyMotor::pwm(byte val,bool dir) {
  if(_invertDir) dir = !dir;
  analogWrite(enPin,val);
  digitalWrite(in1Pin,dir);
  digitalWrite(in2Pin, (0==val)?dir:(!dir) );
}

void MyMotor::brake(byte val,bool dir) {
  analogWrite(enPin,val);
  digitalWrite(in1Pin,dir);
  digitalWrite(in2Pin,dir);
}

#define REAR_IR_PIN 4

#define EN_RIGHT_SIDE_PIN 5
#define PWM_RIGHT_SIDE_1_PIN A0
#define PWM_RIGHT_SIDE_2_PIN A1

#define EN_LEFT_SIDE_PIN 6
#define PWM_LEFT_SIDE_1_PIN 7
#define PWM_LEFT_SIDE_2_PIN 8

MyMotor motorRight(EN_RIGHT_SIDE_PIN,PWM_RIGHT_SIDE_1_PIN,PWM_RIGHT_SIDE_2_PIN);
MyMotor motorLeft(EN_LEFT_SIDE_PIN,PWM_LEFT_SIDE_1_PIN,PWM_LEFT_SIDE_2_PIN);

/*
class Car
{
  MyMotor leftMotor;
  MyMotor rightMotor
  public:
  inline Car(byte leftEnPin,left)
    :leftMotor(),
    rightMotor() {
      }
  inline void init() {leftMotor.init(); rightMotor.init();}
};*/

void setup() {
  pinMode(REAR_IR_PIN,INPUT);
  irrecv.enableIRIn();
  motorLeft.init();
  motorRight.init();
  motorRight.invertDir(true);
  motorLeft.invertDir(true);
}

void forward(byte val) {
  motorRight.pwm(val);
  motorLeft.pwm(val);
}

void backward(byte val) {
  motorRight.pwm(val,false);
  motorLeft.pwm(val,false);
}

void turn(byte val,bool right) {
  if(right) {
    motorRight.pwm(val,false);
    motorLeft.pwm(val,true);
  } else {
    motorRight.pwm(val,true);
    motorLeft.pwm(val,false);
  }
}

void free_run() {
  motorRight.freeRun();
  motorLeft.freeRun();
}

void brake(byte val) {
  motorRight.brake(val);
  motorLeft.brake(val);
}

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

enum IR_COMMAND {
  
  IRC_PWM_MAX,
  IRC_PWM_INC = 0x00ffa857,
  IRC_PWM_DEC = 0x00ffe01f,
  IRC_BRAKE_PWM_MAX,

  IRC_FORWARD = 0x00ff18e7,
  IRC_BACKWARD = 0x00ff4ab5,
  IRC_TURN_RIGHT = 0x00ff5aa5,
  IRC_TURN_LEFT = 0x00ff10ef,

  IRC_BRAKE = 0x00ff38c7,
  
  IRC_FREE_RUN = 0x00ff6897,
};

unsigned long last_ir_value = 0;
unsigned long last_ir_cmd_timems = 0;

void handleIRcommand() {
  unsigned long val = ir_results.value;
  if(val==0xffffffff) val = last_ir_value;
  else last_ir_value = val;

  last_ir_cmd_timems = millis();
  
  switch(val) {
    case IRC_FORWARD:
      settings.motion_state = FORWARD;
    break;
    case IRC_BACKWARD:
      settings.motion_state = BACKWARD;
    break;
    case IRC_BRAKE:
      settings.motion_state = BRAKE;
    break;
    case IRC_FREE_RUN:
      settings.motion_state = FREE_RUN;
    break;
    case IRC_TURN_RIGHT:
      settings.motion_state = TURN_RIGHT;
    break;
    case IRC_TURN_LEFT:
      settings.motion_state = TURN_LEFT;
    break;
    case IRC_PWM_INC:
      settings.pwm = (settings.pwm>=250)?(255):(settings.pwm+5);
    break;
    case IRC_PWM_DEC:
      settings.pwm = (settings.pwm<=5)?(0):(settings.pwm-5);
    break;
    
    /*case 0xffffffff:
    break;*/
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
    case FREE_RUN: free_run(); break;
    case FORWARD: forward(settings.pwm); break;
    case TURN_RIGHT: turn(settings.pwm,true); break;
    case TURN_LEFT: turn(settings.pwm,false); break;
    case BRAKE: brake(settings.brakePwm); break;
    case BACKWARD: backward(settings.pwm); break;
  }
}

void loop() {

  bool cmdReceived = pollIRremote();

  unsigned long long dtms = millis() - last_ir_cmd_timems;
  if(dtms>5000 || (dtms>500 && settings.motion_state!=FREE_RUN)) settings.motion_state = BRAKE;
  bool obstacleRear = !digitalRead(REAR_IR_PIN);
  if(obstacleRear && settings.motion_state!=FORWARD) {
    settings.motion_state = BRAKE;
  } else {
    
  }
  updateMotorControl();
}
