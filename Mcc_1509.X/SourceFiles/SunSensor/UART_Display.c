/** @file Led.c
 *  @ingroup SunSensor
 *
 * @author Samuel Wang
 *
 * 
 */

/*-- Includes --*/
#include "../../mcc_generated_files/mcc.h"
#include "UART_Display.h"

static uint8_t U8Message = 0x55;
static uint8_t NewMessageFlag = 0;

void SendByUart(uint8_t Txt)
{
    U8Message = Txt;
    NewMessageFlag = 1;
}

void UartDisplay(void)
{
    if (NewMessageFlag)
    {
        EUSART_Write(U8Message);
        NewMessageFlag = 0;
    }
}


