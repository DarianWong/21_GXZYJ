#ifndef	_W_EEPROM_H
#define	_W_EEPROM_H

#include "w_main.h"

//EEPROM
#define SetParameterAddress 	0x0000//���ò�����ŵ�ַ
#define	WaterAddress					0x0200	//��ˮ����ַ
#define	ZSTimeAddress					0x0400	//��ˮʱ���ַ
#define	SysIpPort							0x0600	

#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
#define CMD_IDLE    0               //����ģʽ
#define CMD_READ    1               //IAP�ֽڶ�����
#define CMD_PROGRAM 2               //IAP�ֽڱ������
#define CMD_ERASE   3               //IAP������������

void IapIdle(void);
uint8_t IapReadByte(uint16_t addr);
void IapProgramByte(uint16_t addr, uint8_t dat);
void IapEraseSector(uint16_t addr);
void PReadEeprom(uint16_t begin_addr, uint16_t counter, uint8_t array[]);//��E2���ݵ�����
//void WriteEeprom(uint16_t begin_addr, uint16_t counter, uint8_t array[]);//��ĳ����������ݴ����E2

void WriteEeprom(uint16_t begin_addr, uint16_t counter, uint8_t *pArray);//��ĳ����������ݴ����E2


#endif