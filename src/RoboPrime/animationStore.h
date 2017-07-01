/**
 * Part of RoboPrime Firmware.
 *
 * animationStore.cpp
 * Store motor movments to archive animations.
 *
 * RoboPrime Firmware, (https://github.com/simonepri/RoboPrime)
 * Copyright (c) 2015, Simone Primarosa, (https://simoneprimarosa.com)
 *
 * Licensed under The MIT License
 * Redistribution of file must retain the above copyright notice.
 *
 * @copyright     Copyright (c) 2015, Simone Primarosa, (https://simoneprimarosa.com)
 * @link          (https://github.com/simonepri/RoboPrime)
 * @since         0.0.0
 * @require       SerialServo
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */

/*
 * PURPOSE:
 *
 * This creates a class that adds movements to the queue in order to achieve 
 * some basic robot animations.
 * The code for the animations can be generated with the AnimHelper program.
 *
 * Implemented animations:
 * BASIC MOVMENTS:
 *  - Forward walk.                         WIP
 *  - Backward walk.                        NO
 *  - Side walk to right.                   NO
 *  - Side walk to left.                    NO
 *  - Clockwise standstill rotation.        NO
 *  - Counterclockwise standstill rotation. NO
 *  - Clockwise curved walk.                NO
 *  - Counterclockwise curved walk.         NO
 *  - Upward lift.                          NO
 *  - Downward lift.                        NO
 *  - Sit down.                             DONE
 *  - Hello.                                DONE
 *  - Fuck off.                             DONE
 */

#ifndef _ANIMATION_STORE_H
#define _ANIMATION_STORE_H

#define ANIM_FWW                  0
#define ANIM_BWW                  1
#define ANIM_SWR                  2
#define ANIM_SWL                  3
#define ANIM_CWSR                 4
#define ANIM_CCWSR                5
#define ANIM_CWCW                 6
#define ANIM_CCWCW                7
#define ANIM_SIT                  8
#define ANIM_HR                   9
#define ANIM_FOR                 10

#define ANIM_SIZE                 11
#define ANIM_NULL                255

#define ANIM_STEPS_START          0
#define ANIM_STEPS_LOOP           1
#define ANIM_STEPS_END            2
#define ANIM_STEPS_DIV            3

#define ANIM_STEPS_HF             0
#define ANIM_STEPS_PART           1
#define ANIM_STEPS_POS            2
#define ANIM_STEPS_TIME           3
#define ANIM_STEPS_INFO           4

#define ANIM_STEPS_SIZE {         \
  {2, 43, 0},                     \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {0, 0, 0},                      \
  {56, 0, 0},                     \
  {5, 2, 0},                      \
  {8, 9, 0},                     \
}

#define ANIM_FWW_SIZE             45
#define ANIM_BWW_SIZE             1
#define ANIM_SWR_SIZE             1
#define ANIM_SWL_SIZE             1
#define ANIM_CWSR_SIZE            1
#define ANIM_CCWSR_SIZE           1
#define ANIM_CWCW_SIZE            1
#define ANIM_CCWCW_SIZE           1
#define ANIM_SIT_SIZE             56
#define ANIM_HR_SIZE              7
#define ANIM_FOR_SIZE             17

#define ANIM_FWW_STEPS {                               \
  {HF_L, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 500},  \
  {HF_L, PART_SHOULDER_Y_ROT, 500, 250},               \
  {HF_L, PART_ANKLE_X_ROT, INVALID_BODY_POS, 500},     \
  {HF_L, PART_ANKLE_X_ROT, 850, 500},                  \
  {HF_R, PART_ANKLE_X_ROT, INVALID_BODY_POS, 500},     \
  {HF_R, PART_ANKLE_X_ROT, 1050, 500},                 \
  {HF_L, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 250},  \
  {HF_L, PART_SHOULDER_Y_ROT, 300, 250},               \
  {HF_R, PART_KNEE_X_ROT, INVALID_BODY_POS, 2000},     \
  {HF_R, PART_KNEE_X_ROT, 1100, 500},                  \
  {HF_L, PART_KNEE_X_ROT, INVALID_BODY_POS, 2000},     \
  {HF_L, PART_KNEE_X_ROT, 1325, 500},                  \
  {HF_R, PART_HIP_Y_ROT, INVALID_BODY_POS, 2000},      \
  {HF_R, PART_HIP_Y_ROT, 600, 500},                    \
  {HF_R, PART_HIP_X_ROT, INVALID_BODY_POS, 2000},      \
  {HF_R, PART_HIP_X_ROT, 1000, 500},                   \
  {HF_R, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 3000}, \
  {HF_R, PART_SHOULDER_Y_ROT, 500, 250},               \
  {HF_R, PART_ANKLE_X_ROT, INVALID_BODY_POS, 2000},    \
  {HF_R, PART_ANKLE_X_ROT, 850, 500},                  \
  {HF_L, PART_ANKLE_X_ROT, INVALID_BODY_POS, 2000},    \
  {HF_L, PART_ANKLE_X_ROT, 1050, 500},                 \
  {HF_R, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 250},  \
  {HF_R, PART_SHOULDER_Y_ROT, 300, 250},               \
  {HF_R, PART_KNEE_X_ROT, INVALID_BODY_POS, 1500},     \
  {HF_R, PART_KNEE_X_ROT, 1300, 500},                  \
  {HF_R, PART_HIP_Y_ROT, INVALID_BODY_POS, 1500},      \
  {HF_R, PART_HIP_Y_ROT, 850, 500},                    \
  {HF_L, PART_ANKLE_X_ROT, INVALID_BODY_POS, 2000},    \
  {HF_L, PART_ANKLE_X_ROT, 900, 500},                  \
  {HF_R, PART_ANKLE_X_ROT, INVALID_BODY_POS, 2000},    \
  {HF_R, PART_ANKLE_X_ROT, 900, 500},                  \
  {HF_L, PART_KNEE_X_ROT, INVALID_BODY_POS, 3000},     \
  {HF_L, PART_KNEE_X_ROT, 1300, 500},                  \
  {HF_R, PART_HIP_Y_ROT, INVALID_BODY_POS, 1000},      \
  {HF_R, PART_HIP_Y_ROT, 850, 500},                    \
  {HF_L, PART_HIP_Y_ROT, INVALID_BODY_POS, 5500},      \
  {HF_L, PART_HIP_Y_ROT, 850, 500},                    \
  {HF_R, PART_HIP_X_ROT, INVALID_BODY_POS, 3000},      \
  {HF_R, PART_HIP_X_ROT, 900, 500},                    \
  {HF_R, PART_KNEE_X_ROT, INVALID_BODY_POS, 1500},     \
  {HF_R, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 2250}, \
  {HF_L, PART_SHOULDER_Y_ROT, INVALID_BODY_POS, 4750}, \
}
#define ANIM_BWW_STEPS {                    \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_SWR_STEPS {                    \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_SWL_STEPS {                    \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_CWSR_STEPS {                   \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_CCWSR_STEPS {                  \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_CWCW_STEPS {                   \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_CCWCW_STEPS {                  \
  {HF_SIZE, PART_SIZE, INVALID_BODY_POS, 0} \
}
#define ANIM_SIT_STEPS {                                 \
  {HF_R, PART_ANKLE_X_ROT, 950, 1000},                   \
  {HF_L, PART_ANKLE_X_ROT, 950, 1000},                   \
  {HF_R, PART_ANKLE_Y_ROT, 1800, 1000},                  \
  {HF_L, PART_ANKLE_Y_ROT, 1800, 1000},                  \
  {HF_L, PART_KNEE_X_ROT, 0, 1000},                      \
  {HF_R, PART_KNEE_X_ROT, 0, 1000},                      \
  {HF_R, PART_HIP_Y_ROT, 500, 1000},                     \
  {HF_L, PART_HIP_Y_ROT, 500, 1000},                     \
  {HF_L, PART_SHOULDER_X_ROT, INVALID_BODY_POS, 1000},   \
  {HF_L, PART_SHOULDER_X_ROT, 500, 1000},                \
  {HF_R, PART_SHOULDER_X_ROT, INVALID_BODY_POS, 1000},   \
  {HF_R, PART_SHOULDER_X_ROT, 500, 1000},                \
  {HF_L, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 1000},      \
  {HF_L, PART_ELBOW_Z_ROT, 500, 1000},                   \
  {HF_R, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 1000},      \
  {HF_R, PART_ELBOW_Z_ROT, 500, 1000},                   \
  {HF_L, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
  {HF_L, PART_ELBOW_X_ROT, 1500, 1000},                  \
  {HF_R, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
  {HF_R, PART_ELBOW_X_ROT, 1500, 1000},                  \
  {HF_R, PART_HIP_Y_ROT, INVALID_BODY_POS, 1000},        \
  {HF_R, PART_HIP_Y_ROT, 1000, 1000},                    \
  {HF_L, PART_HIP_Y_ROT, INVALID_BODY_POS, 1000},        \
  {HF_L, PART_HIP_Y_ROT, 1000, 1000},                    \
  {HF_L, PART_SHOULDER_X_ROT, 0, 1000},                  \
  {HF_R, PART_SHOULDER_X_ROT, 0, 1000},                  \
  {HF_R, PART_ANKLE_Y_ROT, INVALID_BODY_POS, 2000},      \
  {HF_R, PART_ANKLE_Y_ROT, 1200, 1000},                  \
  {HF_L, PART_ANKLE_Y_ROT, INVALID_BODY_POS, 2000},      \
  {HF_L, PART_ANKLE_Y_ROT, 1200, 1000},                  \
  {HF_L, PART_KNEE_X_ROT, INVALID_BODY_POS, 2000},       \
  {HF_L, PART_KNEE_X_ROT, 1400, 1000},                   \
  {HF_R, PART_KNEE_X_ROT, INVALID_BODY_POS, 2000},       \
  {HF_R, PART_KNEE_X_ROT, 1400, 1000},                   \
  {HF_R, PART_HIP_Y_ROT, 100, 1000},                     \
  {HF_L, PART_HIP_Y_ROT, 0, 1000},                       \
  {HF_R, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
  {HF_R, PART_ELBOW_X_ROT, 1200, 1000},                  \
  {HF_L, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
  {HF_L, PART_ELBOW_X_ROT, 1200, 1000},                  \
  {HF_R, PART_SHOULDER_X_ROT, INVALID_BODY_POS, 1000},   \
  {HF_R, PART_SHOULDER_X_ROT, 900, 1000},                \
  {HF_L, PART_SHOULDER_X_ROT, INVALID_BODY_POS, 1000},   \
  {HF_L, PART_SHOULDER_X_ROT, 900, 1000},                \
  {HF_R, PART_ANKLE_X_ROT, INVALID_BODY_POS, 4000},      \
  {HF_R, PART_ANKLE_Y_ROT, INVALID_BODY_POS, 1000},      \
  {HF_R, PART_KNEE_X_ROT, INVALID_BODY_POS, 1000},       \
  {HF_R, PART_HIP_Y_ROT, INVALID_BODY_POS, 1000},        \
  {HF_R, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 3000},      \
  {HF_R, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
  {HF_L, PART_ANKLE_X_ROT, INVALID_BODY_POS, 4000},      \
  {HF_L, PART_ANKLE_Y_ROT, INVALID_BODY_POS, 1000},      \
  {HF_L, PART_KNEE_X_ROT, INVALID_BODY_POS, 1000},       \
  {HF_L, PART_HIP_Y_ROT, INVALID_BODY_POS, 1000},        \
  {HF_L, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 3000},      \
  {HF_L, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1000},      \
}
#define ANIM_HR_STEPS {                                  \
  {HF_R, PART_SHOULDER_X_ROT, 1600, 1000},               \
  {HF_R, PART_SHOULDER_Y_ROT, 400, 1000},                \
  {HF_R, PART_ELBOW_Z_ROT, 900, 1000},                   \
  {HF_R, PART_ELBOW_X_ROT, 400, 1000},                   \
  {HF_R, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 1000},      \
                                                         \
  {HF_R, PART_ELBOW_Z_ROT, 500, 1000},                   \
  {HF_R, PART_ELBOW_Z_ROT, 1300, 1000},                  \
}
#define ANIM_FOR_STEPS {                                 \
  {HF_R, PART_SHOULDER_X_ROT, 1500, 1000},               \
  {HF_L, PART_SHOULDER_X_ROT, 1500, 1000},               \
  {HF_L, PART_SHOULDER_Y_ROT, 0, 1000},                  \
  {HF_R, PART_SHOULDER_Y_ROT, 100, 1000},                \
  {HF_R, PART_ELBOW_Z_ROT, 600, 1000},                   \
  {HF_L, PART_ELBOW_Z_ROT, 0, 1000},                     \
  {HF_R, PART_ELBOW_X_ROT, 600, 1000},                   \
  {HF_L, PART_ELBOW_X_ROT, 400, 1000},                   \
  {HF_R, PART_SHOULDER_X_ROT, INVALID_BODY_POS, 1000},   \
  {HF_R, PART_SHOULDER_X_ROT, 1800, 500},                \
  {HF_L, PART_ELBOW_Z_ROT, INVALID_BODY_POS, 1250},      \
  {HF_L, PART_ELBOW_Z_ROT, 500, 250},                    \
  {HF_R, PART_ELBOW_X_ROT, INVALID_BODY_POS, 1250},      \
  {HF_R, PART_ELBOW_X_ROT, 400, 250},                    \
  {HF_R, PART_SHOULDER_X_ROT, 1500, 500},                \
  {HF_L, PART_ELBOW_Z_ROT, 0, 500},                      \
  {HF_R, PART_ELBOW_X_ROT, 600, 500},                    \
}

typedef struct anim_t {
  bool busyAnimation, endingAnimation;
  uint8_t activeAnimation, stepAnimation,
          startAnimation, loopAnimation, endAnimation;
  uint16_t distAnimation, timeAnimation, angleAnimation;
};

typedef const PROGMEM uint8_t steps_size_t;
typedef const PROGMEM uint16_t steps_info_t;

class AnimationStore {
  public:
    static void begin();
    static void applyAnimation(uint8_t _anim, uint16_t _dist,
                               uint16_t _time, uint16_t _angle = 0);
    static void clearAnimation(bool _force = false);
    static void executeAnimation();
  private:
    // No-one have to create an istance of this class as we use it as
    // a singleton, so we keep constructor as private.
    AnimationStore();

    static void nextStep(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepFWW(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepBWW(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepSWR(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepSWL(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepCWSR(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepCCWSR(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepCWCW(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepCCWCW(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepSIT(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepHR(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static void nextStepFOR(bool &_half, uint8_t &_idx, uint16_t &_angle, uint16_t &_time);
    static anim_t anim;
    static steps_size_t steps_size[ANIM_SIZE][3];
    static steps_info_t steps_FWW[ANIM_FWW_SIZE][4];
    static steps_info_t steps_BWW[ANIM_BWW_SIZE][4];
    static steps_info_t steps_SWR[ANIM_SWR_SIZE][4];
    static steps_info_t steps_SWL[ANIM_SWL_SIZE][4];
    static steps_info_t steps_CWSR[ANIM_CWSR_SIZE][4];
    static steps_info_t steps_CCWSR[ANIM_CCWSR_SIZE][4];
    static steps_info_t steps_CWCW[ANIM_CWCW_SIZE][4];
    static steps_info_t steps_CCWCW[ANIM_CCWCW_SIZE][4];
    static steps_info_t steps_SIT[ANIM_SIT_SIZE][4];
    static steps_info_t steps_HR[ANIM_HR_SIZE][4];
    static steps_info_t steps_FOR[ANIM_FOR_SIZE][4];
};

#endif
