

#ifndef __IR_CMDS_H__
#define __IR_CMDS_H__

namespace IR_REMOTE_CODES {
  namespace REMOTE_TYPE_CARMP3 {
    enum REMOTE_BTN {
      CHANNEL_MINUS = 0x00ffa25d,
      CHANNEL = 0x00ff629d,
      CHANNEL_PLUS = 0x00ffe21d,
      PREV = 0x00ff22dd,
      NEXT = 0x00ff02fd,
      PLAY_PAUSE = 0x00ffc23d,
      VOLUME_MINUS = 0x00ffe01f,
      VOLUME_PLUS = 0x00ffa857,
      EQ = 0x00ff906f,
      NUM_0 = 0x00ff6897,
      PLUS_100 = 0x00ff9867,
      PLUS_200 = 0x00ffb04f,
      NUM_1 = 0x00ff30cf,
      NUM_2 = 0x00ff18e7,
      NUM_3 = 0x00ff7a85,
      NUM_4 = 0x00ff10ef,
      NUM_5 = 0x00ff38c7,
      NUM_6 = 0x00ff5aa5,
      NUM_7 = 0x00ff42bd,
      NUM_8 = 0x00ff4ab5,
      NUM_9 = 0x00ff52ad
    };
  }

  /*
  namespace REMOTE_TYPE_SAMSUNG {
   enum REMOTE_BTN {
     VOLUME_MINUS = ,
     VOLUME_PLUS = ,
   }
  }*/
}

namespace IR_CMD {
  namespace IRCMD {
    using namespace IR_REMOTE_CODES::REMOTE_TYPE_CARMP3;
    enum IR_COMMAND {
      PWM_MAX,
      PWM_INC = VOLUME_PLUS,
      PWM_DEC = VOLUME_MINUS,
      BRAKE_PWM_MAX,
      FORWARD = NUM_2,
      BACKWARD = NUM_8,
      TURN_RIGHT = NUM_6,
      TURN_LEFT = NUM_4,
      BRAKE = NUM_5,
      FREE_RUN = NUM_0,
      TOGGLE_OBSTACLE_IGNORING = EQ,
      TOGGLE_SMOOTH_TORQUE = PLAY_PAUSE,
      TOGGLE_LAMPS = CHANNEL
    };
  }
}

#endif

