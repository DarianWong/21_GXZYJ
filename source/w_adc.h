#ifndef	_W_ADC_H
#define	_W_ADC_H

#include "w_main.h"

#define ADC_POWER   0x80            //ADC电源控制位
#define ADC_FLAG    0x20            //ADC完成标志
#define ADC_START   0x40            //ADC起始控制位
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟


void W_ADCConfig(void);
uint16_t GetADCResult(uint8_t ch);

#endif