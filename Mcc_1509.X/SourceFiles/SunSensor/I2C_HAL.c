#include "../COS/GlobalDef.h"
#include "I2C_HAL.h"

void DelayMicroSeconds (UINT_16 nbrOfUs)
{
    UINT_16 i;
    for(i=0; i<nbrOfUs; i++)
    {
        __delay_us(1);
    }
}

void I2c_Init ()
{
    SSP1STAT = 0x00;
    SSP1CON1 = 0x28;
    SSP1CON2 = 0x00;
    SSP1ADD  = 0x4F;
    SSP1CON1bits.SSPEN = 0;
}

void I2c_StartCondition (void)
{
    SSP1IF = 0;
    SSP1CON2 = 0x01;    // initiate start condition
    while(!SSP1IF);
    SSP1IF = 0;
}

void I2c_StopCondition ()
{
    SSP1IF = 0;
    SSP1CON2 = 0x04;    // initiate stop condition
    while(!SSP1IF);
    SSP1IF = 0;
}

UINT_8 I2c_WriteByte (UINT_8 txByte)
{
    UINT_8 ack;
    ack = 0;
    SSP1IF = 0;
    SSP1BUF = txByte;
    while(!SSP1IF);
    SSP1IF = 0;
    SSP1STAT = 0;
    ack |= SSP1CON2&0x40;
    return ack; //return ACK (0) or NACK(1)
}

UINT_8 I2c_ReadByte (UINT_8 ack)
{
    UINT_8 rxByte;
    rxByte=0;

    SSP1CON2bits.RCEN = 1;        // initiate read sequence
    while(!SSP1IF);               // wait for read complete
    SSP1IF = 0;                   // clear interrupt flag
    rxByte = SSP1BUF;             // read received byte
    if(ack == 0)SSP1CON2bits.ACKDT = 0;     // set up to ACK or NACK according to ack parameter
    else SSP1CON2bits.ACKDT = 1;     // set up to ACK or NACK according to ack parameter
    SSP1IF = 0;
    SSP1CON2bits.ACKEN = 1;           // enable ack
    while(!SSP1IF);
    SSP1IF = 0;
    return rxByte;                      //return read byte
}

