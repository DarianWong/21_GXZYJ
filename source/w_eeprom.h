#ifndef	_W_EEPROM_H
#define	_W_EEPROM_H

#include "w_main.h"

//EEPROM
#define SetParameterAddress 	0x0000//设置参数存放地址
#define	WaterAddress					0x0200	//制水量地址
#define	ZSTimeAddress					0x0400	//制水时间地址
#define	SysIpPort							0x0600	

#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
#define CMD_IDLE    0               //空闲模式
#define CMD_READ    1               //IAP字节读命令
#define CMD_PROGRAM 2               //IAP字节编程命令
#define CMD_ERASE   3               //IAP扇区擦除命令

void IapIdle(void);
uint8_t IapReadByte(uint16_t addr);
void IapProgramByte(uint16_t addr, uint8_t dat);
void IapEraseSector(uint16_t addr);
void PReadEeprom(uint16_t begin_addr, uint16_t counter, uint8_t array[]);//读E2数据到数组
//void WriteEeprom(uint16_t begin_addr, uint16_t counter, uint8_t array[]);//将某个数组的数据存放至E2

void WriteEeprom(uint16_t begin_addr, uint16_t counter, uint8_t *pArray);//将某个数组的数据存放至E2


#endif