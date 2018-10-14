#include "trafficlamp.h"

void TrafficLamp::setup() {
  pinMode(pinRed,OUTPUT);
  pinMode(pinYellow,OUTPUT);
  pinMode(pinGreen,OUTPUT);
}

void TrafficLamp::update() {
  long long now = millis();
  long long dt_ms;
  bool ryg[3] = {false,false,false};
  switch(state) {
    case OFF:
    break;
    case BLINKING_YELLOW:
      if(0==((millis()/(yellowBlinkPeriodMs>>1))%2)) ryg[1] = true;
    break;
    case RED:
      ryg[0] = true;
    break;
    case RED_TO_GREEN:
      dt_ms = now - _transitionStartTimeMs;
      if(dt_ms > yellowRedBeforeGreenMs) {
        state = GREEN;
        ryg[2] = true;
      } else {
        ryg[0] = true;
        ryg[1] = true;
      }
    break;
    case GREEN:
      ryg[2] = true;
    break;
    case GREEN_TO_RED:
      dt_ms = now - _transitionStartTimeMs;
      if(dt_ms > yellowBeforeRedMs) {
        state = RED;
        ryg[0] = true;
      } else {
        ryg[1] = true;
      }
    break;
    default:
    break;
  }
  digitalWrite(pinRed,ryg[0]);
  digitalWrite(pinYellow,ryg[1]);
  digitalWrite(pinGreen,ryg[2]);
}

void TrafficLamp::switchToRed() {
  if(state==RED || state==GREEN_TO_RED) return;
  state = GREEN_TO_RED;
  _transitionStartTimeMs = millis();
  update();
}

void TrafficLamp::switchToGreen() {
  if(state==GREEN || state==RED_TO_GREEN) return;
  state = RED_TO_GREEN;
  _transitionStartTimeMs = millis();
  update();
}

void TrafficLamp::switchToBlinkingYellow() {
  state = BLINKING_YELLOW;
  update();
}

void TrafficLamp::switchOff() {
  state = OFF;
  update();
}
