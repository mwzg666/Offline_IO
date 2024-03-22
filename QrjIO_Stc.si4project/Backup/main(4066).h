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

#define SysTick     9216   // 10ms    // 次/秒, 系统滴答频率, 在4000~16000之间

#define Timer0_Reload   (65536UL - SysTick)     //Timer 0 中断频率

#define UART_BUFF_LENGTH   MAX_LENGTH

#define POWER_ON               0xAA
#define POWER_OFF              0x55

#define VERSION  "2.1.4"

//#define QRJCTL

#define CMD_GET_FLOW       0x01     // 获取2个流量和压差
#define CMD_CTL_PAPER      0x02     // 控制走纸
#define CMD_OUT_4_20MA     0x03     // 4_20MA 输出控制
#define CMD_GET_4_20MA     0x04     // 读4_20MA 输入
#define CMD_OUT_4_20MA_2   0x05     // 4_20MA 输出控制2


#define CMD_LED_CTL    'Y'   // 报警灯控制
#define CMD_SOUND      'A'   // 报警音
#define CMD_IO_OUT     'C'   // 开关量输出      
#define CMD_VER        'V'   // 软件版本
#define CMD_IO_IN      'B'   // 输入状态


typedef enum
{
    LED_HOLD = -1,   // 保持不变
    LED_OFF,         // 关闭
    LED_ON,          // 打开
    LED_FLASH        // 闪烁
}LED_STATUS;


typedef enum
{
    STOP_OK = 1,        // 急停
    ALARMCFM_OK         // 报警确认
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
    char Prea;           //  预警
    char Alarm;          //  报警
    char Fault;          //  故障
    char Bump;           //  泵
    #ifdef QRJCTL
    char Alarm1;
    char Alarm2;
    char Alarm3;
    char ChuShuan;       //  除酸
    char ChouQi;         //  抽气
    #endif
}OUT_PARAM;

typedef struct
{
    BYTE OnOff;  // 1: 走纸， 0: 停止
    WORD Pluse;  // 走纸的脉冲数
}PAGER_CTL;

typedef struct
{
    WORD Flow1;  // 流量
    WORD Flow2;  // 流量
    WORD Press;  // 压差
    WORD Temp;   // 温度
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
