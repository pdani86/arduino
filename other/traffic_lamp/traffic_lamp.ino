#include "trafficlamp.h"

class LampController {
public:
  inline LampController(TrafficLamp* lamp) {
    _lamp = lamp;
    _redSec = 20;
    _greenSec = 15;
  }
  void update();
private:
  TrafficLamp* _lamp;
  int _redSec;
  int _greenSec;
};

void LampController::update() {
  if(_lamp->isInTransition()) {_lamp->update(); return;}
  
  int mod = ((millis()/1000)%(_redSec + _greenSec));
  bool shouldBeGreen = (mod >= _redSec);
  
  TrafficLamp::STATE curState = _lamp->getState();
  if(shouldBeGreen && curState!=TrafficLamp::GREEN) {
      _lamp->switchToGreen();
  } else if(!shouldBeGreen && curState!=TrafficLamp::RED) {
    _lamp->switchToRed();
  } else {
    _lamp->update();
  }
}

TrafficLamp lamp;
LampController lampControl(&lamp);

//#define _USE_SERIAL_LOG

void setup() {
  pinMode(13,OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(2), blink, FALLING);
  lamp.setup();
  lamp.switchToBlinkingYellow();
#ifdef _USE_SERIAL_LOG
  Serial.begin(9600);
#endif
}

long long last_serial_msg_timems = 0;

void loop() {
  lampControl.update();
  bool isGreen = (lamp.getState()==TrafficLamp::GREEN);
  digitalWrite(13,isGreen);
#ifdef _USE_SERIAL_LOG
  long long now = millis();
  long long serial_dt_ms = now - last_serial_msg_timems;
  if(serial_dt_ms > 1500) {
      Serial.println("State:");
      Serial.println(lamp.getState());
      last_serial_msg_timems = now;
  }
#endif
}
