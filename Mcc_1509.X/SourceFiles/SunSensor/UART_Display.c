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
static uint8_t counter;

void SendByUart(uint8_t Txt)
{
    U8Message = Txt;
}

void UartDisplay(void)
{
//    if (EUSART_is_tx_ready())
//    {
//        EUSART_Write(U8Message);
//    }
    counter++;
    TXREG = counter;
    if (counter > 200)
    {
        counter = 0;
    }
}


