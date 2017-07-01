/**
 * Part of RoboPrime Firmware.
 *
 * animationStore.cpp
 * Store motor movments to archive animations.
 *
 * RoboPrime Firmware, (https://github.com/simonepri/RoboPrime)
 * Copyright (c) 2015, Simone Primarosa,(http://simoneprimarosa.com)
 *
 * Licensed under The MIT License
 * Redistribution of file must retain the above copyright notice.
 *
 * @copyright     Copyright (c) 2015, Simone Primarosa,(http://simoneprimarosa.com)
 * @link          (https://github.com/simonepri/RoboPrime)
 * @since         0.0.0
 * @require       bodyMovement
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */

#include "Arduino.h"
#include "bodyMovement.h"

#include "animationStore.h"

/**
 * anim struct is located in SRAM momery and store information about the current
 * applied animation, the struct is cleared after each command is successfully executed.
 **/
anim_t
  AnimationStore::anim;

/**
 * "steps_size" array is located in FLASH memory and store information about the
 * number of steps of each animation.
 **/
steps_size_t
  AnimationStore::steps_size[ANIM_SIZE][ANIM_STEPS_DIV] = ANIM_STEPS_SIZE;

/**
 * "steps_*" arrays are located in FLASH memory and they store information about the
 * bodypart to move on each step for each animation.
 **/
steps_info_t
  AnimationStore::steps_FWW[ANIM_FWW_SIZE][ANIM_STEPS_INFO] = ANIM_FWW_STEPS;
steps_info_t
  AnimationStore::steps_BWW[ANIM_BWW_SIZE][ANIM_STEPS_INFO] = ANIM_BWW_STEPS,
  AnimationStore::steps_SWR[ANIM_SWR_SIZE][ANIM_STEPS_INFO] = ANIM_SWR_STEPS,
  AnimationStore::steps_SWL[ANIM_SWL_SIZE][ANIM_STEPS_INFO] = ANIM_SWL_STEPS,
  AnimationStore::steps_CWSR[ANIM_CWSR_SIZE][ANIM_STEPS_INFO] = ANIM_CWSR_STEPS,
  AnimationStore::steps_CCWSR[ANIM_CCWSR_SIZE][ANIM_STEPS_INFO] = ANIM_CCWSR_STEPS,
  AnimationStore::steps_CWCW[ANIM_CWCW_SIZE][ANIM_STEPS_INFO] = ANIM_CWCW_STEPS,
  AnimationStore::steps_CCWCW[ANIM_CCWCW_SIZE][ANIM_STEPS_INFO] = ANIM_CCWCW_STEPS,
  AnimationStore::steps_SIT[ANIM_SIT_SIZE][ANIM_STEPS_INFO] = ANIM_SIT_STEPS,
  AnimationStore::steps_HR[ANIM_HR_SIZE][ANIM_STEPS_INFO] = ANIM_HR_STEPS,
  AnimationStore::steps_FOR[ANIM_FOR_SIZE][ANIM_STEPS_INFO] = ANIM_FOR_STEPS;

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::begin() {
  clearAnimation(true);
}
/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::applyAnimation(uint8_t _anim, uint16_t _dist,
                                    uint16_t _time, uint16_t _angle) {
  BodyMovement::setSequence(true);
  anim.activeAnimation = _anim;
  anim.stepAnimation = 0;
  anim.timeAnimation = _time;
  anim.distAnimation = _dist;
  anim.angleAnimation = _angle;

  anim.startAnimation = pgm_read_word_near(&(steps_size[_anim][ANIM_STEPS_START]));
  anim.loopAnimation = pgm_read_word_near(&(steps_size[_anim][ANIM_STEPS_LOOP]));
  anim.endAnimation = pgm_read_word_near(&(steps_size[_anim][ANIM_STEPS_END]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::clearAnimation(bool _force) {
  if(_force) {
    BodyMovement::setSequence(false);
    anim.activeAnimation = ANIM_NULL;
    anim.endingAnimation = false;
    anim.stepAnimation = 0;
    anim.timeAnimation = 0;
    anim.distAnimation = 0;
    anim.angleAnimation = 0;
  }
  else {
    anim.endingAnimation = true;
  }
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::executeAnimation() {
  static bool _half;
  static uint8_t _idx;
  static uint16_t _angle, _time;
  if(anim.activeAnimation == ANIM_NULL) {
    return;
  }
  if(!anim.busyAnimation) {
    nextStep(_half, _idx, _angle, _time);
  }
  bool _inserted = BodyMovement::pushQueue(_half, _idx, _angle, _time);
  if(!_inserted) {
    anim.busyAnimation = true;
    return;
  }
  anim.busyAnimation = false;
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStep(bool &_half, uint8_t &_idx, uint16_t &_angle,
                              uint16_t &_time) {
  if(!anim.endingAnimation && anim.loopAnimation == 0 && anim.stepAnimation == anim.startAnimation) {
    return;
  }
  else if(!anim.endingAnimation && anim.stepAnimation == anim.startAnimation + anim.loopAnimation) {
    anim.stepAnimation = anim.startAnimation;
  }
  else if(anim.stepAnimation == anim.startAnimation + anim.loopAnimation + anim.endAnimation) {
    return clearAnimation(true);
  }

  switch(anim.activeAnimation) {
    case ANIM_FWW: nextStepFWW(_half, _idx, _angle, _time); break;
    case ANIM_BWW: nextStepBWW(_half, _idx, _angle, _time); break;
    case ANIM_SWR: nextStepSWR(_half, _idx, _angle, _time); break;
    case ANIM_SWL: nextStepSWL(_half, _idx, _angle, _time); break;
    case ANIM_CWSR: nextStepCWSR(_half, _idx, _angle, _time); break;
    case ANIM_CCWSR: nextStepCCWSR(_half, _idx, _angle, _time); break;
    case ANIM_CWCW: nextStepCWCW(_half, _idx, _angle, _time); break;
    case ANIM_CCWCW: nextStepCCWCW(_half, _idx, _angle, _time); break;
    case ANIM_SIT: nextStepSIT(_half, _idx, _angle, _time); break;
    case ANIM_HR: nextStepHR(_half, _idx, _angle, _time); break;
    case ANIM_FOR: nextStepFOR(_half, _idx, _angle, _time); break;
  }
  anim.stepAnimation++;
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepFWW(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_FWW[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_FWW[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_FWW[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_FWW[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepBWW(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_BWW[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_BWW[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_BWW[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_BWW[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepSWR(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_SWR[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_SWR[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_SWR[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_SWR[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepSWL(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_SWL[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_SWL[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_SWL[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_SWL[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepCWSR(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                  uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_CWSR[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_CWSR[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_CWSR[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_CWSR[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepCCWSR(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                   uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_CCWSR[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_CCWSR[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_CCWSR[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_CCWSR[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepCWCW(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                  uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_CWCW[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_CWCW[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_CWCW[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_CWCW[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepCCWCW(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                   uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_CCWCW[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_CCWCW[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_CCWCW[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_CCWCW[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepSIT(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_SIT[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_SIT[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_SIT[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_SIT[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepHR(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_HR[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_HR[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_HR[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_HR[anim.stepAnimation][ANIM_STEPS_TIME]));
}

/**
 *
 *
 * @param .
 * @return none.
 **/
void AnimationStore::nextStepFOR(bool &_half, uint8_t &_idx, uint16_t &_angle,
                                 uint16_t &_time) {
  _half = pgm_read_word_near(&(steps_FOR[anim.stepAnimation][ANIM_STEPS_HF]));
  _idx = pgm_read_word_near(&(steps_FOR[anim.stepAnimation][ANIM_STEPS_PART]));
  _angle = pgm_read_word_near(&(steps_FOR[anim.stepAnimation][ANIM_STEPS_POS]));
  _time = pgm_read_word_near(&(steps_FOR[anim.stepAnimation][ANIM_STEPS_TIME]));
}
