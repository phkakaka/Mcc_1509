#include "I2C_HAL.h"
#include "SHT21.h"
#include "StateMachine.h"

UINT_8 I2C_State;
UINT_16 I2C_Timer;
UINT_8 ErrorStat;

SINT_16 TI_TemperatureAmbient;

void Tsk_I2C_StateMachine(void)
{
    ErrorStat = 0;
    switch (I2C_State)
    {
        case I2C_START_0: // Initialize I2C interface
            I2c_Init();
            I2C_Timer = 0;
            I2C_State = I2C_START_1;
            break;
        case I2C_START_1: // Humidity sensors need 15 ms power up delay
            I2C_Timer += I2C_TASK_TIME;
            if (I2C_Timer > I2C_SOFT_RESET_DELAY)
            {
                I2C_State = I2C_SHT21;
            }
            break;
        case I2C_SHT21:
            SHT21_StateMachine();
            I2C_State = I2C_SHT21;
            break;
        default: /* should not happen, default to normal */
            break;
    }

}