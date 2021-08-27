#include "w_adc.h"

void W_ADCConfig(void)
{
	
	ADCCFG = 0x2f;      //设置ADC时钟为系统时钟/2/16/16
	ADC_CONTR = 0x80;   //使能ADC模块
}
uint16_t GetADCResult(uint8_t ch)
{		
//	if(ch==12||ch==13||ch==14)YE_power=1;	Delay1ms();
	ADC_CONTR = ADC_POWER| ch | ADC_START;
	_nop_();                        //
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR & ADC_FLAG));//
	ADC_CONTR &= ~ADC_FLAG;         //Close ADC
	return (ADC_RES*256+ADC_RESL);//;//(((ADC_RES*256+ADC_RESL)*10)*5)/1024;                 //·μ??ADC?á1?
}
