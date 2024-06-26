#include "main.h"
#include <intrins.h>

#ifdef DEV_FIXED
#define STATUS_MASK     0x3FFC
#define MODE_MASK       0x00FC
#endif

#ifdef DEV_OFFLINE_LOW
#define STATUS_MASK 0x1FF8      // 能远程控制的IO值1
#define MODE_MASK    0x01F8     // 能闪烁的IO值1
#endif

WORD  OutStatus = 0;   // 32bit- 0: 关闭  1：打开
WORD  OutMode    = 0;  // 32bit-0: 常亮，1：闪烁
WORD  RunTime    = 0;
WORD  Task1s   = 0;
BOOL  InputReport = FALSE;
BYTE  InputStatus = 0;
BOOL  NeedGetFlow  = TRUE;
BOOL  PaperErr = FALSE;

BYTE g_Key_Confrom = 0;

BYTE Input_Status = 0;

PAGER_CTL xdata Paper;
WORD  PaperPluse = 0;
WORD  PaperTimer = 0;
WORD  CommIdleTime = 0;
BASE_INFO xdata g_BaseInfo;

static WORD OutHis =  0;

BYTE  RecvBuf[UART_BUFF_LENGTH] = {0};
BYTE  SendBuf[UART_BUFF_LENGTH] = {0};
BYTE RecLength = 0;

u16  Timer0Cnt = 0;

WORD xdata FlashIoTimer[32] = {0};    


/*========================================================================
// 函数名称: WORD WordToSmall(WORD dat)
// 函数功能: 将WORD的数据转换为小端模式
// 入口参数: @WORD dat：要转换的数据
// 函数返回: 返回类型为WORD的小端模式数据
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
========================================================================*/
WORD WordToSmall(WORD dat)
{
    BYTE buf[2];
    BYTE t;
    WORD ret;
    
    memcpy(buf, &dat, 2);
    t = buf[1];
    buf[1] = buf[0];
    buf[0] = t;
    
    memcpy(&ret, buf, 2);
    return ret;
}

float FloatToSmall(float dat)
{
    BYTE buf[4];
    BYTE t;
    float ret;
    
    memcpy(buf, &dat, 4);
    t = buf[3];
    buf[3] = buf[0];
    buf[0] = t;
    t = buf[2];
    buf[2] = buf[1];
    buf[1] = t;

    memcpy(&ret, buf, 4);
    return ret;
}

DWORD DwordToSmall(DWORD dat)
{
    BYTE buf[4];
    BYTE t;
    DWORD ret;
    
    memcpy(buf, &dat, 4);
    t = buf[3];
    buf[3] = buf[0];
    buf[0] = t;
    t = buf[2];
    buf[2] = buf[1];
    buf[1] = t;

    memcpy(&ret, buf, 4);
    return ret;
}

void Error()
{
    while(1)
    {
        RUN_LED(1);
        Delay(50);
        RUN_LED(0);
        Delay(50);
    }
}

//void Delay(WORD ms)
//{
//    WORD t = 1000;
//    while(ms--)
//    {
//        for (t=0;t<1000;t++) ;
//    }
//}

void Delay(WORD ms)    //@11.0592MHz
{
    unsigned long edata i;

    while(ms--)
    {
        _nop_();
        _nop_();
        i = 2763UL;
        while (i) 
        {
            i--;
        }
    }
}

void SysInit()
{
    HIRCCR = 0x80;           // 启动内部高速IRC
    while(!(HIRCCR & 1));
    CLKSEL = 0;              
}

void Timer0Init()
{
    AUXR = 0x00;    //Timer0 set as 12T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    
    // 中断优先级3
//    PT0  = 1;
//    PT0H = 1;
}

// 10ms 中断一下
void Timer0Int (void) interrupt 1
{
    Timer0Cnt ++;
}


void Bump_ONOFF(BYTE x)
{
    VALVE(x);
    BUMP(x);
}

void Key_Scan(void)
{
    static BYTE keyVal = 0;
    if(STOP_M() == 0)
    {
        Delay(15);
        if(STOP_M() == 0)
        {
            OutCtl(0, BIT_BUMP);
            //keyVal = STOP_OK;
        }
    }
//    else
//    {
//        keyVal = 0;
//    }
//    return keyVal;
}

void Output(BYTE i, BYTE x)
{
    switch(i)
    {
        
        case BIT_HOST_POWER:    HOST_POWER(x);    break;    // 主电源控制
        case BIT_SYS_POWER:     SYS_POWER(x);     break;    // 工控机电源控制
        //case BIT_SEN_ONOFF:     SEN_POWER(x);     break;    // 探测器电源  
        case BIT_LED_RED:       LED_RED(x);       break;    // 指示灯(红)
        case BIT_LED_YELLOW:    LED_YELLOW(x);    break;    // 指示灯(黄)
        case BIT_LED_GREEN:     LED_GREEN(x);     break;    // 指示灯(绿)
        case BIT_LIGHT_RED:     LIGHT_RED(x);     break;    // 报警灯（红）
        case BIT_LIGHT_YELLOW:  LIGHT_YELLOW(x); break;     // 报警灯（黄）
        case BIT_ALARM_SOUND:   ALARM_SOUND(x);   break;    // 报警声控制
        case BIT_BUMP:          Bump_ONOFF(x);    break;    // 泵
        case BIT_PREA:          PREA(x);          break;    // 预警
        case BIT_ALARM:         ALARM(x);         break;    // 报警
        #ifdef DEV_FIXED
        case BIT_CHU_SHUAN:     CHU_SHUAN(x);     break;    // 除酸
        case BIT_CHOU_QI:       CHOU_QI(x);       break;    // 抽气
        case BIT_ZHOU_ZHI:      ZHOU_ZHI(x);      break;    // 走纸
        #endif
        case BIT_FAULT:         FAULT(x);         break;    // 故障
    }
}


// 闪烁 -- 300ms 开 300ms 关
void OutFlash(BYTE i)
{
    //static BYTE FlashHis = 0;
    WORD mask = 0;
    
    if (FlashIoTimer[i]++ > 20)   // 320ms
    {
        FlashIoTimer[i] = 0;
        
        mask = 1<<i;
        if ((OutHis & mask) == 0)
        {
            OutHis |= mask;
            Output(i, 1);
        }
        else
        {
            OutHis &= ~mask;
            Output(i, 0);
        }
    }
}

void OutVal(BYTE i, BYTE st)
{
    WORD mask = 1 << i;
    WORD his = (WORD)((OutHis >> i) & 1);
    if (his == st)
    {
        return;
    }

    if (st)
    {
        OutHis |= mask;
    }
    else
    {
        OutHis &= ~mask;
    }
    
    Output(i, st);
}

// Io输出控制 -- 16ms 运行一次
void OutTask()
{
    BYTE i, st;
    WORD mask;
    for (i=0;i<16;i++)
    {
        mask = 1 << i;
        if ((mask & STATUS_MASK) == 0)
        {
            // 有些IO不能在这里控制
            continue;
        }
        
        if ( (OutMode & (1 << i) ) != 0)    // 闪烁模式
        {
            if ((OutStatus & (1 << i)) != 0)
            {
                // 开始闪烁
                OutFlash(i);
            }
            else
            {
                // 停止闪烁
                OutVal(i, 0);
            }
        }
        else
        {
            st = (OutStatus & (1 << i))?1:0;
            OutVal(i, st);
        }
    }
}


void Task_1s()
{
    #if 0
    static BYTE on = 1;
    OutCtl(on,   BIT_ALARM_1);
    on = !on;
    #endif
   
    //Read4_20ma();
    //GetFlow();

    int Voltage = 0;
    CLR_WDT = 1;  // 喂狗

    // 需要的是否才采集
    if (NeedGetFlow)
    {
        NeedGetFlow = FALSE;
        //GetAds1110(I2C_GASFLOW1); 
        g_BaseInfo.Flow1 = Voltage;

        //GetAds1110(I2C_GASFLOW2); 
        g_BaseInfo.Flow2 = Voltage;

        //GetAds1110(I2C_PROGRESS); 
        g_BaseInfo.Press = Voltage;
        g_BaseInfo.Temp = Voltage;
    }
}


void RunLed(WORD dt)
{
    static WORD tm = 0;
    tm += dt;
    if (tm > 2500)
    {
        tm = 0;
        RUN_LED(0);
    }
    else if (tm > 2300)
    {
        RUN_LED(1);
    }
        
}


// 定时任务
void TimerTask(void)
{
    WORD Delta = 0;

    if (Timer0Cnt)
    {
        Delta = Timer0Cnt * 10;
        Timer0Cnt = 0;

        if (RX1_Cnt > 0)
        {
            Rx1_Timer += Delta;
        }

        if (RunTime < 5000)
        {
            RunTime += Delta;
        }
        #ifdef DEV_FIXED
        if (Paper.OnOff)
        {
            PaperTimer += Delta;
        }
        #endif
        Task1s += Delta;
        if (Task1s >= 1000)
        {
            Task1s = 0;
            CLR_WDT = 1;  // 喂狗
            if (InputReport == FALSE)
            {
                Task_1s();
            }
        }

        if (CommIdleTime < 500)
        {
            CommIdleTime += Delta;
        } 
        
        OutTask();
        RunLed(Delta);
    }
}

void OutCtl(BYTE st, BYTE i)
{
    switch(st)
    {
        case 0: OutStatus &= ~(1<<i); OutMode &= ~(1<<i);  break;
        case 1: OutStatus |= (1<<i);   OutMode &= ~(1<<i);   break;
        case 2: OutStatus |= (1<<i);   OutMode |= (1<<i);    break;

        // 报警声特殊操作 -- 沿用原来的协议
        case 0xAA: OutStatus |= (1<<i);   OutMode |= (1<<i);    break;   // 开启
        case 0x55: OutStatus &= ~(1<<i); OutMode &= ~(1<<i);  break;  // 关闭
    }
}


void LampCtl()
{
    LED_PARAM led;
    memcpy(&led, (BYTE *)&RecvBuf[sizeof(FRAME_HEAD)], sizeof(LED_PARAM));

    OutCtl(led.StateLed_Green,   BIT_LED_GREEN);
    OutCtl(led.StateLed_Red,     BIT_LED_RED);
    OutCtl(led.StateLed_Yellow, BIT_LED_YELLOW);
    OutCtl(led.AlarmLed_RED ,    BIT_LIGHT_RED);
    OutCtl(led.AlarmLed_Yellow, BIT_LIGHT_YELLOW);
     
    OutStatus &= STATUS_MASK;
    OutMode &= MODE_MASK;
}

void SndCtl()
{
    BYTE Snd = RecvBuf[sizeof(FRAME_HEAD)];
    OutCtl(Snd, BIT_ALARM_SOUND);
     
    OutStatus &= STATUS_MASK;
    OutMode &= MODE_MASK;
}

void IoCtl()
{
    OUT_PARAM out;
    memcpy(&out, (BYTE *)&RecvBuf[sizeof(FRAME_HEAD)], sizeof(OUT_PARAM));
    #ifdef DEV_OFFLINE_LOW
    OutCtl(out.Prea,      BIT_PREA);
    OutCtl(out.Alarm,     BIT_ALARM);
    OutCtl(out.Fault,     BIT_FAULT);
    OutCtl(out.Bump,      BIT_BUMP);
    #endif
    
    #ifdef DEV_FIXED
    OutCtl(out.Alarm1,     BIT_ALARM_1);
    OutCtl(out.Alarm2,     BIT_ALARM_2);
    OutCtl(out.Alarm3,     BIT_ALARM_3);
    OutCtl(out.ChuShuan,   BIT_CHU_SHUAN);
    OutCtl(out.ChouQi ,    BIT_CHOU_QI);
    #endif
    
    OutStatus &= STATUS_MASK;
    OutMode &= MODE_MASK;
}

void LedInit()
{
   // 初始状态都为0 
   //运行灯
   RUN_LED(0);          // 运行灯
   // 指示灯
   LED_RED(0);          // 指示灯（红）
   LED_YELLOW(0) ;      // 指示灯（黄）
   LED_GREEN(0);        // 指示灯（绿）
   //报警灯
   LIGHT_RED(0) ;       // 报警灯（红）
   LIGHT_YELLOW(0);     // 报警灯（黄）
   ALARM_SOUND(0);      // 报警音 
   //报警
   PREA(0);
   ALARM(0);
   FAULT(0);
   Bump_ONOFF(0);


}

void GetFlow()
{
    BASE_INFO baseinfo;
    NeedGetFlow = TRUE;
    baseinfo.Flow1 = WordToSmall(g_BaseInfo.Flow1);
    baseinfo.Flow2 = WordToSmall(g_BaseInfo.Flow2);
    baseinfo.Press = WordToSmall(g_BaseInfo.Press);
    baseinfo.Temp  = WordToSmall(g_BaseInfo.Temp);
    
    SendCmd(CMD_GET_FLOW,(BYTE *)&baseinfo,sizeof(BASE_INFO));
}

#ifdef DEV_FIXED
void CtlPaper()
{
    memcpy(&Paper, (BYTE *)&RecvBuf[sizeof(FRAME_HEAD)], sizeof(PAGER_CTL));
    if (Paper.OnOff)
    {
        OutCtl(1,    BIT_ZHOU_ZHI);
        PaperTimer = 0;
        PaperPluse = 0;
    }
    else
    {
        OutCtl(0,    BIT_ZHOU_ZHI);
    }
}
#endif

void Out4_20ma(BYTE val)
{
    WORD v = (WORD)((float)val *100);
    //v = WordToSmall(v);
    MCP4725_OutVol(MCP4725_aV_ADDR, v);
}


void Out4_20ma_2(BYTE val)
{
    WORD v = (WORD)((float)val * 100);
    //v = WordToSmall(v);
    MCP4725_OutVol2(MCP4725_HV_ADDR, v);
}



BYTE Read4_20ma()
{
    BYTE ret = 0;
    int Voltage = 0;
    GetAds1110(I2C_4_20MA_IN);
    
    //GetAds1110(2);

    ret = (BYTE)((float)Voltage/60);
    SendCmd(CMD_GET_4_20MA, &ret, 1);
    return ret;
}


void GetVer()
{
    BYTE ver[8] = {0};
    memcpy(ver, VERSION, strlen(VERSION));
    SendCmd(CMD_VER, ver, 8);
}


void HndUartFrame()
{
    FRAME_HEAD *pFrameHead = (FRAME_HEAD *)RecvBuf; 
    switch(pFrameHead->Cmd)
    {
        case CMD_LED_CTL:  LampCtl();  break;    // 报警灯控制
        case CMD_SOUND:    SndCtl();   break;    // 报警音
        case CMD_IO_OUT:   IoCtl();    break;          
        case CMD_VER:      GetVer();   break;    // 软件版本

        case CMD_GET_FLOW:    GetFlow();    break;
        #ifdef DEV_FIXED
        case CMD_CTL_PAPER:   CtlPaper();   break;
        #endif
        case CMD_OUT_4_20MA:  Out4_20ma(RecvBuf[sizeof(FRAME_HEAD)]);    break;
        case CMD_GET_4_20MA:  Read4_20ma();  break;
        case CMD_OUT_4_20MA_2:Out4_20ma_2(RecvBuf[sizeof(FRAME_HEAD)]);    break;
    }
}

void PowerOff()
{
    RUN_LED(0);
    //HOST_POWER(0);
    //SYS_POWER(0);
    //SEN_POWER(0);
    IoInit();
    while(1)
    {
        ;
    }
}

void PowerHnd(BYTE InVal)
{
    IN_DEF in;
    in.Val = InVal;

    if (RunTime >= 5000)  // 关机
    {
        if (in.IoBit.HostPwSt == 0)  // 工控已经关机
        {
            //Error(); 
            OutVal(BIT_SYS_POWER, OFF);   // 关闭工控机
            OutVal(BIT_HOST_POWER, OFF);   // 关闭总电源
            //OutVal(BIT_SEN_ONOFF, OFF);   // IO板总电源
           

            PowerOff();
            //ALARM_1(0);
        }
    }
}

//BYTE GetInput()
//{
//    // 当前只有一个开关机状态 P5.0
//    static BYTE his = LOCK_BIT();
//    BYTE st = ONOFF_LOCK();
//
//    if (st != his)
//    {
//        Delay(50);
//        if ( st == ONOFF_LOCK() )
//        {
//            his = st;
//            return st;
//        }
//    }
//    return 0xFF;
//}

BYTE GetInput()
{
    // 当前只有一个开关机状态 P5.0
  
    BYTE ret;
    
    ret = P1 & 0x0B;
    ret <<= 4;
    ret |= (P4 & 0x02);
    ret |= (P5 & 0x01);

    return ret;
}



//void HndInput()
//{
//    static BYTE  PwrHis = POWER_ON;   
//    static BYTE  StHis = 0x00;
//    BYTE key = 0;
//    BYTE s;
//
//    key = Key_Scan();
//    switch(key)
//    {
//        case STOP_OK:  
//        {
//            //OutVal(BIT_BUMP,OFF);  
//            OutCtl(0, BIT_BUMP);
//            break;
//        }
//        case ALARMCFM_OK:  
//        {
//            g_Key_Confrom = 1; 
//            //OutVal(BIT_ALARM_SOUND,OFF);  
//            OutCtl(0, BIT_ALARM_SOUND);
//            break;
//        }
//    }
//    
//    Input_Status = GetInput();
//    if (Input_Status != 0xFF)
//    {
//        //DebugMsg("Power Lock status \r\n");
//        s = (Input_Status ^ StHis);
//        if (s & LOCK_BIT())   // 开关机锁
//        {
//            if (Input_Status & LOCK_BIT())
//            {
//                // 高电平关机
//                PwrHis = POWER_OFF;
//                //DebugMsg("Power Off \r\n");
//                
//                InputReport = 1;
//            }
//            else
//            {
//                // 低电平开机
//                //DebugMsg("Power On \r\n");
//                //PW_MAIN(1);
//                PwrHis = POWER_ON;
//                RunTime = 0;
//            }
//        }
//        StHis = Input_Status;
//    }
//
//
//    // 延时关机
//    if (PwrHis == POWER_OFF)
//    {
//        if ((PC_STAUTUS() == 0) && (RunTime >= 5000))
//        {
//            RunTime = 0;
//            //DebugMsg("Power Down \r\n");
//            PowerOff();
//        }
//    }
//}


void HndInput()
{
    #define IO_MASK 0x33  

    static BYTE InHis = 0;
    static BYTE RpHis = 0;
    BYTE InCur;
    
    Key_Scan();
    
    InCur= GetInput();
    PowerHnd(InCur);
    if (InHis != InCur)
    {
        Delay(20);
        InCur = GetInput();
        if (InCur != InHis)
        {
            InHis = InCur;

            InCur &= IO_MASK;
            if (RpHis != InCur)
            {
                // 状态变了才上报
                RpHis = InCur;
                InputReport = TRUE;
                InputStatus = InCur;
                //SendCmd(CMD_IO_IN, (BYTE *)&InCur, 1);
                //Sleep(10);
            }
        }
    }
}

/*
void HndPaper()
{
    static BYTE IrHis = 0; 
    BYTE ir;
    //BYTE ret = 1;
    
    if (Paper.OnOff)
    {
        ir = PAPER_IR();
        if (IrHis != ir)
        {
            IrHis = ir;
            PaperPluse ++;
            PaperTimer = 0;
        }
        
        if ((PaperPluse >= Paper.Pluse) || (PaperTimer > 500))
        {
            OutCtl(0,    BIT_ZHOU_ZHI);
            Paper.OnOff = 0;
        }

        if (PaperTimer > 500)
        {   
            // 走纸失败
            PaperErr = TRUE;
            //SendCmd(CMD_CTL_PAPER, (BYTE *)&ret, 1);
        }
    }
}
*/

//void ReportInput()
//{
//    BYTE ret = 1;
//    BYTE PwOff = POWER_OFF;
//    
//    // 通信空闲的时候才上报，不然会冲突
//    if (CommIdleTime > 200)
//    {
//        if (InputReport)
//        {
//            InputReport = FALSE;
//            SendCmd(CMD_IO_IN, (BYTE *)&PwOff, 1);
//            return;
//        }
//
//        if (g_Key_Confrom)
//        {
//            g_Key_Confrom = 0;
//            SendCmd(CMD_SOUND, NULL, 0);
//            return;
//        }
//    }
//}

void ReportInput()
{
    BYTE ret = 1;
    
    // 通信空闲的时候才上报，不然会冲突
    if (CommIdleTime > 200)
    {
        
        if (InputReport)
        {
            //Error();
            InputReport = FALSE;
            SendCmd(CMD_IO_IN, (BYTE *)&InputStatus, 1);
            return;
        }

//        if (g_Key_Confrom)
//        {
//            g_Key_Confrom = 0;
//            SendCmd(CMD_SOUND, NULL, 0);
//            return;
//        }
    }
}



void ClearRevBuf()
{
    memset(RecvBuf, 0, UART_BUFF_LENGTH);
    RecLength = 0;
}

void HndUartData()
{
    if (ValidUartFrame())
    {
        HndUartFrame();
    }
    ClearRevBuf();
}

int main( void )
{
    SysInit();
    
    IoInit();
    OutVal(BIT_HOST_POWER, ON);      //主电源
    
    LedInit(); 
    RUN_LED(1);
    
    //OutVal(BIT_SEN_ONOFF, ON);      // 上电打开IO板总电源
    OutVal(BIT_SYS_POWER, ON);      // 上电打开工控机

    Delay(200);
    
    Timer0Init();
    UART1_config();
    ClearUart1Buf();

    Delay(200);
    Out4_20ma(0);
    Out4_20ma_2(0);
    OutCtl(1, BIT_LED_GREEN);   // 上电开启运行灯
    
    RUN_LED(0);
    
    EA = 1;

    WDT_CONTR |= (1<<5) |  7;  // 启动开门狗，约8秒
    while(1)
    {        
        TimerTask();
        HndInput();
        Uart1Hnd();
        ReportInput();
        
    }  
}















