#ifndef __MWPRO_H__
#define __MWPRO_H__

#include "main.h"
#include "uart.h"

#define HEAD   0x02
#define TAIL   0x03


#pragma pack(1)

// 协议头结构
typedef struct
{
    BYTE Head;       // 1 0x02 帧头
    BYTE Lenght;     // 2 帧长 整个帧的长度
    BYTE Type;       // 3 类型
    BYTE Addr;       // 4 地址
    BYTE Cmd;        // 5 命令
}FRAME_HEAD;
#pragma pack()

BYTE CheckSum(BYTE *Data,BYTE len);

void SendCmd(BYTE Cmd, BYTE *Data, BYTE Length);
BOOL ValidUartFrame();

#endif
