#ifndef UART_H
#define UART_H

#define Baudrate   115200UL
#define Baudrate3   9600UL

#define MAX_LENGTH   200

extern u8  TX1_Cnt;    //发送计数
extern u8  RX1_Cnt;    //接收计数
extern bit B_TX1_Busy; //发送忙标志
extern u16 Rx1_Timer;

extern u8 xdata RX1_Buffer[]; //接收缓冲


/* UART config */
void UART1_config();

void IrDAConfig(long int InputFreq,long int BaudRate);

void Uart1Send(u8 *buf, int len);

void Uart1Hnd();

void ClearUart1Buf();



#ifdef __cplusplus
}
#endif
#endif
