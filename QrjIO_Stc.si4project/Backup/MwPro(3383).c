#include "MwPro.h"


BYTE CheckSum(BYTE *Data,BYTE len)
{
    BYTE dwSum = 0;
	BYTE i;
    for (i = 0; i < len; i++)
    {
        dwSum += Data[i];
    }

    return dwSum;
}

BYTE BcdToHex(BYTE Bh, BYTE Bl)
{
    BYTE temp;
    if (Bh > 0x39)
    {
        temp = Bh - 0x37;
    }
    else
    {
        temp = Bh - 0x30;
    }

    temp <<= 4;
    temp &=0xF0;
    
    if (Bl > 0x39)
    {
        temp |= (Bl - 0x37)&0x0F;
    }
    else
    {
        temp |= (Bl - 0x30)&0x0F;
    }

    return temp;
}


void SendCmd(BYTE Cmd, BYTE *Data, BYTE Length)
{	
	BYTE Sum;
    BYTE Str[3] = {0};
	FRAME_HEAD *head;

    memset(SendBuf, 0, UART_BUFF_LENGTH);
    head = (FRAME_HEAD*)SendBuf;

    head->Head   = HEAD;
    head->Lenght = 8+Length;
    head->Type   = 0x10;    // 暂时未使用
    head->Addr   = 0xA0;    
    head->Cmd    = Cmd;

    // data
    if (Length > 0)
    {
        memcpy(&SendBuf[sizeof(FRAME_HEAD)], Data, Length);
    }

    // CS
    Sum = CheckSum(&SendBuf[1],(BYTE)(Length+4));
    sprintf((char *)Str, "%02X", Sum);
    memcpy(&SendBuf[Length+5], Str, 2);

    SendBuf[Length+7] = TAIL;

    while(CommIdleTime < 200)
    {
        TimerTask();
        HndInput();
    }

    Uart1Send(SendBuf,head->Lenght);

    CommIdleTime = 0;
}



BOOL ValidUartFrame()
{
	BYTE cs;
    BYTE rs;
    FRAME_HEAD *pFrameHead;
	if ((RecLength < (sizeof(FRAME_HEAD) + 3)) ||
      (RecLength >= 255))   // 长度大于最大包长 或 小于 帧头长度
    {
        return FALSE;
    }

    pFrameHead = (FRAME_HEAD *)RecvBuf;
    if (pFrameHead->Head != HEAD)
    {
        return FALSE;
    }

    if (RecvBuf[RecLength-1] != TAIL)
    {
        return FALSE;
    }

    if (pFrameHead->Lenght != RecLength) // 协议数据长度 大于 实际长度
    {
        return FALSE;
    }

    cs = CheckSum(&RecvBuf[1], (BYTE)(RecLength-4));
    rs = BcdToHex(RecvBuf[RecLength-3], RecvBuf[RecLength-2]);
    
    if (cs != rs)
    {
        return FALSE;
    }

    return TRUE;
}

