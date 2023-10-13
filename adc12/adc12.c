#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "system.h"
#include "adc12.h"


#if 0

#define SAMPLE_NUM 20 //采样平均次数
#define REMOVE_CNT 200

unsigned int bat_result[SAMPLE_NUM];
unsigned int hv_result; //[SAMPLE_NUM];
WORD index=0;



#define BAT_AD_VPP 3000   //参考电源为系统电源，系统电源变化需要修改该值,AD的参考电压只是作为AD本身使用，不涉及计算
#define BAT_VOL_RATION 6  //分压比

#define HV_AD_VPP 3300//参考电源为系统电源，系统电源变化需要修改该值,AD的参考电压只是作为AD本身使用，不涉及计算
#define HV_VOL_RATION 1651//分压比,根据电阻计算出来

extern char StrTemp[24];


void InitAdc(void)
{
    //ADC12设置**************************
    P6DIR &= BIT0;
    P6SEL |= BIT0; //使用A/D通道 A0
    
    ADC12CTL0 = ADC12ON + MSC + SHT0_8 + REFON + REF2_5V;// 开启ad,参考电压2.5v,内部参考电压打开
    ADC12CTL1 = SHP + ADC12SSEL_3;                  //Use sampling timer, SMCLK ;
    
    ADC12MCTL0 |= INCH_0 + SREF_1; //R+=2.5V R-=VSSADC12IE = 1<<(n-1); 
	
	ADC12IE = 0x01; 
    //转换允许
    //ADC12CTL0 |= ENC ; //转换允许(上升沿)
}


void ADC12_Start()
{
    ADC12CTL0 |= ENC;
    ADC12CTL0 |= ADC12SC;

    index = 0;
}

float GetVlotValu()
{
    //#define DEBUG_BAT//测试
    #define K_BAT 0.00531
    #define B_BAT -0.78836
    
    unsigned char i;
    float value,result;
    
    float temp = 0;

    for(i=0;i<SAMPLE_NUM;i++)
    {
        temp += bat_result[i];   //Get Result AD valude
    }

    #if 0
    value=(unsigned int)(temp/SAMPLE_NUM);
    temp = (float)value*BAT_AD_VPP*BAT_VOL_RATION;
    result=(WORD)(temp/4095);
    #else
    //result = (unsigned int)(temp * 50 / 242);
    value = temp/SAMPLE_NUM;

    // debug result = (value - 234)/202;
    //result = (value - 785)/125;  // ---- my
    //result =  (value - 82)/218; 
    //------------ result =  (value - 40)/223;    // 老版本分压电阻
    result =  K_BAT*value + B_BAT;    // 老版本分压电阻
    //result =  (value - 16.4)/205; 

	//result = ((float)gtestADValue-40)/207.7;//yao测试++++
    #endif

    #ifdef DEBUG_BAT
    memset(StrTemp,0,24);
    sprintf(StrTemp,"%.0f:%.2fV",value,result);
    Clear(0,10,70,8);
    Show5X7String(0,10,StrTemp,0);
    #endif

    return result;
}
#endif

#if 0
void Test_ADC12()
{
  delay_ms(10000);
  volatile unsigned int bat_vol=0,tmp1=0;
  volatile unsigned long hv_vol=0,tmp2=0;
  //Init_ADC12();
  delay_ms(1000);
  bat_vol = Get_Bat_Vol();
  hv_vol = Get_HV_Vol();
  tmp1 = bat_vol;
  tmp2 = hv_vol;
  _NOP();
}
#endif

#if 0

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    if (index < REMOVE_CNT)
    {
        // 去掉前面n次转换数据
        hv_result = ADC12MEM0;                 // Move results, IFG is cleared
        index++;
    }
    else 
    {
        bat_result[index-REMOVE_CNT] = ADC12MEM0;                 // Move results, IFG is cleared

        index++;
        if(index >= SAMPLE_NUM+REMOVE_CNT)
        {
            index = 0;
            ADC12CTL0 &= ~ENC;  // 停止ADC
        }
    }

}
#endif
