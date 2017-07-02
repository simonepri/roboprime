/**
 * Part of RoboPrime Firmware.
 *
 * main.cpp
 * Setup for library and main loop.
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
 
#include "Arduino.h"
#include "serialServo.h"
#include "bodyMovement.h"
#include "commandParser.h"
#include "animationStore.h"

void setup() {
  Serial.begin(SERIAL_BAUD);
  SerialServo::begin();
  BodyMovement::begin();
  AnimationStore::begin();
  CommandParser::begin();
}

void loop() {
  /*static uint32_t _time;
  uint32_t _time2 = micros();
  Serial.println(_time2-_time);
  _time = _time2;*/
  SerialServo::servoRoutine();
  BodyMovement::movementPlanner();
  AnimationStore::executeAnimation();
  CommandParser::parseSerial();
}