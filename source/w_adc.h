#ifndef	_W_ADC_H
#define	_W_ADC_H

#include "w_main.h"

#define ADC_POWER   0x80            //ADC��Դ����λ
#define ADC_FLAG    0x20            //ADC��ɱ�־
#define ADC_START   0x40            //ADC��ʼ����λ
#define ADC_SPEEDLL 0x00            //540��ʱ��
#define ADC_SPEEDL  0x20            //360��ʱ��
#define ADC_SPEEDH  0x40            //180��ʱ��
#define ADC_SPEEDHH 0x60            //90��ʱ��


void W_ADCConfig(void);
uint16_t GetADCResult(uint8_t ch);

#endif