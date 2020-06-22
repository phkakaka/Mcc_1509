#include "../COS/GlobalDef.h"
#include "I2C_HAL.h"
#include "SHT21.h"
#include "StateMachine.h"
#include "UART_Display.h"

//Variables with external linkage
UINT_16 SensirionTemperature_UBP8;
UINT_16 SensirionRH_UBP8;
UINT_1 SHT21_Fault;

//Variables used only by this module
UINT_8 checksum;
UINT_8  TemperatureRaw_H;
UINT_8  TemperatureRaw_L;
UINT_16 TemperatureRaw;
UINT_8  RelativeHumidityRaw_H;
UINT_8  RelativeHumidityRaw_L;
UINT_16 RelativeHumidityRaw;
SHT21_State_tt SHT21_State = SHT21_MEASURE_TEMP_0;

void SHT21_FaultHand(void);

void SHT21_StateMachine(void)
{
    switch(SHT21_State)
    {
        case SHT21_MEASURE_TEMP_0: // Select Sensirion or SiLabs
            SHT21_Fault = 0; // clear fault flag
            SHT21_State = SHT21_MEASURE_TEMP_1;
            break;
        case SHT21_MEASURE_TEMP_1: // trigger temperature measurement
            //-- write I2C sensor address and command --
            I2c_StartCondition();
            ErrorStat |= I2c_WriteByte(SHT21_ADDR_W); // I2C Adr
            ErrorStat |= I2c_WriteByte(TRIG_T_MEASUREMENT_POLL);
            if (ErrorStat == 0)
            {
                SHT21_State = SHT21_MEASURE_TEMP_2;
                I2C_Timer = 0;
            }
            else
            {
                SHT21_FaultHand();
            }
            break;
        case SHT21_MEASURE_TEMP_2: // poll for temperature measurement complete
            I2c_StartCondition();
            I2C_Timer += I2C_TASK_TIME;
            if (I2C_Timer > I2C_MEASUREMENT_TIME_OUT)
            {
                SHT21_FaultHand();
            }
            else if (I2c_WriteByte(SHT21_ADDR_R) != 0)
            {
                // do nothing, measurement not complete yet
            }
            else
            {
                //-- read two data bytes and one checksum byte --
                TemperatureRaw_H = I2c_ReadByte(ACK);
                TemperatureRaw_L = I2c_ReadByte(ACK);
                TemperatureRaw = (UINT_16) TemperatureRaw_H;
                TemperatureRaw <<= 8;
                TemperatureRaw |= (UINT_16) TemperatureRaw_L;
                checksum = I2c_ReadByte(NO_ACK);
                I2c_StopCondition();
                SHT21_State = SHT21_MEASURE_RH_0;
            }
            break;
        case SHT21_MEASURE_RH_0: // trigger relative humidity measurement
            //-- write I2C sensor address and command --
            I2c_StartCondition();
            ErrorStat |= I2c_WriteByte(SHT21_ADDR_W); // I2C Adr
            ErrorStat |= I2c_WriteByte(TRIG_RH_MEASUREMENT_POLL);
            if (ErrorStat == 0)
            {
                I2C_Timer = 0;
                SHT21_State = SHT21_MEASURE_RH_1;
            }
            else
            {
                SHT21_FaultHand();
            }
            break;
        case SHT21_MEASURE_RH_1: // poll for relative humidity measurement complete
            I2c_StartCondition();
            I2C_Timer += I2C_TASK_TIME;
            if (I2C_Timer > I2C_MEASUREMENT_TIME_OUT)
            {
                SHT21_FaultHand();
            }
            else if (I2c_WriteByte(SHT21_ADDR_R) != 0)
            {
                // do nothing, measurement not complete yet
            }
            else
            {
                //-- read two data bytes and one checksum byte --
                RelativeHumidityRaw_H = I2c_ReadByte(ACK);
                RelativeHumidityRaw_L = I2c_ReadByte(ACK);
                RelativeHumidityRaw = (UINT_16) RelativeHumidityRaw_H;
                RelativeHumidityRaw <<= 8;
                RelativeHumidityRaw |= (UINT_16) RelativeHumidityRaw_L;
                checksum = I2c_ReadByte(NO_ACK);
                I2c_StopCondition();
                SHT21_State = SHT21_CALC_TEMP_0;
            }
            break;
        case SHT21_CALC_TEMP_0:
            SensirionTemperature_UBP8 = SHT2x_CalcTemperatureC();
            SHT21_State = SHT21_CALC_RH_0;
            break;
        case SHT21_CALC_RH_0:
            SHT21_State = SHT21_MEASURE_TEMP_0;
            SensirionRH_UBP8 = SHT2x_CalcRelativeHumidity();
            break;
    }
    SendByUart(SensirionTemperature_UBP8);
}

void SHT21_FaultHand(void)
{
    TemperatureRaw = 0;
    RelativeHumidityRaw = 0;
    SHT21_Fault = 1;
    SHT21_State = SHT21_CALC_TEMP_0;
}


UINT_8 SHT2x_SoftReset(void)
{
    UINT_8  error;           //error variable
    error=0;
    I2c_StopCondition();

    I2c_StartCondition();
    SSP1BUF = SHT21_ADDR_W;
    while((SSP1STAT&0x04)!=0);
    SSP1STAT = 0;
    error |= SSP1CON2&0x40;
    SSP1BUF = SOFT_RESET;
    while((SSP1STAT&0x04)!=0);
    SSP1STAT = 0;
    error |= SSP1CON2&0x40;

    I2c_StopCondition();

    //  DelayMicroSeconds(15000); // wait till sensor has restarted
    // note may want to remove this and handle the 15 msec delay external
    return error;
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
