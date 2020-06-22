/* 
 * File:   I2C_HAL.h
 * Author: steve.deasy
 *
 * Created on June 18, 2014, 9:43 AM
 */

#ifndef I2C_HAL_H
#define	I2C_HAL_H
#include "../COS/GlobalDef.h"

#define HIGH 1
#define LOW 0
#define ACK_ERROR       1
#define TIME_OUT_ERROR  2
#define CHECKSUM_ERROR  4
#define ACK 0
#define NO_ACK 1

// Function prototypes
void I2c_Init (void);

void I2c_StartCondition (void);
// writes a start condition on I2C-bus
// input : -
// output: -
// return: -
// note  : timing (delay) may have to be changed for different microcontroller
//       _____
// SDA:       |_____
//       _______
// SCL :        |___

void I2c_StopCondition (void);
// writes a stop condition on I2C-bus
// input : -
// output: -
// return: -
// note  : timing (delay) may have to be changed for different microcontroller
//              _____
// SDA:   _____|
//            _______
// SCL :  ___|

UINT_8 I2c_WriteByte (UINT_8 txByte);
// writes a byte to I2C-bus and checks acknowledge
// input:  txByte  transmit byte
// output: -
// return: error
// note: timing (delay) may have to be changed for different microcontroller

UINT_8 I2c_ReadByte (UINT_8 ack);
// reads a byte on I2C-bus
// input:  rxByte  receive byte
// output: rxByte
// note: timing (delay) may have to be changed for different microcontroller

#endif	/* I2C_HAL_H */

