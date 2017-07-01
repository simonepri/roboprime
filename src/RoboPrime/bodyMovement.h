/**
 * Part of RoboPrime Firmware.
 *
 * BodyMovement.h
 * Robot kinetics.
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
 * @require       SerialServo
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */

/*
 * PURPOSE:
 *
 * This create an abstraction class for the robot movments.
 * This also implement a circular buffer for movements.
 * NOTE: that the buffer size need to be a power of two, as we use the AND
 * bitwise operator instead of the slow module operation to compute the next
 * index for the circular buffer.
 */

#ifndef _BODY_MOVEMENT_H
#define _BODY_MOVEMENT_H

#define PART_ANKLE_X_ROT        0
#define PART_ANKLE_Y_ROT        1
#define PART_KNEE_X_ROT         2
#define PART_HIP_Y_ROT          3
#define PART_HIP_X_ROT          4
#define PART_HIP_Z_ROT          5
#define PART_SHOULDER_X_ROT     6
#define PART_SHOULDER_Y_ROT     7
#define PART_ELBOW_Z_ROT        8
#define PART_ELBOW_X_ROT        9
#define PART_SIZE              10


#define HF_R                    0
#define HF_L                    1
#define HF_SIZE                 2
#define HF_NUM          PART_SIZE

#define POS_MIN                 0
#define POS_MED                 1
#define POS_MAX                 2
#define POS_SIZE                3

#define BF_SIZE                 4

#if BF_SIZE && !(BF_SIZE & (BF_SIZE - 1))   // If BF_SIZE is a power of 2
	#define modQueue(n) ((n) & (BF_SIZE - 1))
#else
	#define modQueue(n) ((n) % (BF_SIZE))
#endif

#define nextQueue(n) modQueue(n + 1)
#define countQueue(head,tail) modQueue(head - tail + BF_SIZE)
#define emptyQueue(head,tail) (head == tail)
#define fullQueue(head,tail) (nextQueue(head) == tail)

#define SERVO_ANGLE_POS {                                                      \
	{800,  900,  1200}, {  0,  900,  1800},                                      \
	{  0, 1300,  1800}, {  0,  900,  1800},                                      \
	{800,  900,  1300}, {500,  900,  1800},                                      \
	{  0,  900,  1800}, {  0,  300,  1800},                                      \
	{  0,  900,  1800}, {  0, 1200,  1800}                                       \
}


#define SERVO_ANGLE_OFFSET {                                                   \
	{  0,   0}, {  0,   0}, {  0,   50}, {  -10,   -50}, {  -10,   -10},         \
	{  -120,   10}, {  0,  0}, {  0,   0}, {  0,   0}, {  0,   0}                 \
}

#define INVALID_BODY_POS 65535

typedef struct block_t {
	uint16_t movAngle, movTime;
};

typedef const PROGMEM uint16_t body_pos_t;
typedef const PROGMEM int8_t body_offset_t;

class BodyMovement {
	public:
		static void begin();
		static bool isValidBodypart(uint8_t _idx);
		static void setPos(bool _half, uint8_t _idx, uint16_t _angle);
		static void setSweep(bool _half, uint8_t _idx, uint16_t _angle,
		                     uint16_t _time);
		static void setDefault();
		static void setDefault(bool _half, uint8_t _idx);
		static void setSequence(bool _status);
		static void setWait(bool _half, uint8_t _idx, uint16_t _time);
		static uint16_t getPos(bool _half, uint8_t _idx);
		static uint16_t getMinPos(uint8_t _idx);
		static uint16_t getDefaultPos(uint8_t _idx);
		static uint16_t getMaxPos(uint8_t _idx);
		static bool isMoving(bool _half, uint8_t _idx);

		static bool pushQueue(bool _half, uint8_t _idx, uint16_t _angle,
		                      uint16_t _time);
		static bool popQueue(bool _half, uint8_t _idx);
		static bool isQueueFull(bool _half, uint8_t _idx);
		static bool isQueueEmpty(bool _half, uint8_t _idx);

		static void movementPlanner();
	private:
		// No-one have to create an istance of this class as we use it as
		// static method, so we keep constructor as private.
		BodyMovement();

		static uint16_t raw_validAngle(const bool &_half, const uint8_t &_idx,
		                               const uint16_t &_angle);

		static void raw_setPos(const bool &_half, const uint8_t &_idx,
		                       const uint16_t &_angle);
		static void raw_setSweep(const bool &_half, const uint8_t &_idx,
		                         const uint16_t &_angle, const uint16_t &_time);
		static void raw_setDefault(const bool &_half, const uint8_t &_idx);
		static void raw_setWait(const bool &_half, const uint8_t &_idx,
		                        const uint16_t &_time);
		static uint16_t raw_getPos(const bool &_half, const uint8_t &_idx);
		static uint16_t raw_getMinPos(const uint8_t &_idx);
		static uint16_t raw_getDefaultPos(const uint8_t &_idx);
		static uint16_t raw_getMaxPos(const uint8_t &_idx);
		static bool raw_isMoving(const bool &_half, const uint8_t &_idx);

		static void raw_pushQueue(const bool &_half, const uint8_t &_idx,
		                          const uint16_t &_angle, const uint16_t &_time);
		static void raw_popQueue(const bool &_half, const uint8_t &_idx);
		static bool raw_isQueueFull(const bool &_half, const uint8_t &_idx);
		static bool raw_isQueueEmpty(const bool &_half, const uint8_t &_idx);

		static body_pos_t pos[HF_NUM][POS_SIZE];
		static body_offset_t offset[HF_NUM][HF_SIZE];
		static uint8_t head[HF_SIZE][HF_NUM], tail[HF_SIZE][HF_NUM];
		static block_t queue[HF_SIZE][HF_NUM][BF_SIZE];
};

#endif
