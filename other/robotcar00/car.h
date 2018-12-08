#ifndef __CAR_H__
#define __CAR_H__

#include <Arduino.h>

#include "motor_driver.h"

class Car
{
  MyMotor motorLeft;
  MyMotor motorRight;
  public:
  inline Car(
    byte leftEnPin,byte leftIn1Pin,byte leftIn2Pin,
    byte rightEnPin,byte rightIn1Pin,byte rightIn2Pin
    )
    :motorLeft(leftEnPin,leftIn1Pin,leftIn2Pin),
    motorRight(rightEnPin,rightIn1Pin,rightIn2Pin) {}
    
  inline void init() {motorLeft.init(); motorRight.init();}

  void forward(byte val=255);
  void brake(byte val=255);
  void backward(byte val=255);
  void turn(byte val,bool right);
  void freeRun();
  inline void left(byte val=255) {turn(val,false);}
  inline void right(byte val=255) {turn(val,true);}
};

#endif

