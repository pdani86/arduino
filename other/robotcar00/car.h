#ifndef __CAR_H__
#define __CAR_H__

#include <Arduino.h>
#include <Servo.h>

#include "motor_driver.h"

class Car
{
  MyMotor motorLeft;
  MyMotor motorRight;
  Servo steeringServo;
  bool differentialDrive; // different PWM, when servo is not centered
public:
  inline Car(
    byte leftEnPin,byte leftIn1Pin,byte leftIn2Pin,
    byte rightEnPin,byte rightIn1Pin,byte rightIn2Pin,
    byte steeringServoPin
    )
    :motorLeft(leftEnPin,leftIn1Pin,leftIn2Pin),
    motorRight(rightEnPin,rightIn1Pin,rightIn2Pin),
    _steeringServoPin(steeringServoPin) {
        differentialDrive = true;
      }
    
  inline void init() {
    motorLeft.init();
    motorRight.init();
    steeringServo.attach(_steeringServoPin);
    steeringServo.write(90);
  }

  void forward(byte val=255);
  void brake(byte val=255);
  void backward(byte val=255);
  void turn(byte val,bool right);
  void freeRun();
  inline void left(byte val=255) {turn(val,false);}
  inline void right(byte val=255) {turn(val,true);}

  inline Servo& getSteeringServo() {return steeringServo;}

private:
  byte _steeringServoPin;
};

#endif

