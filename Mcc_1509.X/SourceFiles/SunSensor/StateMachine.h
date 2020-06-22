/* 
 * File:   SHT21.h
 * Author: steve.deasy
 *
 * Created on June 18, 2014, 9:55 AM
 */

#ifndef STATE_MACHINE_H
#define	STATE_MACHINE_H

typedef enum
{
    I2C_START_0, // Initialize I2C interface
    I2C_START_1, // Humidity sensor needs 15 ms power up delay

    I2C_SHT21, // Run Sensirion SHT21 state machine
    I2C_TI_TMP, // Run TMP006 state machine
    I2C_SHT31, //Run Sensirion SHT31 state machine
} I2C_State_tt;

#define I2C_TASK_TIME			TIME_TSK_EVERYSLOT

#define I2C_SOFT_RESET_DELAY        15
#define I2C_MEASUREMENT_TIME_OUT    200


extern UINT_16 I2C_Timer;
extern UINT_8 ErrorStat;

extern void Tsk_I2C_StateMachine(void);
#endif	/* STATE_MACHINE_H */

