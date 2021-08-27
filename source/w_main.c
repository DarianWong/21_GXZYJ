/*
20210529余孝阳要求每制水20升保存一次制水量，每制一升水发送一次数据给显示板进行显示

*/

#include "w_main.h"
#include "w_timer.h"
#include "w_uart.h"
#include "w_show.h"
#include "w_work.h"
#include "w_adc.h"
#include "w_eeprom.h"

extern uint8_t SoundFlag;
extern	bit Sys1sFlag,Sys100msFlag,Sys200msFLag,Sys1mFlag,SendShowStateFlag;
extern uint8_t code Cdatas[4];
extern bit TestFlag,PowerOn,ShutDown,NetWork;
extern uint8_t WorkStateByte,ShowState;
uint32_t EquipmentNumber=0;
extern uint8_t SendDataInterval,S2PacketNum,NetProce;
extern bit NetTimeFlag,PowerDown;

extern struct 
{ uint8_t		FirstChar;   //字头      1B
	uint16_t	CardPrice1;			//刷卡单价
	uint16_t	CardPrice2;			//刷卡单价
	uint16_t	VirtualPrice1;	//扫码单价
	uint16_t	VirtualPrice2;	//扫码单价
	uint16_t	Pluse1;					//1号口流量计脉冲数
	uint16_t	Pluse2;					//2号口流量计脉冲数
	uint16_t	Pluse3;					//制水流量脉冲设置
	uint16_t	MaxMoney;				//最大单次出水量
	
	uint8_t		ParameterLock;	//设备参数锁
//	uint8_t		Confiscate;		//没收时间				1B
	uint8_t 	OnceCXTime;	//单次冲洗时间					1B
	uint8_t		CXInterval;	//冲洗间隔							1B
	uint8_t		HaltState;//停机状态
		uint8_t 	OnceOzoneTime;//臭氧工作时间				1B
		uint8_t		OzoneInterval;//臭氧工作间隔时间		1B
	uint8_t 	LastChar;    //字尾      1B
}SetParameterData;
void main(void)
{
	SysInit();
	ReadEquipment();
	W_EepromData();
	S2PacketNum=0;
	W_Joint2001Data();
	W_Send2001Data_1();W_Send2001Data_2();
	WDT_CONTR=0x35;//
	
	while(1)
	{
		WDT_CONTR=0x35;//
//		if(Sys1sFlag)
//		{
//			Sys1sFlag=0;Uart1Send(UsedWater.LXLife1);Uart1Send(UsedWater.LXLife4);//Uart1Send(sizeof(UsedWater.ZSYield));Uart1Send((UsedWater.FirstChar));
//		}
		if(ShutDown)//如果处于关机状态
		{
			WorkStateByte&=0xfe;ShowState&=0xfe;
			//if(PowerOn==0)WDT_CONTR=0x35;关闭显示//memset(DisBuf,0,16);Update_Display();
			W_CloseAll();//关闭所有输出
		}
		else
		{
			ShowState|=0x01;
			if(TestFlag)
			{
				W_OpenAll();W_CheckWaterLevel();
			}
			else
			{
//				if(Sys100msFlag){Sys100msFlag=0;Uart2Send(SetParameterData.HaltState);}
				W_WorkLogic();
			}
			W_Menu();
		}
		W_Show();
		if(SendShowStateFlag)
		{
			SendShowStateFlag=0;W_SendShowState();
		}
//		NetProce=0x50;NetWork=1;
		if(NetWork==0||NetProce<0x50){if(NetTimeFlag){NetTimeFlag=0;if(PowerDown)InitModel();}}
		if(SendDataInterval==0&&NetWork){SendDataJoint();}
		UartDataDispose();//NetProce=0x50;NetWork=1;//Uart2Send(2);
//		Uart1Send(1);Uart3Send(3);Uart4Send(4);
	}
}
void SysInit(void)//系统初始化
{
	P0M0 = 0x90;P0M1 = 0x10;       //设置P04为开漏输出口，其余准双向
	P1M0 = 0x00;P1M1 = 0xf8;       //设置P13-P17为ADC口
	P2M0 = 0x40;P2M1 = 0x4e;       //设置P04为输出口
	P3M0 = 0x80;P3M1 = 0x00;       //设置P3全为准双向
	P4M0 = 0x16;P4M1 = 0x00;       //设置P41,P44为输出口
	P5M0 = 0x01;P5M1 = 0x00;       //设置P5全为准双向
	P6M0 = 0x00;P6M1 = 0xc3;       //设置P63为输出口,P60,61,66,67为高阻输入
	P7M0 = 0x0f;P7M1 = 0x00;       //设置P70-P74为输出口
	W_CloseAll();
	M800Power=1;
	W_TimerConfig();
	W_ExintConfig();
	W_ADCConfig();
	W_UartConfig();
	EA=1;
	Update_Display();
	pcf_init();Readtime();
	
}
void ReadEquipment()
{
	EquipmentNumber=Cdatas[0];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[1];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[2];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[3];
	
}