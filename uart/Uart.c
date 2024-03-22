#include "main.h"
#include "uart.h"
#include "io.h"

u16 Rx1_Timer  = 0;

u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt = 0;    //接收计数
bit B_TX1_Busy; //发送忙标志

u8 xdata RX1_Buffer[MAX_LENGTH] = {0}; //接收缓冲

void UART1_config()
{
    /*********** 波特率使用定时器1 *****************/
    {
        TR1 = 0;
        AUXR &= ~0x01;      //S1 BRT Use Timer1;
        AUXR |=  (1<<6);    //Timer1 set as 1T mode
        TMOD &= ~(1<<6);    //Timer1 set As Timer
        TMOD &= ~0x30;      //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256);
        ET1 = 0;    //禁止中断
        INTCLKO &= ~0x02;  //不输出时钟
        TR1  = 1;
    }

    /*************************************************/
    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 
    //           0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率 

    SCON = (SCON & 0x3f) | 0x40; 

//    PS  = 0;    //中断高优先级
//    PSH = 0;
    //PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    
    //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 
    //                 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    P_SW1 &= 0x3f;
    P_SW1 |= 0x40;  
    
    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}

void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;
        Rx1_Timer = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        
        if(++RX1_Cnt >= MAX_LENGTH)   
        {
            RX1_Cnt = 0;
        }
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}

void Uart1Send(u8 *buf, int len)
{
    int i;
    for (i=0;i<len;i++)     
    {
        SBUF = buf[i];
        B_TX1_Busy = 1;
        while(B_TX1_Busy);
    }
}

void Uart1Hnd()
{
    if (Rx1_Timer > 20)
    {
        Rx1_Timer = 0;
        
        memcpy(RecvBuf,RX1_Buffer, RX1_Cnt);
        RecLength = RX1_Cnt;
        HndUartData(); 
        ClearUart1Buf();
    }
}


//重定向Printf
char putchar(char c )
{
    SBUF = c;
    while(!TI);
    TI = 0;
    return c;
}

void ClearUart1Buf()
{
    memset(RX1_Buffer,0,MAX_LENGTH);
    RX1_Cnt = 0;
}




