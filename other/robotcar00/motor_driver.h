
#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

#include <Arduino.h>

class MyMotor
{
  byte enPin;
  byte in1Pin;
  byte in2Pin;
  bool _invertDir;

public:
  inline MyMotor(byte _enPin,byte _in1Pin,byte _in2Pin):enPin(_enPin),in1Pin(_in1Pin),in2Pin(_in2Pin) {_invertDir=false;}
  inline void init() {pinMode(enPin,OUTPUT); pinMode(in1Pin,OUTPUT); pinMode(in2Pin,OUTPUT); invertDir(true);}
  
  void pwm(byte val=255,bool dir = true);
  void brake(byte val=255,bool dir = false);
  void enable(bool en);

  inline void go(byte val=255,bool dir = true) {pwm(val,dir);}
  inline void freeRun(bool _free=true) {enable(!_free);}
  inline void invertDir(bool inv) {_invertDir=inv;};
};

#endif

