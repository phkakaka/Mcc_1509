#include "I2C_Driver.h"
#include <xc.h>

void I2C_Init(void)
{
    SSP1STAT = 0x00;
    SSP1CON1 = 0x08;
    SSP1CON2 = 0x00;
    SSP1ADD  = 0x27;
    SSP1CON1bits.SSPEN = 1;  //Enable MSSP
    SSP1IF = 0;
    PIE1bits.SSP1IE = 1;
}

void I2C_StartCondition(void)
{
    SSP1IF = 0;
    SSP1CON2bits.SEN = 1;    // initiate start condition
    while(!SSP1IF);
    SSP1IF = 0;
}

void I2C_StopCondition(void)
{
    SSP1IF = 0;
    SSP1CON2bits.PEN = 1;    // initiate stop condition
    while(!SSP1IF);
    SSP1IF = 0;
}

void I2C_MasterSendAck(void)
{
    SSP1CON2bits.ACKDT = 0;
    SSP1CON2bits.ACKEN = 1;
}

void I2C_MasterSendNack(void)
{
    SSP1CON2bits.ACKDT = 1;
    SSP1CON2bits.ACKEN = 1;
}

UINT_8 I2C_WriteByte (UINT_8 txByte)
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

UINT_8 I2C_ReadByte (UINT_8 ack)
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