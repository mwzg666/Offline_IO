#include "system.h"
#include "main.h"
#include "mcp4725.h"
#include "i2c.h"
#include "io.h"


void MCP4725_OutVol(BYTE addr,unsigned int voltage)
{
    long uiVolTemp;
    BYTE out_data = 0;
    if(voltage > MCP4725_REF_VOL)
    {
        //printf("input voltage > ref voltage\r\n");
        return;
    }

    //_DINT();
    uiVolTemp = voltage*4096l/MCP4725_REF_VOL;
    I2C_Start(1);
    out_data = addr<<1;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    out_data = ((uiVolTemp>>8)&0xf)|MCP4725_PD_MODE;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    out_data = uiVolTemp & 0xff;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    I2C_Stop(1);
    //_EINT();
}


void MCP4725_OutVol2(BYTE addr,unsigned int voltage)
{
    long uiVolTemp;
    BYTE out_data = 0;
    if(voltage > MCP4725_REF_VOL)
    {
        //printf("input voltage > ref voltage\r\n");
        return;
    }

    //_DINT();
    uiVolTemp = voltage*4096l/MCP4725_REF_VOL;
    I2C_Start(1);
    out_data = addr<<1;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    out_data = ((uiVolTemp>>8)&0xf)|MCP4725_PD_MODE;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    out_data = uiVolTemp & 0xff;
    I2C_Write_Byte(1);
    I2C_ReadAck(1);
    I2C_Stop(1);
    //_EINT();
}


#if 0
unsigned int MCP4725_Read(unsigned char addr)
{
    unsigned int uiTemp1,uiTemp2,data;

    I2C_Start();
    I2C_Write_Byte((addr<<1) | 1);
    I2C_Read_Byte(I2C_ACK);
    uiTemp1 = I2C_Read_Byte(I2C_ACK);
    uiTemp2 = I2C_Read_Byte(I2C_NACK);
    I2C_Stop();
    data = ((uiTemp1&0xf)<<4 |(uiTemp2>>4) ) | (uiTemp1<<4);    
    return data;
}
#endif

