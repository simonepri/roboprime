/**
 * Part of RoboPrime Firmware.
 *
 * commandParser.h
 * Serial command interpreter.
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
 * @require       bodyMovement, animationStore
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */
 
/*
 * PURPOSE:
 *
 * This create a class to manage and interpret the incoming serial messages.
 *
 * Implemented S codes:
 * S0 - Set the default angle for a servo.
 * S1 - Set a angle width for a servo.
 * S2 - Sweep to an angle for a servo.
 * S3 - Apply an animation.
 *
 * Implemented Q codes:
 * Q0 - Plan a movment for a servo.
 *
 * Implemented C codes:
 * C0 - Calibrate servo bound.
 *
 */
 
#ifndef _COMMAND_PARSER_H
#define _COMMAND_PARSER_H

#define SERIAL_BAUD         115200
#define DEFAULT_CMD_IDX        255
#define DEFAULT_CODE_VALUE   65535

#define numIdx(num) num-'0'
#define alpIdx(chr) chr-'A'
#define usedCode(val) (val != DEFAULT_CODE_VALUE)

#define _A_ alpIdx('A')
#define _B_ alpIdx('B')
#define _C_ alpIdx('C')
#define _D_ alpIdx('D')
#define _E_ alpIdx('E')
#define _F_ alpIdx('F')
#define _G_ alpIdx('G')
#define _H_ alpIdx('H')
#define _I_ alpIdx('I')
#define _J_ alpIdx('J')
#define _K_ alpIdx('K')
#define _L_ alpIdx('L')
#define _M_ alpIdx('M')
#define _N_ alpIdx('N')
#define _O_ alpIdx('O')
#define _P_ alpIdx('P')
#define _Q_ alpIdx('Q')
#define _R_ alpIdx('R')
#define _S_ alpIdx('S')
#define _T_ alpIdx('T')
#define _U_ alpIdx('U')
#define _V_ alpIdx('V')
#define _W_ alpIdx('W')
#define _X_ alpIdx('X')
#define _Y_ alpIdx('Y')
#define _Z_ alpIdx('Z')

typedef struct cmd_t {
  bool isBusy;
  uint8_t firstCode, activeCode;
  uint16_t valueCode[_Z_ + 1];
};

class CommandParser {
  public:
    static void begin();
    static void parseSerial();
  private:
    // No-one have to create an istance of this class as we use it as
    // static method, so we keep constructor as private.
    CommandParser();
    
    static void parseByte(char _b);
    static void parseCode();
    
    static void parseCodeS();
    static void parseCodeS0();
    static void parseCodeS1();
    static void parseCodeS2();
    static void parseCodeS3();
    
    static void parseCodeQ();
    static void parseCodeQ0();
    static void parseCodeQ1();
    
    static void parseCodeC();
    static void parseCodeC0();
    
    static cmd_t parser;
};

#endif