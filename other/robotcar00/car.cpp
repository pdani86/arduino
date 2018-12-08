
#include "car.h"

void Car::forward(byte val) {
  motorRight.pwm(val);
  motorLeft.pwm(val);
}

void Car::backward(byte val) {
  motorRight.pwm(val,false);
  motorLeft.pwm(val,false);
}

void Car::turn(byte val,bool right) {
  if(right) {
    motorRight.pwm(val,false);
    motorLeft.pwm(val,true);
  } else {
    motorRight.pwm(val,true);
    motorLeft.pwm(val,false);
  }
}

void Car::freeRun() {
  motorRight.freeRun();
  motorLeft.freeRun();
}

void Car::brake(byte val) {
  motorRight.brake(val);
  motorLeft.brake(val);
}

