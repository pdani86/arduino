#ifndef __RADIO_CONTROL_H__
#define __RADIO_CONTROL_H__

struct RADIO_CONTROL_PACKET
{
  char steer;
  byte throttle;
  byte brake;
  byte clutch;
  char gear;
  byte lamps;
  //byte 
};

struct RADIO_REPORT
{
  unsigned int forwardClearanceCm;
  bool obstacleRear;
  
};

class RadioControl
{
public:
  
};

#endif

