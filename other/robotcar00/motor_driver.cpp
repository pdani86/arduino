#include "motor_driver.h"

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

