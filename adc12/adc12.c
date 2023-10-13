#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "system.h"
#include "adc12.h"


#if 0

#define SAMPLE_NUM 20 //����ƽ������
#define REMOVE_CNT 200

unsigned int bat_result[SAMPLE_NUM];
unsigned int hv_result; //[SAMPLE_NUM];
WORD index=0;



#define BAT_AD_VPP 3000   //�ο���ԴΪϵͳ��Դ��ϵͳ��Դ�仯��Ҫ�޸ĸ�ֵ,AD�Ĳο���ѹֻ����ΪAD����ʹ�ã����漰����
#define BAT_VOL_RATION 6  //��ѹ��

#define HV_AD_VPP 3300//�ο���ԴΪϵͳ��Դ��ϵͳ��Դ�仯��Ҫ�޸ĸ�ֵ,AD�Ĳο���ѹֻ����ΪAD����ʹ�ã����漰����
#define HV_VOL_RATION 1651//��ѹ��,���ݵ���������

extern char StrTemp[24];


void InitAdc(void)
{
    //ADC12����**************************
    P6DIR &= BIT0;
    P6SEL |= BIT0; //ʹ��A/Dͨ�� A0
    
    ADC12CTL0 = ADC12ON + MSC + SHT0_8 + REFON + REF2_5V;// ����ad,�ο���ѹ2.5v,�ڲ��ο���ѹ��
    ADC12CTL1 = SHP + ADC12SSEL_3;                  //Use sampling timer, SMCLK ;
    
    ADC12MCTL0 |= INCH_0 + SREF_1; //R+=2.5V R-=VSSADC12IE = 1<<(n-1); 
	
	ADC12IE = 0x01; 
    //ת������
    //ADC12CTL0 |= ENC ; //ת������(������)
}


void ADC12_Start()
{
    ADC12CTL0 |= ENC;
    ADC12CTL0 |= ADC12SC;

    index = 0;
}

float GetVlotValu()
{
    //#define DEBUG_BAT//����
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
    //------------ result =  (value - 40)/223;    // �ϰ汾��ѹ����
    result =  K_BAT*value + B_BAT;    // �ϰ汾��ѹ����
    //result =  (value - 16.4)/205; 

	//result = ((float)gtestADValue-40)/207.7;//yao����++++
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
        // ȥ��ǰ��n��ת������
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
            ADC12CTL0 &= ~ENC;  // ֹͣADC
        }
    }

}
#endif
