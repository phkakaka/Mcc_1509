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
#include "../../mcc_generated_files/examples/i2c_master_example.h"

uint16_t Temp;
uint8_t TempH;
uint16_t Humi;
uint8_t Command;


void GetTemp(void)
{
    Command = 0xF5;
    //Temp = I2C_Read2ByteRegister(128,0xE5);
    I2C_WriteNBytes(128,&Command,1);
    I2C_ReadNBytes(129,&TempH,1);
    
    
    SendByUart(TempH+0x30);

}

//
////===========================================================================
//UINT_8 SHT2x_MeasureHM(etSHT2xMeasureType eSHT2xMeasureType, nt16 *pMeasurand)
////===========================================================================
//{
//  UINT_8  checksum;   //checksum
//  UINT_8  data[2];    //data array for checksum verification
//  UINT_8  error=0;    //error variable
//  UINT_16 i;          //counting variable
//
//  //-- write I2C sensor address and command --
//  I2c_StartCondition();
//  error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
//  switch(eSHT2xMeasureType)
//  { case HUMIDITY: error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_HM); break;
//    case TEMP    : error |= I2c_WriteByte (TRIG_T_MEASUREMENT_HM);  break;
//    default: assert(0);
//  }
//  //-- wait until hold master is released --
//  I2c_StartCondition();
//  error |= I2c_WriteByte (I2C_ADR_R);
//  SCL=HIGH;                     // set SCL I/O port as input
//  for(i=0; i<1000; i++)         // wait until master hold is released or
//  { DelayMicroSeconds(1000);    // a timeout (~1s) is reached
//    if (SCL_CONF==1) break;
//  }
//  //-- check for timeout --
//  if(SCL_CONF==0) error |= TIME_OUT_ERROR;
//
//  //-- read two data bytes and one checksum byte --
//  pMeasurand->s16.u8H = data[0] = I2c_ReadByte(ACK);
//  pMeasurand->s16.u8L = data[1] = I2c_ReadByte(ACK);
//  checksum=I2c_ReadByte(NO_ACK);
//
//  //-- verify checksum --
//  error |= SHT2x_CheckCrc (data,2,checksum);
//  I2c_StopCondition();
//  return error;
//}
