/** @file Led.c
 *  @ingroup SunSensor
 *
 * @author Samuel Wang
 *
 * 
 */

/*-- Includes --*/
#include "../../mcc_generated_files/mcc.h"
#include "SHT25.h"
#include "UART_Display.h"
#include "I2C_Driver.h"
#include "../../mcc_generated_files/examples/i2c_master_example.h"

//Variables with external linkage
UINT_16 SensirionTemperature_UBP8;
UINT_16 SensirionRH_UBP8;
UINT_1 SHT25_Fault;
UINT_8 ErrorStat;
UINT_16 I2C_Timer;

//Variables used only by this module
UINT_8 checksum;
UINT_8  TemperatureRaw_H;
UINT_8  TemperatureRaw_L;
UINT_16 TemperatureRaw;
UINT_8  RelativeHumidityRaw_H;
UINT_8  RelativeHumidityRaw_L;
UINT_16 RelativeHumidityRaw;
SHT25_State_tt SHT25_State = SHT25_MEASURE_INIT;
UINT_8 Command = TRIG_T_MEASUREMENT_POLL;

void SHT25_StateMachine(void)
{
    switch(SHT25_State)
    {
        case SHT25_MEASURE_INIT:
            I2C_Init();
            SHT25_Fault = 0; // clear fault flag
            SHT25_State = SHT25_MEASURE_TEMP_0;
            break;
        case SHT25_MEASURE_TEMP_0:
            I2C_StartCondition();
            ErrorStat |= I2C_WriteByte(SHT25_ADDR_W);
            ErrorStat |= I2C_WriteByte(TRIG_T_MEASUREMENT_POLL);
            if (ErrorStat == 0)
            {
                SHT25_State = SHT25_MEASURE_TEMP_1;
                I2C_Timer = 0;
            }
            else
            {
                SHT25_FaultHand();
            }
            break;
        case SHT25_MEASURE_TEMP_1:
            I2C_StartCondition();
            I2C_Timer += I2C_TASK_TIME;
            if (I2C_Timer > I2C_MEASUREMENT_TIME_OUT)
            {
                SHT25_FaultHand();
            }
            else if (I2C_WriteByte(SHT25_ADDR_R) != 0)
            {
                // do nothing, measurement not complete yet
            }
            else
            {
                //-- read two data bytes and one checksum byte --
                TemperatureRaw_H = I2C_ReadByte(ACK);
                TemperatureRaw_L = I2C_ReadByte(ACK);
                TemperatureRaw = (UINT_16) TemperatureRaw_H;
                TemperatureRaw <<= 8;
                TemperatureRaw |= (UINT_16) TemperatureRaw_L;
                checksum = I2C_ReadByte(NO_ACK);
                I2C_StopCondition();
                SHT25_State = SHT25_MEASURE_RH_0;
            }
            break;
        case SHT25_MEASURE_RH_0:
            //-- write I2C sensor address and command --
            I2C_StartCondition();
            ErrorStat |= I2C_WriteByte(SHT25_ADDR_W); // I2C Adr
            ErrorStat |= I2C_WriteByte(TRIG_RH_MEASUREMENT_POLL);
            if (ErrorStat == 0)
            {
                I2C_Timer = 0;
                SHT25_State = SHT25_MEASURE_RH_1;
            }
            else
            {
                SHT25_FaultHand();
            }
            break;
        case SHT25_MEASURE_RH_1:
            I2C_StartCondition();
            I2C_Timer += I2C_TASK_TIME;
            if (I2C_Timer > I2C_MEASUREMENT_TIME_OUT)
            {
                SHT25_FaultHand();
            }
            else if (I2C_WriteByte(SHT25_ADDR_R) != 0)
            {
                // do nothing, measurement not complete yet
            }
            else
            {
                //-- read two data bytes and one checksum byte --
                RelativeHumidityRaw_H = I2C_ReadByte(ACK);
                RelativeHumidityRaw_L = I2C_ReadByte(ACK);
                RelativeHumidityRaw = (UINT_16) RelativeHumidityRaw_H;
                RelativeHumidityRaw <<= 8;
                RelativeHumidityRaw |= (UINT_16) RelativeHumidityRaw_L;
                checksum = I2C_ReadByte(NO_ACK);
                I2C_StopCondition();
                SHT25_State = SHT25_CALC_TEMP_0;
            }
            break;
        case SHT25_CALC_TEMP_0:
            SensirionTemperature_UBP8 = SHT2x_CalcTemperatureC();
            SHT25_State = SHT25_CALC_RH_0;
            break;
        case SHT25_CALC_RH_0:
            SHT25_State = SHT25_MEASURE_TEMP_0;
            SensirionRH_UBP8 = SHT2x_CalcRelativeHumidity();
            break;
        default:
            break;
    }
}

void SHT25_FaultHand(void)
{
    TemperatureRaw = 0;
    RelativeHumidityRaw = 0;
    SHT25_Fault = 1;
    SHT25_State = SHT25_CALC_TEMP_0;
}


UINT_16 SHT2x_CalcTemperatureC(void)
{
    SINT_32 TemperatureTemp;
    UINT_16 Temperature_UBP8;
    //-- calculate temperature [°C] --
    //  temperatureC= -46.85 + 175.72/65536 *(ft)u16sT; //T= -46.85 + 175.72 * ST/2^16

    TemperatureTemp = (SINT_32)TemperatureRaw * (SINT_32)(175.72 * BIN8CONV);
    TemperatureTemp -= (SINT_32)(6.85 * BIN16CONV * BIN8CONV);
    if(TemperatureTemp < 0)TemperatureTemp = 0;
    TemperatureTemp >>= 8;
    TemperatureTemp >>= 8;
    Temperature_UBP8 = (UINT_16)TemperatureTemp;
    return(Temperature_UBP8);
}

UINT_16 SHT2x_CalcRelativeHumidity(void)
{
    SINT_32 RelativeHumidityTemp;
    UINT_16 RelativeHumidity_UBP8;
    //-- calculate temperature [°C] --
    //  temperatureC= -46.85 + 175.72/65536 *(ft)u16sT; //T= -46.85 + 175.72 * ST/2^16

    RelativeHumidityTemp = (SINT_32)RelativeHumidityRaw * (SINT_32)(125 * BIN8CONV);
    RelativeHumidityTemp -= (SINT_32)(6 * BIN16CONV * BIN8CONV);
    if(RelativeHumidityTemp < 0)RelativeHumidityTemp = 0;
    RelativeHumidityTemp >>= 8;
    RelativeHumidityTemp >>= 8;
    RelativeHumidity_UBP8 = (UINT_16)RelativeHumidityTemp;
    if(RelativeHumidity_UBP8 >(UINT_16)(100.0 * BIN8CONV))
    {
        RelativeHumidity_UBP8 = (UINT_16)(100.0 * BIN8CONV);
    }
    return(RelativeHumidity_UBP8);
}


