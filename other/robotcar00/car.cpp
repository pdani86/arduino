
#include "car.h"

void Car::forward(byte val) {
  if(differentialDrive) {
    byte angle = steeringServo.read();
    byte leftVal = val;
    byte rightVal = val;
    if(angle<90) {
      leftVal = map(angle,45,90,0,val);
      //if(leftVal<100) leftVal = 100;
    } else {
      rightVal = map(angle,90,135,val,0);
    }
    
    motorRight.pwm(rightVal);
    motorLeft.pwm(leftVal);
  } else {
    motorRight.pwm(val);
    motorLeft.pwm(val);
  }
}

void Car::backward(byte val) {
  if(differentialDrive) {
    byte angle = steeringServo.read();
    byte leftVal = val;
    byte rightVal = val;
    if(angle<90) {
      leftVal = map(angle,45,90,0,val);
      //if(leftVal<100) leftVal = 100;
    } else {
      rightVal = map(angle,90,135,val,0);
    }
    motorRight.pwm(rightVal,false);
    motorLeft.pwm(leftVal,false);
  } else {
    motorRight.pwm(val,false);
    motorLeft.pwm(val,false);
  }
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

