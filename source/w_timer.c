#include "w_timer.h"

bit Sys100msFlag=0,Sys1sFlag=0,Sys200msFlag=0,Sys1mFlag=0,SendShowStateFlag=0;
extern bit ZSFlag;
extern bit TDS1flag,TDS2flag;
extern uint16_t TDS1Pluse,TDS2Pluse,CSTDS,YSTDS,TDS5ms1,TDS5ms2,TDSTemp;
uint8_t	AutoChange1ms=0,Test1s=0,SZKeyTime100ms=0;
bit SaveSetDataFlag=0;
extern bit TZKeyFlag,AutoChange,TestKeyFlag,TestFlag,SZKeyFlag,SZFlag;
uint16_t	Sys1ms=0;
extern bit NetTimeFlag,PowerDown;
extern uint8_t PowerDown1s;
extern bit TimingFlag,ZSTimeFlag,StateFlag,HeartFlag;
extern uint8_t TimingTime,StateTime,HeartTime;
extern uint8_t SendDataInterval;//数据包间隔
extern uint8_t SoundFlag;

extern struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Week;
	
}SysTime;
void W_TimerConfig(void)
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0=1;
}
void tm0_isr(void) interrupt 1
{
	Sys1ms++;
	SoundCnt();
	ReceiveTimeOut();
	if(TZKeyFlag)AutoChange1ms++;
	else AutoChange1ms=0;
	if(AutoChange1ms>=200){AutoChange1ms=0;AutoChange=1;}
	if(Sys1ms%5==0)
	{
		if(ZSFlag)
		{
			TDS5ms1++;
			if(TDS5ms1>=1000)
			{
				TDS5ms1=0;
				YSTDS=CalTDS(TDS1Pluse);TDS1Pluse=0;//TDS1flag=0;
				CSTDS=CalTDS(TDS2Pluse);TDS2Pluse=0;
//				if(CSTDS>YSTDS)
//				{
//					TDSTemp=CSTDS;CSTDS=YSTDS;YSTDS=TDSTemp;
//				}
			}
		}
		else {TDS5ms1=0,TDS1Pluse=0;TDS2Pluse=0;}//TDS1flag=0;TDS5ms2=0,TDS2flag=0;
	}
	if(Sys1ms%100==0)
	{
		Sys100msFlag=1;
		if(SendDataInterval)SendDataInterval--;//数据包间隔
		if(TestKeyFlag)Test1s++;
		else Test1s=0;
		if(SZKeyFlag&&(SZFlag||TestFlag))SZKeyTime100ms++;
		else SZKeyTime100ms=0;
		if(SZKeyTime100ms>=30){SZKeyTime100ms=0;SaveSetDataFlag=1;SoundFlag=1;}
		if(Test1s>=30){TestFlag=1;Test1s=0;}
	}
	if(Sys1ms%200==0)
	{
		Sys200msFlag=1;NetTimeFlag=1;
	}
	if(Sys1ms%1000==0)
	{
		Sys1sFlag=1;SendShowStateFlag=1;
		if(PowerDown==0)PowerDown1s++;
		if(PowerDown1s==3){M800Power=1;}//断电2秒钟
		if(PowerDown1s>=15){PowerDown1s=0;PowerDown=1;}
		if(HeartTime)HeartTime--;
		else HeartFlag=1;
		SysTime.Second++;
		if(SysTime.Second>59)
		{
			SysTime.Second=0;SysTime.Minute++;
			if(SysTime.Minute>59)
			{
				SysTime.Minute=0;SysTime.Hour++;//Sys1mFlag=1;
				if(SysTime.Hour>23)SysTime.Hour=0;
			}
		}
	}
	if(Sys1ms%60000==0)
	{
		Sys1ms=0;
		Sys1mFlag=1;
		if(TimingTime)TimingTime--;
		else {TimingFlag=1;}
		if(StateTime)StateTime--;
		else StateFlag=1;
	}
}