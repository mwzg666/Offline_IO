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


// �ڴ�����ֻ�����������⼸��λ�� �����Ͳ��ö�����
#define BIT_HOST_POWER   0    // ����Դ
#define BIT_SYS_POWER    1    // ���ػ�
#define BIT_SEN_ONOFF    2    // IO���ܵ�Դ


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


#define RUN_LED(x)       (x)?(P0 |= (1<<7)):(P0 &= ~(1<<7))     // ����LED

#define HOST_POWER(x)    (x)?(P0 |= (1<<6)):(P0 &= ~(1<<6))     // ����Դ
#define SYS_POWER(x)     (x)?(P0 |= (1<<3)):(P0 &= ~(1<<3))     // ���ػ���Դ����
#define SEN_POWER(x)     (x)?(P0 |= (1<<4)):(P0 &= ~(1<<4))     // ̽������Դ����

#define LED_RED(x)       (x)?(P2 |= (1<<0)):(P2 &= ~(1<<0))     // ָʾ��(��)
#define LED_YELLOW(x)    (x)?(P4 |= (1<<4)):(P4 &= ~(1<<4))    // ָʾ��(��)
#define LED_GREEN(x)     (x)?(P4 |= (1<<3)):(P4 &= ~(1<<3))     // ָʾ��(��)
#define LIGHT_RED(x)     (x)?(P1 |= (1<<4)):(P1 &= ~(1<<4))     // �����ƣ��죩
#define LIGHT_YELLOW(x)  (x)?(P1 |= (1<<5)):(P1 &= ~(1<<5))     // �����ƣ��ƣ�
#define ALARM_SOUND(x)   (x)?(P4 |= (1<<2)):(P4 &= ~(1<<2))   // ����������

#define VALVE(x)   (x)?(P0 |=(1<<1)):(P0 &= ~(1<<1))       //��ŷ�����

#define PREA(x)      (x)?(P5 |= (1<<2)):(P5  &= ~(1<<2))   // һ������
#define ALARM(x)     (x)?(P5 |= (1<<3)):(P5  &= ~(1<<3))   // ��������
#define FAULT(x)     (x)?(P0 |= (1<<5)):(P0  &= ~(1<<5))   // ����

#define BUMP(x)      (x)?(P0 |= (1<<2)):(P0 &= ~(1<<2))      // ��

#define STOP_M()   (P0 & (1<<0))                           //��ͣ����

#define PC_STAUTUS()  (P1 & (1<<3))       // ���ػ�5V���
#define ALARM_CFM()   (P4 & (1<<1))      // ����ȷ��
#define ONOFF_LOCK()  (P5 & (1<<0))      // ������״̬

#define LOCK_BIT()  (1<<0)

#ifdef QRJCTL

#define RS485_EN(x)      (x)?(P4OUT  |= BIT5):(P4OUT  &= ~BIT5) 

#define ALARM_1(x)      (x)?(P7OUT  |= BIT4):(P7OUT  &= ~BIT4)   // һ������
#define ALARM_2(x)      (x)?(P7OUT  |= BIT5):(P7OUT  &= ~BIT5)   // ��������
#define ALARM_3(x)      (x)?(P7OUT  |= BIT6):(P7OUT  &= ~BIT6)   // ���ر���


#define CHU_SHUAN(x)    (x)?(P9OUT  |= BIT2):(P9OUT  &= ~BIT2)  // ����
#define CHOU_QI(x)      (x)?(P0 |= (1<<2)):(P0 &= ~(1<<2))      // ����
#define ZHOU_ZHI(x)     (x)?(P9OUT  |= BIT6):(P9OUT  &= ~BIT6)  // ��ֽ
#define HOST_PWST()     (P2IN & BIT3)                           // ������Դָʾ
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
