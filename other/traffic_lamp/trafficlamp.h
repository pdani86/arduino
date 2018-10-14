#ifndef __TRAFFIC_LAMP_H
#define	__TRAFFIC_LAMP_H

#import <Arduino.h>

class TrafficLamp {
public:
  enum STATE {
    OFF,
    BLINKING_YELLOW,
    RED,
    RED_TO_GREEN,
    GREEN,
    GREEN_TO_RED
  };
  
  inline TrafficLamp() {
    state = OFF;
    pinRed = 6;
    pinYellow = 5;
    pinGreen = 4;
    yellowBeforeRedMs = 3000;
    yellowRedBeforeGreenMs = 3000;
    yellowBlinkPeriodMs = 2000;
  }
  
  void setup();
  void update();
  
  void switchToRed();
  void switchToGreen();
  void switchToBlinkingYellow();
  void switchOff();

  inline STATE getState() {return state;}
  inline bool isInTransition() {return (state==RED_TO_GREEN)||(state==GREEN_TO_RED);}
private:
  STATE state;
  
  unsigned char pinRed;
  unsigned char pinYellow;
  unsigned char pinGreen;
  
  long long yellowBeforeRedMs;
  long long yellowRedBeforeGreenMs;
  long long yellowBlinkPeriodMs;
  
  long long _transitionStartTimeMs;
};


#endif
