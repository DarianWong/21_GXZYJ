#ifndef	_W_WORK_H
#define	_W_WORK_H

#include "w_main.h"
#include "w_adc.h"
#include "w_eeprom.h"
#include "w_uart.h"


sbit	GYKey=P6^0;//高压开关
sbit	DYKey=P6^1;//低压开关
sbit	SZKey=P6^6;//设置键
sbit	TZKey=P6^7;//调整键


sbit	Buzzer=P5^0;
sbit	ZSPump1=P7^2;//制水泵1
sbit	ZSPump2=P7^1;//制水泵2
sbit	CXValve1=P4^1;//冲洗阀
sbit	CXValve2=P7^0;//冲洗阀
sbit	InflowValve=P3^7;//进水阀
sbit	ProbePower=P4^4;//水位探针电源
sbit	WaterKey1=P0^4;//出水口一水开关
sbit	WaterKey2=P2^6;//出水口
sbit	Ozone=P7^3;//紫外线



#define	ZSPumpOpen				{ZSPump1=1;ZSPump2=1;}
#define	ZSPumpClose 			{ZSPump1=0;ZSPump2=0;}
#define	WashValveOpen			{CXValve1=1;CXValve2=1;}
#define	WashValveClose		{CXValve1=0;CXValve2=0;}
#define	InflowValveOpen		InflowValve=1
#define	InflowValveClose	InflowValve=0
#define	ProbePowerOpen		ProbePower=1
#define	ProbePowerClose		ProbePower=0
#define	WaterKeyOpen			{WaterKey1=1;WaterKey2=1;}
#define	WaterKeyClose			{WaterKey1=0;WaterKey2=0;}
#define	OzoneOpen					Ozone=1;
#define	OzoneClose				Ozone=0;


#define	BUZZER_Open			Buzzer=1
#define	BUZZER_Close		Buzzer=0

#define EX2             0x10


uint16_t CalTDS(uint16_t PlusNum);//根据脉冲数PlusNum，计算TDS值并返回 
void W_ExintConfig(void);
void SoundCnt(void);
void Delay1ms(uint16_t dat);		//@24.000MHz
void W_SendShowState(void);
void W_CheckWaterLevel(void);//每秒钟检测水位一次，检测一次进行10次AD转换，求平均值
void W_TestShow(void);
void W_WaterYield(void);
void OzoneControl(void);
void W_CloseAll(void);//关闭所有负载
void W_OpenAll(void);//打开所有负载
void W_Menu(void);
void W_WorkLogic(void);
void W_EepromData(void);

#endif