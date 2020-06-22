/* 
 * File:   SHT21.h
 * Author: steve.deasy
 *
 * Created on June 18, 2014, 9:55 AM
 */

#ifndef SHT21_H
#define	SHT21_H
//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  SHT2x.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Sensor layer. Definitions of commands and registers,
//              functions for sensor access
//==============================================================================
//---------- Includes ----------------------------------------------------------
//#include "system.h"
//---------- Defines -----------------------------------------------------------
//  CRC
//const u16t POLYNOMIAL = 0x131;  //P(x)=x^8+x^5+x^4+1 = 100110001
#include "../COS/GlobalDef.h"

extern UINT_16 SensirionTemperature_UBP8;
extern UINT_16 SensirionRH_UBP8;
extern UINT_1 SHT21_Fault;

typedef enum
{
    SHT21_MEASURE_TEMP_0, // Select Sensirion or SiLabs
    SHT21_MEASURE_TEMP_1, // trigger humidity sensor temperature measurement
    SHT21_MEASURE_TEMP_2, // poll for measurement complete and read temperature

    SHT21_MEASURE_RH_0, // trigger relative humidity measurement
    SHT21_MEASURE_RH_1, // poll for measurement complete and read relative humidity

    SHT21_CALC_TEMP_0, // calculate temperature in degrees C bin 8
    SHT21_CALC_RH_0, // calculate temperature in relative humidity nim 8
} SHT21_State_tt;

// sensor command
#define TRIG_T_MEASUREMENT_HM       0xE3 // command trig. temp meas. hold master
#define TRIG_RH_MEASUREMENT_HM      0xE5 // command trig. humidity meas. hold master
#define TRIG_T_MEASUREMENT_POLL     0xF3 // command trig. temp meas. no hold master
#define TRIG_RH_MEASUREMENT_POLL    0xF5 // command trig. humidity meas. no hold master
#define USER_REG_W                  0xE6 // command writing user register
#define USER_REG_R                  0xE7 // command reading user register
#define SOFT_RESET                  0xFE  // command soft reset

#define SHT2x_RES_12_14BIT          0x00 // RH=12bit, T=14bit
#define SHT2x_RES_8_12BIT           0x01 // RH= 8bit, T=12bit
#define SHT2x_RES_10_13BIT          0x80 // RH=10bit, T=13bit
#define SHT2x_RES_11_11BIT          0x81 // RH=11bit, T=11bit
#define SHT2x_RES_MASK              0x81  // Mask for res. bits (7,0) in user reg.

#define SHT2x_EOB_ON                0x40 // end of battery
#define SHT2x_EOB_MASK              0x40 // Mask for EOB bit(6) in user reg.

#define SHT2x_HEATER_ON             0x04 // heater on
#define SHT2x_HEATER_OFF            0x00 // heater off
#define SHT2x_HEATER_MASK           0x04 // Mask for Heater bit(2) in user reg.

// measurement signal selection
#define  HUMIDITY   0
#define  TEMP       1

#define SHT21_ADDR_W   128   // sensor I2C address + write bit
#define SHT21_ADDR_R   129    // sensor I2C address + read bit

// function prototypes
extern void SHT21_StateMachine(void);
UINT_8 SHT2x_SoftReset(void);
UINT_16 SHT2x_CalcTemperatureC(void);
UINT_16 SHT2x_CalcRelativeHumidity(void);

// performs a reset, returns error code
UINT_8 SHT2x_SoftReset(void);

#endif	/* SHT21_H */

