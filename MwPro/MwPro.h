#ifndef __MWPRO_H__
#define __MWPRO_H__

#include "main.h"
#include "uart.h"

#define HEAD   0x02
#define TAIL   0x03


#pragma pack(1)

// Э��ͷ�ṹ
typedef struct
{
    BYTE Head;       // 1 0x02 ֡ͷ
    BYTE Lenght;     // 2 ֡�� ����֡�ĳ���
    BYTE Type;       // 3 ����
    BYTE Addr;       // 4 ��ַ
    BYTE Cmd;        // 5 ����
}FRAME_HEAD;
#pragma pack()

BYTE CheckSum(BYTE *Data,BYTE len);

void SendCmd(BYTE Cmd, BYTE *Data, BYTE Length);
BOOL ValidUartFrame();

#endif
