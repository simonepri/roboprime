 /**
 * Part of RoboPrime Firmware.
 *
 * serialServo.cpp
 * Interrupt driven Serial Servo library for Arduino using 4017 Counter IC.
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
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */
 
/*
 * PURPOSE:
 *
 * The library uses two 4017 Counter IC to generate pulses to independently
 * drive up to 20 servos (10 servos for each 4017) from four Arduino Pins.
 * 
 * The interrupt are based on Timer1 output compare register A and Timer1 output compare register B:
 *  - OCR1A is linked to digital pin 9 and so we use digital pin 9 to
 *    generate the clock signal for the first 4017 counter (Bank A).
 *  - Pin 12 is used as the reset pin for the first 4017 counter (Bank A).
 * 
 *  - OCR1B is linked to digital pin 10 and so we use digital pin 10 to
 *    generate the clock signalfor the second 4017 counter (Bank B).
 *  - Pin 13 is used as the reset pin for the second 4017 counter (Bank B).
 * 
 * Thanks to DuaneB for the idea of using 4017.
 */

#ifndef _SERIAL_SERVO_H
#define _SERIAL_SERVO_H

#define SERIAL_SERVO_BANKA_LOW      0     // Bank A lower-bound index.
#define SERIAL_SERVO_BANKA_UP       9     // Bank A upper-bound index.
#define SERIAL_SERVO_BANKB_LOW     10     // Bank A lower-bound index.
#define SERIAL_SERVO_BANKB_UP      19     // Bank A upper-bound index.
#define SERIAL_SERVO_CHANNELS      20     // Number of channels.

#define SERIAL_SERVO_BANKS          2
#define SERIAL_SERVO_BANKA          0
#define SERIAL_SERVO_BANKB          1

#define BANKA_PULSE_PIN             9     
#define BANKA_RST_PIN              12     
#define BANKB_PULSE_PIN            10     
#define BANKB_RST_PIN              13     

#define PRESCALER_BITS              2     // 010.
#define PRESCALER_VALUE             8     // Timer1 prescaler. 8 = 2 ticks = 1us. (With 16MHz clock)
#define START_DELAY_MS              2     // Initial delay for interrupt start.

#define MIN_SERVO_ANGLE             0
#define MAX_SERVO_ANGLE          1800     //180*10

#define BOUND_MIN 0
#define BOUND_MAX 1
#define BOUND_SIZE 2

#define SERVO_WIDTH_BOUND {                                                    \
	{550, 2200}, {480, 2100}, {640, 2360}, {560, 2260}, {580, 2200},             \
	{600, 2260}, {550, 2300}, {630, 2220}, {650, 2280}, {750, 2440},             \
	{590, 2300}, {650, 2350}, {640, 2630}, {610, 2370}, {520, 2250},             \
	{600, 2190}, {590, 2270}, {520, 2100}, {550, 2220}, {720, 2230}              \
}

#define ticksToUs(ticks)     ((ticks) >> 1)        // ((_ticks * PRESCALER_VALUE)/ clockCyclesPerMicrosecond())
#define usToTicks(us)        ((us) << 1)
#define msToTicks(ms)        (usToTicks(uint32_t(ms)) * 1000)
#define ticksToMs(ticks)     (ticksToUs(ticks) / 1000)

#define fastMsToTicks(ms)        (usToTicks(ms) << 10)
#define fastTicksToMs(ticks)     (ticksToUs(ticks) >> 10)

#define PORTB_PIN(_pin) (1 << (_pin-8))

typedef struct servo_data_t {
	bool pulseReached;
	float eachusTicks;
	uint16_t lastUpdate;
	int32_t actionTicks;
	volatile bool updateDisabled;
	volatile int16_t deltaTicks;
	volatile float incrementTicks;
	volatile uint16_t pulseTicks;
};
		
class SerialServo {
	public:
		static void begin();

		static bool isValidChannel(uint8_t _ch);
		static void writeWidth(uint8_t _ch, uint16_t _us, bool _inverted = false, bool _calibration = false);
		static void writeAngle(uint8_t _ch, uint16_t _deg, bool _inverted = false);
		static uint16_t readWidth(uint8_t _ch, bool _inverted = false);
		static uint16_t readAngle(uint8_t _ch, bool _inverted = false);
		static void sweepWidth(uint8_t _ch, uint16_t _us, uint16_t _time,
		                       bool _inverted = false);
		static void sweepAngle(uint8_t _ch, uint16_t _deg, uint16_t _time,
		                       bool _inverted = false);
		static void wait(uint8_t _ch, uint16_t _time);
		static uint16_t readMinWidth(uint8_t _ch, bool _inverted = false);
		static uint16_t readMaxWidth(uint8_t _ch, bool _inverted = false);
		static bool isMoving(uint8_t _ch);
		static void enableSequence();
		static void disableSequence();
		
		static void servoRoutine();
		
		static void OCR1A_ISR();
		static void OCR1B_ISR();
	private:
		// No-one have to create an istance of this class as we use it as
		// static method, so we keep constructor as private.
		SerialServo();                                              

		//All raw_ function declared here do not check the validity of the passed
		//argument for speed reasons. Are supposed to be used only interneally by 
		//this class and need to be declared as protected.
		
		static uint16_t raw_degToUs(const uint8_t &_ch, const uint16_t &_deg);
		static uint16_t raw_usToDeg(const uint8_t &_ch, const uint16_t &_us);
		static uint16_t raw_validWidth(const uint8_t &_ch, const uint16_t &_us);
		static uint16_t raw_validAngle(const uint8_t &_ch, const uint16_t &_deg);
		static uint16_t raw_invertWidth(const uint8_t &_ch, const uint16_t &_us);
		static uint16_t raw_invertAngle(const uint8_t &_ch, const uint16_t &_deg);
		
		static void raw_writeWidth(const uint8_t &_ch, const uint16_t &_us);
		static uint16_t raw_readWidth(const uint8_t &_ch);
		static void raw_sweepWidth(const uint8_t &_ch, const uint16_t &_us,
		                           const uint16_t &_time);
		static void raw_wait(const uint8_t &_ch, const uint16_t &_time);
		static uint16_t raw_readMinWidth(const uint8_t &_ch);
		static uint16_t raw_readMaxWidth(const uint8_t &_ch);
		
		static void raw_movementCheck();
		static void raw_incrementCalculator();
		
		static bool
			sequence;
		static volatile uint16_t 
			channel[SERIAL_SERVO_BANKS];
		static volatile uint16_t 
			period[SERIAL_SERVO_BANKS];
		static servo_data_t data[SERIAL_SERVO_CHANNELS];
		static const PROGMEM uint16_t
			bound[SERIAL_SERVO_CHANNELS][BOUND_SIZE];
};

#endif