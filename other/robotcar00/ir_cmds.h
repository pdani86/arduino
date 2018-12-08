

#ifndef __IR_CMDS_H__
#define __IR_CMDS_H__

namespace IR_CMD {
  namespace REMOTE_TYPE_CARMP3 {
    namespace IRCMD {
      enum IR_COMMAND {
        PWM_MAX,
        PWM_INC = 0x00ffa857,
        PWM_DEC = 0x00ffe01f,
        BRAKE_PWM_MAX,
        FORWARD = 0x00ff18e7,
        BACKWARD = 0x00ff4ab5,
        TURN_RIGHT = 0x00ff5aa5,
        TURN_LEFT = 0x00ff10ef,
        BRAKE = 0x00ff38c7,
        FREE_RUN = 0x00ff6897,
      };
    }
  }
/*
  namespace REMOTE_TYPE_SAMSUNG {
      namespace IRCMD {
        enum IR_COMMAND {
          PWM_MAX,
          PWM_INC = ,
          PWM_DEC = ,
          BRAKE_PWM_MAX,
          FORWARD = ,
          BACKWARD = ,
          TURN_RIGHT = ,
          TURN_LEFT = ,
          BRAKE = ,
          FREE_RUN = ,
        };
      }
    }*/
  
}

#endif

