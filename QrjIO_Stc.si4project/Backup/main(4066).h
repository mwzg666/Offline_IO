#ifndef _MYDEFINE_H
#define _MYDEFINE_H

#include <stdio.h>
#include <string.h>

#include "system.h"

#include "uart.h"
#include "io.h"
#include "i2c.h"
#include "MwPro.h"
#include "Mcp4725.h"
#include "ads1110.h"
#include "Adc12.h"



#define MAIN_Fosc        11059200UL    // 11.0592M
//#define MAIN_Fosc        35000000UL    // 35M

#define SysTick     9216   // 10ms    // ��/��, ϵͳ�δ�Ƶ��, ��4000~16000֮��

#define Timer0_Reload   (65536UL - SysTick)     //Timer 0 �ж�Ƶ��

#define UART_BUFF_LENGTH   MAX_LENGTH

#define POWER_ON               0xAA
#define POWER_OFF              0x55

#define VERSION  "2.1.4"

//#define QRJCTL

#define CMD_GET_FLOW       0x01     // ��ȡ2��������ѹ��
#define CMD_CTL_PAPER      0x02     // ������ֽ
#define CMD_OUT_4_20MA     0x03     // 4_20MA �������
#define CMD_GET_4_20MA     0x04     // ��4_20MA ����
#define CMD_OUT_4_20MA_2   0x05     // 4_20MA �������2


#define CMD_LED_CTL    'Y'   // �����ƿ���
#define CMD_SOUND      'A'   // ������
#define CMD_IO_OUT     'C'   // ���������      
#define CMD_VER        'V'   // ����汾
#define CMD_IO_IN      'B'   // ����״̬


typedef enum
{
    LED_HOLD = -1,   // ���ֲ���
    LED_OFF,         // �ر�
    LED_ON,          // ��
    LED_FLASH        // ��˸
}LED_STATUS;


typedef enum
{
    STOP_OK = 1,        // ��ͣ
    ALARMCFM_OK         // ����ȷ��
}KEY_STATUS;


#pragma pack(1)
typedef struct
{
    char StateLed_Green;
    char StateLed_Yellow;
    char StateLed_Red;
    char AlarmLed_RED;
    char AlarmLed_Yellow;
}LED_PARAM;


typedef struct
{
    char Prea;           //  Ԥ��
    char Alarm;          //  ����
    char Fault;          //  ����
    char Bump;           //  ��
    #ifdef QRJCTL
    char Alarm1;
    char Alarm2;
    char Alarm3;
    char ChuShuan;       //  ����
    char ChouQi;         //  ����
    #endif
}OUT_PARAM;

typedef struct
{
    BYTE OnOff;  // 1: ��ֽ�� 0: ֹͣ
    WORD Pluse;  // ��ֽ��������
}PAGER_CTL;

typedef struct
{
    WORD Flow1;  // ����
    WORD Flow2;  // ����
    WORD Press;  // ѹ��
    WORD Temp;   // �¶�
}BASE_INFO;

#pragma pack()

extern BYTE xdata RecvBuf[UART_BUFF_LENGTH];
extern BYTE  SendBuf[UART_BUFF_LENGTH];
extern BYTE RecLength;
extern WORD  CommIdleTime;




void Delay(WORD ms);
void GetFlow();
BYTE Read4_20ma();
void Out4_20ma_2(BYTE val);

void HndUartData();
void OutCtl(BYTE st, BYTE i);
void HndInput();
void TimerTask(void);
void OutVal(BYTE i, BYTE st);



#ifdef __cplusplus
}
#endif
#endif
