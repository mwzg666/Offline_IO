#ifndef _ADS1110_H
#define _ADS1110_H


#define ADS110_ADDR       0x49

#if 1
#define GetAds1110(id)\
{\
    BYTE rd_data=0; \
    char ack = 0; \
    unsigned char OutBuf[3]={0};\
    char out_data = (ADS110_ADDR<<1)| 1; \
    I2C_Start(id);\
    I2C_Write_Byte(id);\
    I2C_ReadAck(id);\
    ack = I2C_ACK; \
    rd_data=0;\
    I2C_Read_Byte(id); \
    OutBuf[0] = rd_data;\
    ack = I2C_ACK; \
    rd_data=0;\
    I2C_Read_Byte(id); \
    OutBuf[1] = rd_data;\
    ack = I2C_NACK; \
    rd_data=0;\
    I2C_Read_Byte(id); \
    OutBuf[2] = rd_data;\
    I2C_Stop(id); \
    Voltage = (OutBuf[0] << 8) | OutBuf[1];\
    Voltage = (int)(1.0*Voltage/16);\
}
#else
int GetAds1110(BYTE id);

#endif

#endif

