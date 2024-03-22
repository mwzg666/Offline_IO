#ifndef IOCTROL_H
#define IOCTROL_H

#include "main.h"

#if 0
#define IODireOut(io_name,io_mask)\
         do\
         {\
          io_name##DIR|=(io_mask);\
         }while(0)
         
#define IOWrite(io_name,io_mask,data_mask)\
        io_name##OUT=(io_name##OUT & ~(io_mask)) | ((data_mask) & (io_mask))


#define IODireIn(io_name,io_mask)\
        do\
        {\
         io_name##DIR&=~(io_mask);\
        }while(0)  
        
#define IORead(io_name) io_name##IN
#endif

#define  ON  1
#define  OFF 0  


// 在代码中只单独操作了这几个位， 其他就不用定义了
#define BIT_HOST_POWER   0    // 主电源
#define BIT_SYS_POWER    1    // 工控机
#define BIT_SEN_ONOFF    2    // IO板总电源


#define BIT_LED_RED       3
#define BIT_LED_YELLOW    4
#define BIT_LED_GREEN     5
#define BIT_LIGHT_RED     6
#define BIT_LIGHT_YELLOW   7
#define BIT_ALARM_SOUND    8
#define BIT_BUMP         9   


#define BIT_PREA        10
#define BIT_ALARM       11
#define BIT_FAULT       12


#ifdef QRJCTL
#define BIT_ALARM_1     13
#define BIT_ALARM_2     14
#define BIT_CHU_SHUAN   15
#define BIT_CHOU_QI     16
#define BIT_ZHOU_ZHI    17
#define BIT_ALARM_3     18
#endif 


#define RUN_LED(x)       (x)?(P0 |= (1<<7)):(P0 &= ~(1<<7))     // 板载LED

#define HOST_POWER(x)    (x)?(P0 |= (1<<6)):(P0 &= ~(1<<6))     // 主电源
#define SYS_POWER(x)     (x)?(P0 |= (1<<3)):(P0 &= ~(1<<3))     // 工控机电源控制
#define SEN_POWER(x)     (x)?(P0 |= (1<<4)):(P0 &= ~(1<<4))     // 探测器电源控制

#define LED_RED(x)       (x)?(P2 |= (1<<0)):(P2 &= ~(1<<0))     // 指示灯(红)
#define LED_YELLOW(x)    (x)?(P4 |= (1<<4)):(P4 &= ~(1<<4))    // 指示灯(黄)
#define LED_GREEN(x)     (x)?(P4 |= (1<<3)):(P4 &= ~(1<<3))     // 指示灯(绿)
#define LIGHT_RED(x)     (x)?(P1 |= (1<<4)):(P1 &= ~(1<<4))     // 报警灯（红）
#define LIGHT_YELLOW(x)  (x)?(P1 |= (1<<5)):(P1 &= ~(1<<5))     // 报警灯（黄）
#define ALARM_SOUND(x)   (x)?(P4 |= (1<<2)):(P4 &= ~(1<<2))   // 报警声控制

#define VALVE(x)   (x)?(P0 |=(1<<1)):(P0 &= ~(1<<1))       //电磁阀控制

#define PREA(x)      (x)?(P5 |= (1<<2)):(P5  &= ~(1<<2))   // 一级报警
#define ALARM(x)     (x)?(P5 |= (1<<3)):(P5  &= ~(1<<3))   // 二级报警
#define FAULT(x)     (x)?(P0 |= (1<<5)):(P0  &= ~(1<<5))   // 故障

#define BUMP(x)      (x)?(P0 |= (1<<2)):(P0 &= ~(1<<2))      // 泵

#define STOP_M()   (P0 & (1<<0))                           //急停控制

#define PC_STAUTUS()  (P1 & (1<<3))       // 工控机5V监测
#define ALARM_CFM()   (P4 & (1<<1))      // 报警确认
#define ONOFF_LOCK()  (P5 & (1<<0))      // 开关锁状态

#define LOCK_BIT()  (1<<0)

#ifdef QRJCTL

#define RS485_EN(x)      (x)?(P4OUT  |= BIT5):(P4OUT  &= ~BIT5) 

#define ALARM_1(x)      (x)?(P7OUT  |= BIT4):(P7OUT  &= ~BIT4)   // 一级报警
#define ALARM_2(x)      (x)?(P7OUT  |= BIT5):(P7OUT  &= ~BIT5)   // 二级报警
#define ALARM_3(x)      (x)?(P7OUT  |= BIT6):(P7OUT  &= ~BIT6)   // 过载报警


#define CHU_SHUAN(x)    (x)?(P9OUT  |= BIT2):(P9OUT  &= ~BIT2)  // 除酸
#define CHOU_QI(x)      (x)?(P0 |= (1<<2)):(P0 &= ~(1<<2))      // 抽气
#define ZHOU_ZHI(x)     (x)?(P9OUT  |= BIT6):(P9OUT  &= ~BIT6)  // 走纸
#define HOST_PWST()     (P2IN & BIT3)                           // 主机电源指示
#define PAPER_IR()      (P9IN & BIT7)
#endif

typedef union
{
    BYTE Val;
    struct
    {
        BYTE HostPw:1;
        BYTE SysPw:1;
        BYTE Led_r:1;
        BYTE Led_y:1;
        BYTE Led_g:1;
        BYTE Light_r:1;
        BYTE Light_y:1;
        BYTE Sound:1;  
    };
}OUT_DEF;


typedef union
{
    BYTE Val;
    struct
    {
        BYTE OnOffLock:1;
        BYTE AlamCfm:1;
        BYTE Res1:1;
        BYTE Res2:1;
        BYTE Input1:1;
        BYTE Input2:1;
        BYTE Res3:1;
        BYTE HostPwSt:1; 
    };
}IN_DEF;

void IoInit();




#endif
