#ifndef __RCCAR_CONTROLLER_H__
#define __RCCAR_CONTROLLER_H__

#include <Arduino.h>
#include "radio_control.h"



namespace RcCarController
{

class Feedback
{
  public:
  
};

class Joystick
{
  int xAxis;
  int yAxis;
  bool btn;
public:
  inline Joystick() {xAxis = yAxis = 0; btn = false;}
  inline void begin() {
    pinMode(_xPin,INPUT);
    pinMode(_yPin,INPUT);
    pinMode(_btnPin,INPUT);
  };
  void update();
  void fillControlPacket(RADIO_CONTROL_PACKET* packet);
private:
  byte _xPin;
  byte _yPin;
  byte _btnPin;
};

void Joystick::update();
  analogRead(_xPin);
  xAxis = analogRead(_xPin);
  analogRead(_yPin);
  yAxis = analogRead(_yPin);
  btn = digitalRead(_btnPin);
}

void Joystick::fillControlPacket(RADIO_CONTROL_PACKET* packet) {
  packet->steer = map(xAxis,0,1023,-128,127);
  packet->throttle = map(abs(yAxis-512),0,512,0,255);
  packet->gear = (yAxis<512)?(-1):(1);
  packet->lamps = (btn)?(0xff):(0x00);
}

#endif

