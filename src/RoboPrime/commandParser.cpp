/**
 * Part of RoboPrime Firmware.
 *
 * commandParser.cpp
 * Serial command interpreter.
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
 * @require       bodyMovement, animationStore
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */
 
#include "Arduino.h"
#include "commandParser.h"

#include "serialServo.h"
#include "bodyMovement.h"
#include "animationStore.h"

/**
 * parser struct is located in SRAM momery and store information about the parsed
 * command, the struct is cleared after each command is successfully executed.
 **/
cmd_t
  CommandParser::parser;

/**
 * Method variables intialization.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::begin() {
	parser.firstCode = DEFAULT_CMD_IDX;
	parser.activeCode = DEFAULT_CMD_IDX;
	for(uint8_t _cmd = _A_; _cmd <= _Z_; _cmd++) {
		parser.valueCode[_cmd] = DEFAULT_CODE_VALUE;
	}
}

/**
 * This routine is called by the loop and parse the serial commands.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseSerial() {
	if(!parser.isBusy) {
		if(Serial.available() > 0) {
			parseByte(Serial.read());
		}
	}
	else {
		parseByte('\n');
	}
}

/**
 * Parse the input byte.
 *
 * @param character.
 * @return none.
 **/
void CommandParser::parseByte(char _b) {
	if(_b == ' ') {
		parser.activeCode = DEFAULT_CMD_IDX;
		return;
	}
	else if('0' <= _b && _b <= '9') {
		if(parser.activeCode == DEFAULT_CMD_IDX) {
			return;
		}
		if(!usedCode(parser.valueCode[parser.activeCode])) {
			parser.valueCode[parser.activeCode] = 0;
		}
		parser.valueCode[parser.activeCode] *= 10;
		parser.valueCode[parser.activeCode] += numIdx(_b);
	}
	else if('A' <= _b && _b <= 'Z') {
		parser.activeCode = alpIdx(_b);
		if(usedCode(parser.valueCode[parser.activeCode])) {
			parser.activeCode = DEFAULT_CMD_IDX;
			return;
		}
		if(parser.firstCode == DEFAULT_CMD_IDX) {
			parser.firstCode = parser.activeCode;
		}
	}
	else if(_b == '\n' || _b == '\r' || _b == '\r\n') {
		parseCode();
		if(parser.isBusy) {
			return;
		}
		parser.firstCode = DEFAULT_CMD_IDX;
		parser.activeCode = DEFAULT_CMD_IDX;
		for(uint8_t _cmd = _A_; _cmd <= _Z_; _cmd++) {
			parser.valueCode[_cmd] = DEFAULT_CODE_VALUE;
		}
	}
}

/**
 * Parse codes.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCode() {
	switch(parser.firstCode) {
		case _S_: parseCodeS(); return;
		case _Q_: parseCodeQ(); return;
		case _C_: parseCodeC(); return;
	}
}

/**
 * Parse the S codes.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeS() {
	switch(parser.valueCode[_S_]) {
		case 0: parseCodeS0(); return;
		case 1: parseCodeS1(); return;
		case 2: parseCodeS2(); return;
		case 3: parseCodeS3(); return;
	}
}

/**
 * S0
 * R<index[0-9](optional)> or L<index[0-9](optional)>
 * Set the default angle for a servo.
 * If no index is passed all servos will be resetted.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeS0() {
	if(usedCode(parser.valueCode[_L_])) {
		BodyMovement::setDefault(HF_L, parser.valueCode[_L_]);
	}
	else if(usedCode(parser.valueCode[_R_])) {
		BodyMovement::setDefault(HF_R, parser.valueCode[_R_]);
	}
	else {
		BodyMovement::setDefault();
	}
}

/**
 * S1
 * R<index[0-9]> or L<index[0-9]> A<angle[deg*10]>
 * Set an angle for a servo.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeS1() {
	if((!usedCode(parser.valueCode[_L_]) && !usedCode(parser.valueCode[_R_])) || 
	   !usedCode(parser.valueCode[_A_])) {
		return;
	}
	if(usedCode(parser.valueCode[_L_])) {
		BodyMovement::setPos(HF_L, parser.valueCode[_L_],
		                           parser.valueCode[_A_]);
	}
	else {
		BodyMovement::setPos(HF_R, parser.valueCode[_R_],
		                           parser.valueCode[_A_]);
	}
}

/**
 * S2
 * R<index[0-9]> or L<index[0-9]> A<angle[deg*10]> T<duration[ms]>
 * Sweep to a pulse width for a servo.
 * 
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeS2() {
	if((!usedCode(parser.valueCode[_L_]) && !usedCode(parser.valueCode[_R_])) ||
	   !usedCode(parser.valueCode[_A_])  || !usedCode(parser.valueCode[_T_])) {
		return;
	}
	if(usedCode(parser.valueCode[_L_])) {
		BodyMovement::setSweep(HF_L, parser.valueCode[_L_],
		                             parser.valueCode[_A_],
													       parser.valueCode[_T_]);
	}
	else {
		BodyMovement::setSweep(HF_R, parser.valueCode[_R_],
		                             parser.valueCode[_A_],
													       parser.valueCode[_T_]);
	}
}

/**
 * S3
 * A<animation[]> D<distance[ms]> T<duration[ms]>
 * Apply an animation.
 * 
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeS3() {
	if(!usedCode(parser.valueCode[_A_]) ||
	   !usedCode(parser.valueCode[_D_]) ||
     !usedCode(parser.valueCode[_T_])) {
		AnimationStore::clearAnimation(true);
		return;
	}
	AnimationStore::applyAnimation(parser.valueCode[_A_],
	                               parser.valueCode[_D_],
																 parser.valueCode[_T_]);
}

/**
 * Parse the Q codes.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeQ() {
	switch(parser.valueCode[_Q_]) {
		case 0: parseCodeQ0(); return;
		case 1: parseCodeQ1(); return;
	}
}

/**
 * Q0
 * R<index[0-9]> or L<index[0-9]> A<angle[deg*10](optional)> D<duration[ms]>
 * Plan a movment for a servo.
 * If 'A' is not passed or is seted to 0 a pause will be planned instead.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeQ0() {
	if((!usedCode(parser.valueCode[_L_]) && !usedCode(parser.valueCode[_R_])) ||
	   !usedCode(parser.valueCode[_D_])) {
		return;
	}
	if(!usedCode(parser.valueCode[_A_])) {
		parser.valueCode[_A_] = 0;
	}
	if(usedCode(parser.valueCode[_L_])) {
		bool _inserted = BodyMovement::pushQueue(HF_L, parser.valueCode[_L_],
		                                               parser.valueCode[_A_],
		                                               parser.valueCode[_D_]);
		if(!_inserted) {
			parser.isBusy = true;
			return;
		}
	}
	else {
		bool _inserted = BodyMovement::pushQueue(HF_R, parser.valueCode[_R_],
		                                               parser.valueCode[_A_],
		                                               parser.valueCode[_D_]);
		if(!_inserted) {
			parser.isBusy = true;
			return;
		}
	}
	parser.isBusy = false;
}

/**
 * Q1
 * A<antimation[]> D<duration[ms]> S<space[cm]>
 * Plan an animation.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeQ1() {
	if(!usedCode(parser.valueCode[_A_]) ||
	   !usedCode(parser.valueCode[_D_]) ||
	   !usedCode(parser.valueCode[_S_])) {
		return;
	}
}

/**
 * Parse the C codes.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeC() {
	switch(parser.valueCode[_C_]) {
		case 0: parseCodeC0(); return;
	}
}

/**
 * C0
 * R<index[0-9]> or L<index[0-9]> W<pulse witdh[us]>
 * Set a specific pulse width for calibration purposes.
 *
 * @param none.
 * @return none.
 **/
void CommandParser::parseCodeC0() {
	if((!usedCode(parser.valueCode[_L_]) && !usedCode(parser.valueCode[_R_]))
     || !usedCode(parser.valueCode[_W_])) {
		return;
	}
  uint8_t _ch = 0;
	if(usedCode(parser.valueCode[_L_])) {
    _ch = HF_NUM + parser.valueCode[_L_];
	}
	else {
    _ch = parser.valueCode[_R_];
	}
  SerialServo::writeWidth(_ch, parser.valueCode[_W_], false, true);
}