 /**
 * Part of RoboPrime Firmware.
 *
 * serialServo.cpp
 * Interrupt driven Serial Servo library for Arduino using 4017 Counter IC.
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
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */

#include "Arduino.h"
#include "serialServo.h"
/**
 * "sequence" variable is located in SRAM momery and is a flag in order to enable
 * time compensation for a squence of movments.
 **/
bool
  SerialServo::sequence;

/**
 * "channel" variable is located in SRAM momery and store the next channel that
 * will be uptated in the Interrupt Routine Service.
 **/
volatile uint16_t 
  SerialServo::channel[SERIAL_SERVO_BANKS];
  
/**
 * "period" variable is located in SRAM momery and store the time elapsed from
 * the last update of a channel.
 **/
volatile uint16_t 
  SerialServo::period[SERIAL_SERVO_BANKS];
  
/**
 * "data" array is located in SRAM momery and store information about the setted
 * pulse width for each channel.
 **/
servo_data_t
  SerialServo::data[SERIAL_SERVO_CHANNELS];

/**
 * "bound" array is located in FLASH memory and store information about the maximum
 * and minimum pulse width that can be setted for each channel.
 **/
const PROGMEM uint16_t
  SerialServo::bound[SERIAL_SERVO_CHANNELS][BOUND_SIZE] = SERVO_WIDTH_BOUND;

/**
 * Initialization of data array to default value, program each pin used as an
 * OUTPUT pin, send a reset pulse to the 4017 and start Timer1 interrupt.
 *
 * @param 
 * @return none.
 **/
void SerialServo::begin() {
  for(uint8_t _ch = 0; _ch < SERIAL_SERVO_CHANNELS; _ch++) {
    data[_ch].pulseReached = true;
    data[_ch].updateDisabled = true;
    data[_ch].incrementTicks = 0;
    data[_ch].eachusTicks = 0;
    data[_ch].pulseTicks = 0;
    data[_ch].lastUpdate = 0;
    data[_ch].actionTicks = 0;
  }
  period[SERIAL_SERVO_BANKA] = 0;
  period[SERIAL_SERVO_BANKB] = 0;
  channel[SERIAL_SERVO_BANKA] = SERIAL_SERVO_BANKA_LOW;
  channel[SERIAL_SERVO_BANKB] = SERIAL_SERVO_BANKB_LOW;
  
  // Set the pins we will use for Bank A (OCR1A) as outputs and pulse reset.
  pinMode(BANKA_PULSE_PIN,OUTPUT); 
  pinMode(BANKA_RST_PIN,OUTPUT);
  digitalWrite(BANKA_RST_PIN,HIGH);
  digitalWrite(BANKA_RST_PIN,LOW);
  // Set the pins we will use for Bank B (OCR1B) as outputs and pulse reset.
  pinMode(BANKB_PULSE_PIN,OUTPUT); 
  pinMode(BANKB_RST_PIN,OUTPUT);
  digitalWrite(BANKB_RST_PIN,HIGH);
  digitalWrite(BANKB_RST_PIN,LOW);

  TCNT1 = 0;                           // Clear the timer 1 count.
  TCCR1A = 0;                          // Normal counting mode for Timer1.
  TCCR1B = PRESCALER_BITS;             // Set prescaler for Timer1.

  // ENABLE TIMER1 OCR1A INTERRUPT to enabled the first bank (A) of 10 servos.
  TIFR1 |= _BV(OCF1A);                 // Clear any pending interrupts.
  TIMSK1 |=  _BV(OCIE1A);              // Enable the output compare interrupt.

  // ENABLE TIMER1 OCR1B INTERRUPT to enable the second bank (B) of 10 servos.
  TIFR1 |= _BV(OCF1B);                 // Clear any pending interrupts.
  TIMSK1 |=  _BV(OCIE1B);              // Enable the output compare interrupt.

  // Start OCR1A interrupt with an initial delay.
  uint16_t _TCNT1 = TCNT1;
  uint16_t _ticks = msToTicks((START_DELAY_MS));
  OCR1A = _TCNT1 + _ticks;
  _ticks += usToTicks(500);
  // Start OCR1B interrupt with an initial delay.
  OCR1B = _TCNT1 + _ticks;
}

/**
 * Check if a valid channel is passed
 *
 * @param channel index.
 * @return false if is a valid channel, false otherwise.
 **/
inline bool SerialServo::isValidChannel(uint8_t _ch) {
  return (_ch < SERIAL_SERVO_CHANNELS);
}

/**
 * Updates a channel to a new pulse width, the class will continue to pulse the
 * channel with this value for the lifetime of the MPU or until writeWidth or
 * writeAngle is called again to update the value.
 *
 * @param channel index, pulse width to set.
 * @return none.
 **/
void SerialServo::writeWidth(uint8_t _ch, uint16_t _us, bool _inverted, bool _calibration) {
  if(!isValidChannel(_ch)) {
    return;
  }
  if(sequence) {
    data[_ch].actionTicks -= TCNT1 - data[_ch].lastUpdate;
  }
  data[_ch].lastUpdate = TCNT1;
  if(!_calibration) {
    _us = raw_validWidth(_ch, _us);
    if(_inverted) {
      _us = raw_invertWidth(_ch, _us);
    }
  }
  raw_writeWidth(_ch, _us);
}

/**
 * Updates a channel to a new angle, the class will continue to pulse the
 * channel with this value for the lifetime of the MPU or until writeWidth or
 * writeAngle is called again to update the value.
 *
 * @param channel index, angle to set.
 * @return none.
 **/
void SerialServo::writeAngle(uint8_t _ch, uint16_t _deg, bool _inverted) {
  if(!isValidChannel(_ch)) {
    return;
  }
  if(sequence) {
    data[_ch].actionTicks -= TCNT1 - data[_ch].lastUpdate;
  }
  data[_ch].lastUpdate = TCNT1;
  _deg = raw_validAngle(_ch, _deg);
  if(_inverted) {
    _deg = raw_invertAngle(_ch, _deg);
  }
  _deg = raw_degToUs(_ch, _deg);
  raw_writeWidth(_ch, _deg);
}

/**
 * Read previously setted pulse width for a channel.
 *
 * @param channel index.
 * @return channel pulse width.
 **/
uint16_t SerialServo::readWidth(uint8_t _ch, bool _inverted) {
  if(!isValidChannel(_ch)) {
    return 0;
  }
  uint16_t _us = raw_readWidth(_ch);
  if(_inverted) {
    _us = raw_invertWidth(_ch, _us);
  }
  return _us;
}

/**
 * Read previously setted angle for a channel.
 *
 * @param channel index.
 * @return channel angle.
 **/
uint16_t SerialServo::readAngle(uint8_t _ch, bool _inverted) {
  if(!isValidChannel(_ch)) {
    return -1.0;
  }
  uint16_t _us = raw_readWidth(_ch);
  if(_inverted) {
    _us = raw_invertWidth(_ch, _us);
  }
  return raw_usToDeg(_ch, _us);
}

/**
 * Set a certain pulse width smoothly for a channel in a predeterminated number
 * of millisecond.
 *
 * @param channel index, pulse width, time to sweep.
 * @return none.
 **/
void SerialServo::sweepWidth(uint8_t _ch, uint16_t _us, uint16_t _time,
                             bool _inverted) {
  if(!isValidChannel(_ch)) {
    return;
  }
  if(sequence) {
    data[_ch].actionTicks -= TCNT1 - data[_ch].lastUpdate;
  }
  data[_ch].lastUpdate = TCNT1;
  _us = raw_validWidth(_ch, _us);
  if(_inverted) {
    _us = raw_invertWidth(_ch, _us);
  }
  raw_sweepWidth(_ch, _us, _time);
}

/**
 * Set an angle smoothly for a channel in a predeterminated number of
 * millisecond.
 *
 * @param channel index, angle, time to sweep.
 * @return none.
 **/
void SerialServo::sweepAngle(uint8_t _ch, uint16_t _deg, uint16_t _time,
                             bool _inverted) {
  if(!isValidChannel(_ch)) {
    return;
  }
  if(sequence) {
    data[_ch].actionTicks -= TCNT1 - data[_ch].lastUpdate;
  }
  data[_ch].lastUpdate = TCNT1;
  _deg = raw_validAngle(_ch, _deg);
  if(_inverted) {
    _deg = raw_invertAngle(_ch, _deg);
  }
  _deg = raw_degToUs(_ch, _deg);
  raw_sweepWidth(_ch, _deg, _time);
}

/**
 * Keep a channel in the actual position for a predeterminated number
 * of millisecond.
 *
 * @param channel index, time to wait.
 * @return none.
 **/
void SerialServo::wait(uint8_t _ch, uint16_t _time) {
  if(!isValidChannel(_ch)) {
    return;
  }
  if(sequence) {
    data[_ch].actionTicks -= TCNT1 - data[_ch].lastUpdate;
  }
  data[_ch].lastUpdate = TCNT1;
  raw_wait(_ch, _time);
}

/**
 * Get the minimum pulse width that can be setted to a channel.
 *
 * @param channel index.
 * @return min pulse width.
 **/
uint16_t SerialServo::readMinWidth(uint8_t _ch, bool _inverted) {
  if(!isValidChannel(_ch)) {
    return 0;
  }
  if(_inverted) {
    return raw_readMaxWidth(_ch);
  }
  return raw_readMinWidth(_ch);
}

/**
 * Get the maximum pulse width that can be setted to a channel.
 *
 * @param channel index.
 * @return max pulse width.
 **/
uint16_t SerialServo::readMaxWidth(uint8_t _ch, bool _inverted) {
  if(!isValidChannel(_ch)) {
    return 0;
  }
  if(_inverted) {
    return raw_readMinWidth(_ch);
  }
  return raw_readMaxWidth(_ch);
}

/**
 * Check if a specific channel has reached the planned position.
 *
 * @param channel index.
 * @return false if position is reached, false otherwise.
 **/
bool SerialServo::isMoving(uint8_t _ch) {
  if(!isValidChannel(_ch)) {
    return false;
  }
  return (data[_ch].actionTicks > 0);
}

/**
 * Enable sequence time compensation for sweep movments.
 *
 * @param none.
 * @return none.
 **/
void SerialServo::enableSequence() {
  sequence = true;
}

/**
 * Disable sequence time compensation for sweep movments.
 *
 * @param none.
 * @return none.
 **/
void SerialServo::disableSequence() {
  sequence = false;
}
 
/**
 * Convert deg to us.
 *
 * @param channel index, angle.
 * @return microseconds.
 **/
inline uint16_t SerialServo::raw_degToUs(const uint8_t &_ch,
                                         const uint16_t &_deg) {
  return (_deg - MIN_SERVO_ANGLE) *
         (float(raw_readMaxWidth(_ch) - raw_readMinWidth(_ch)) /
         (MAX_SERVO_ANGLE - MIN_SERVO_ANGLE)) +
         raw_readMinWidth(_ch);
}

/**
 * Convert us to deg.
 *
 * @param channel index, microseconds.
 * @return angle.
 **/
inline uint16_t SerialServo::raw_usToDeg(const uint8_t &_ch,
                                         const uint16_t &_us) {
  uint8_t _readus = raw_readMinWidth(_ch);
  return (_us - _readus) *
         (float(MAX_SERVO_ANGLE - MIN_SERVO_ANGLE) /
         (raw_readMaxWidth(_ch) - _readus)) +
         MIN_SERVO_ANGLE;
}

/**
 * Constrain a pulse width into our limit.
 *
 * @param microseconds.
 * @return constrained microseconds.
 **/
inline uint16_t SerialServo::raw_validWidth(const uint8_t &_ch,
                                            const uint16_t &_us) {
  uint8_t _readus = raw_readMinWidth(_ch);
  if(_us < _readus) {
    return _readus;
  }
  _readus = raw_readMaxWidth(_ch);
  if(_us > _readus) {
    return _readus;
  }
  return _us;
}

/**
 * Constrain an angle into our limit.
 *
 * @param angle.
 * @return constrained angle.
 **/
inline uint16_t SerialServo::raw_validAngle(const uint8_t &_ch,
                                            const uint16_t &_deg) {
  if(_deg < MIN_SERVO_ANGLE) {
    return MIN_SERVO_ANGLE;
  }
  if(_deg > MAX_SERVO_ANGLE) {
    return MAX_SERVO_ANGLE;
  }
  return _deg;
}

/**
 * Invert a pulse width into our limit.
 *
 * @param microseconds.
 * @return inverted microseconds.
 **/
inline uint16_t SerialServo::raw_invertWidth(const uint8_t &_ch,
                                             const uint16_t &_us) {
  return raw_readMaxWidth(_ch) + raw_readMinWidth(_ch) - _us;
}

/**
 * Invert an angle into our limit.
 *
 * @param angle.
 * @return inverted angle.
 **/
inline uint16_t SerialServo::raw_invertAngle(const uint8_t &_ch,
                                             const uint16_t &_deg) {
  return MAX_SERVO_ANGLE + MIN_SERVO_ANGLE - _deg;
}

/**
 * See writeWidth
 *
 * @param channel index, pulse width to set.
 * @return none.
 **/
inline void SerialServo::raw_writeWidth(const uint8_t &_ch,
                                        const uint16_t &_us) {
  data[_ch].updateDisabled = true;
  uint16_t _width = data[_ch].pulseTicks;
  data[_ch].incrementTicks = 0;

  data[_ch].actionTicks = 0;
  data[_ch].eachusTicks = 0;
  data[_ch].deltaTicks = usToTicks(_us) - _width;        // Save the wanted pulse width.
  
  data[_ch].pulseReached = false;       // Set the pulse width as not reached.
}

/**
 * See readWidth
 *
 * @param channel index.
 * @return channel pulse width.
 **/
inline uint16_t SerialServo::raw_readWidth(const uint8_t &_ch) {
  return data[_ch].pulseTicks;
}

//static uint32_t start[20];
/**
 * See sweepWidth
 *
 * @param channel index, pulse width, sweep time.
 * @return none.
 **/
inline void SerialServo::raw_sweepWidth(const uint8_t &_ch, const uint16_t &_us,
                                        const uint16_t &_time) {
  data[_ch].updateDisabled = true;
  uint16_t _width = data[_ch].pulseTicks;
  
  if(sequence) {
    data[_ch].actionTicks = msToTicks(_time) + data[_ch].actionTicks;
  }
  else {
    data[_ch].actionTicks = msToTicks(_time);
  }
  
  data[_ch].incrementTicks = 0;
  int16_t _delta_ticks = usToTicks(_us) - _width;
  float _us_ticks = data[_ch].actionTicks / float(_delta_ticks);
  data[_ch].eachusTicks = _us_ticks;
  data[_ch].deltaTicks = _delta_ticks;        // Save the wanted pulse width.
  
  data[_ch].pulseReached = false;       // Set the pulse width as not reached.
  
  //Serial.println(' ');
  //Serial.println(_time);
  //start[_ch] = millis();
}

/**
 * See wait
 *
 * @param channel index, wait time.
 * @return none.
 **/
inline void SerialServo::raw_wait(const uint8_t &_ch, const uint16_t &_time) {
  data[_ch].updateDisabled = true;
  
  if(sequence) {
    data[_ch].actionTicks = msToTicks(_time) + data[_ch].actionTicks;
  }
  else {
    data[_ch].actionTicks = msToTicks(_time);
  }
  
  data[_ch].eachusTicks = 0;
  data[_ch].deltaTicks = 0;
  data[_ch].incrementTicks = 0;
  
  data[_ch].pulseReached = false;       // Set the pulse width as not reached.
}


/**
 * See readMinWidth.
 *
 * @param channel index.
 * @return min pulse width.
 **/
inline uint16_t SerialServo::raw_readMinWidth(const uint8_t &_ch) {
  return pgm_read_word_near(&(bound[_ch][BOUND_MIN]));
}

/**
 * See readMaxWidth.
 *
 * @param channel index.
 * @return max pulse width.
 **/
inline uint16_t SerialServo::raw_readMaxWidth(const uint8_t &_ch) {
  return pgm_read_word_near(&(bound[_ch][BOUND_MAX]));
}

/**
 * Utility to check if is elapsed enought time to set the movment as completed.
 *
 * @param none.
 * @return none.
 **/
inline void SerialServo::raw_movementCheck() {
  static uint8_t _ch = SERIAL_SERVO_BANKA_LOW;
  
  uint16_t _elapsed = TCNT1 - data[_ch].lastUpdate;
  data[_ch].lastUpdate += _elapsed;
  
  if(!data[_ch].pulseReached) {
    data[_ch].actionTicks -= _elapsed;
    if(data[_ch].actionTicks <= 0) {
      data[_ch].eachusTicks = 0;
      if(data[_ch].deltaTicks) {
        data[_ch].incrementTicks = data[_ch].deltaTicks;
      }
      data[_ch].updateDisabled = false;             // ATOMIC BLOCK
      data[_ch].pulseReached = true;
      //Serial.println(millis()-start[_ch]);
    }
  }
  else {
    if(sequence) {
      data[_ch].actionTicks -= _elapsed;
    }
  }
  
  if(++_ch > SERIAL_SERVO_BANKB_UP) {
    _ch = SERIAL_SERVO_BANKA_LOW;
  }
}

/**
 * Utility to calculate the next pulse ticks increment.
 *
 * @param none.
 * @return none.
 **/
inline void SerialServo::raw_incrementCalculator() {
  static bool _block = SERIAL_SERVO_BANKA;
  static uint8_t _actual_ch[SERIAL_SERVO_BANKS];
  uint8_t _next_ch = channel[_block];
  if(_next_ch != _actual_ch[_block]) {
    if(data[_next_ch].eachusTicks) {
      data[_next_ch].incrementTicks += period[_block] /
                                       data[_next_ch].eachusTicks;
      data[_next_ch].updateDisabled = false;
    }
    _actual_ch[_block] = _next_ch;
  }
  _block = !_block;
}

/**
 * See raw_interrupt.
 *
 * @param none.
 * @return none.
 **/
 
void SerialServo::servoRoutine() {
  raw_movementCheck();
  raw_incrementCalculator();
}

/**
 * Send clock pulse for each channel to 4017 decade counter .
 * If the channel number is >= __block_upp, we need to reset the counter and
 * start again from __block_low.
 * To do this we pulse the reset pin of the counter this sets output 0 of the
 * counter high, effectivley starting the first pulse of our first channel.
 *
 * @param none.
 * @return none.
 **/
 
#define raw_interrupt(__block, __block_low, __block_upp, __timer_reg, __pin_reg, __pulse_pin, __reset_pin) {\
  static uint8_t _pin_to_pulse = __reset_pin;                                  \
                                                                               \
  __pin_reg |= _pin_to_pulse;                                                  \
  __pin_reg ^= _pin_to_pulse;                                                  \
                                                                               \
  sei();                                                                       \
  if(!data[ channel[__block] ].updateDisabled) {                               \
    int16_t _increment = data[ channel[__block] ].incrementTicks;              \
    data[ channel[__block] ].incrementTicks -= _increment;                     \
    data[ channel[__block] ].pulseTicks += _increment;                         \
    data[ channel[__block] ].deltaTicks -= _increment;                         \
    period[__block] += _increment;                                             \
    data[ channel[__block] ].updateDisabled = true;                            \
  }                                                                            \
                                                                               \
  __timer_reg += data[ channel[__block] ].pulseTicks;                          \
                                                                               \
  if(++channel[__block] > __block_upp) {                                       \
    channel[__block] = __block_low;                                            \
    _pin_to_pulse = __reset_pin;                                               \
  }                                                                            \
  else {                                                                       \
    _pin_to_pulse = __pulse_pin;                                               \
  }                                                                            \
}

/**
 * See raw_interrupt.
 *
 * @param none.
 * @return none.
 **/
inline void SerialServo::OCR1A_ISR() {
  /*static uint32_t _time = micros();
  uint32_t _time2 = micros();
  if(channel[SERIAL_SERVO_BANKA] == 0) {
    Serial.println(usToTicks(_time2-_time));
    Serial.println(period[SERIAL_SERVO_BANKA]);
    _time = _time2;
  }*/
  raw_interrupt(SERIAL_SERVO_BANKA,
                SERIAL_SERVO_BANKA_LOW,
                SERIAL_SERVO_BANKA_UP,
                OCR1A,
                PORTB,
                PORTB_PIN(BANKA_PULSE_PIN),
                PORTB_PIN(BANKA_RST_PIN));
}
 
/**
 * See raw_interrupt.
 *
 * @param none.
 * @return none.
 **/
 
inline void SerialServo::OCR1B_ISR() {
  raw_interrupt(SERIAL_SERVO_BANKB,
                SERIAL_SERVO_BANKB_LOW,
                SERIAL_SERVO_BANKB_UP,
                OCR1B,
                PORTB,
                PORTB_PIN(BANKB_PULSE_PIN),
                PORTB_PIN(BANKB_RST_PIN));
}

// Timer1 Output Compare A interrupt service routine.
ISR(TIMER1_COMPA_vect) {
  SerialServo::OCR1A_ISR();
}

// Timer1 Output Compare B interrupt service routine.
ISR(TIMER1_COMPB_vect) {
  SerialServo::OCR1B_ISR();
}