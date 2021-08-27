#include "w_show.h"

extern bit SZFlag,TestFlag,ShutDown;
extern uint8_t SetCnt,TestCnt;
extern bit WaterLowOk,WaterMiddleOk,WaterHighOk;
//extern uint8_t	Year,Month,Day,Hour,Minute,Second,Week;
extern uint8_t	WaterState,NetProce,ErrorTemp;
extern uint8_t ShowEquipmentTime,ShowEquipmentCnt;
extern uint32_t EquipmentNumber;
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
uint8_t DisBuf[16]={0};           // 显示缓存


uint8_t code DispData[32]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,   //0--9
                         0x77,0x7c,0x39,0x5e,0x79,0x71,                       //A--F
                         0x00,0x37,0x73,0x76,0x40,0x08,0xa3,0x50,0x0e,0x38,  //SP N P H - _ ? r J L
                         0x48,0x1c,0x54,0x31,0x5c,0x6e};   //= u n o

void W_Show(void)
{
	uint8_t ch1=0,i=0;
	uint16_t int1=0;
	
	memset(DisBuf,0,16);
	if(ShutDown==0)
	{
		if(TestFlag)
		{
			W_TestShow();
		}
		else
		{
			if(ShowEquipmentTime)
			{
				switch(ShowEquipmentCnt)
				{
					case 1:
						int1=EquipmentNumber/10000;
						ch1=int1/100; DisBuf[0]=DispData[ch1/10];DisBuf[1]=DispData[ch1%10];
						ch1=int1%100; DisBuf[2]=DispData[ch1/10];DisBuf[13]=DispData[ch1%10];
					break;
					case 2:
						int1=EquipmentNumber%10000;
						ch1=int1/100; DisBuf[0]=DispData[ch1/10];DisBuf[1]=DispData[ch1%10];
						ch1=int1%100; DisBuf[2]=DispData[ch1/10];DisBuf[13]=DispData[ch1%10];
					break;
					default:break;
				}
			}
			else
			{
				if(SZFlag)W_MenuShow();
				else 
				{
					if(ErrorTemp)
					{
						DisBuf[0]=DispData[Disp_E];DisBuf[1]=DispData[8];DisBuf[2]=DispData[ErrorTemp%10];
					}
					else
					{
						DisBuf[0]=DispData[Disp_P];DisBuf[1]=DispData[WaterState%10];
					}
					DisBuf[11]=DispData[NetProce/0x10];
				}
			}
		}
	}
	Update_Display();
}
void W_TestShow(void)
{
	switch(TestCnt)
	{
		case 0:
			memset(DisBuf,0xff,3);
		break;
		case 1:
			memset(&DisBuf[11],0xff,3);
		break;
		case 2:
			if(GYKey==0)DisBuf[0]=DispData[1];else DisBuf[0]=DispData[0];
			if(DYKey==0)DisBuf[1]=DispData[1];else DisBuf[1]=DispData[0];
//			if(LowTension)DisBuf[2]=DispData[1];else DisBuf[2]=DispData[0];
			if(WaterLowOk)DisBuf[13]=DispData[1];else DisBuf[13]=DispData[0];
			if(WaterMiddleOk)DisBuf[12]=DispData[1];else DisBuf[12]=DispData[0];
			if(WaterHighOk)DisBuf[11]=DispData[1];else DisBuf[11]=DispData[0];
		break;
		default:break;
	}
}
void W_MenuShow(void)
{
	switch(SetCnt)
	{
		case 0:
		break;
		case 1://1AA,1号口刷卡单价
			DisBuf[0]=DispData[1];DisBuf[1]=DispData[Disp_A];DisBuf[2]=DispData[Disp_A];
			DispSetData(SetParameterData.CardPrice1);
		break;
		case 2://1AB,2号口刷卡单价
			DisBuf[0]=DispData[1];DisBuf[1]=DispData[Disp_A];DisBuf[2]=DispData[Disp_B];
			DispSetData(SetParameterData.CardPrice2);
		break;
		case 3://2AA，1号扫码单价
			DisBuf[0]=DispData[2];DisBuf[1]=DispData[Disp_A];DisBuf[2]=DispData[Disp_A];
			DispSetData(SetParameterData.VirtualPrice1);
		break;
		case 4://2Ab，2号扫码单价
			DisBuf[0]=DispData[2];DisBuf[1]=DispData[Disp_A];DisBuf[2]=DispData[Disp_B];
			DispSetData(SetParameterData.VirtualPrice2);
		break;
		case 5://1号口流量计脉冲数
			DisBuf[0]=DispData[3];DisBuf[1]=DispData[Disp_A];//DisBuf[2]=DispData[Disp_A];
			DispSetData(SetParameterData.Pluse1);
		break;
		case 6://2号口流量计脉冲数
			DisBuf[0]=DispData[4];DisBuf[1]=DispData[Disp_A];//DisBuf[2]=DispData[Disp_B];
			DispSetData(SetParameterData.Pluse2);
		break;
		case 7://总净水流量计脉冲数
			DisBuf[0]=DispData[5];DisBuf[1]=DispData[Disp_A];//DisBuf[2]=DispData[Disp_B];
			DispSetData(SetParameterData.Pluse3);
		break;
		case 8://最大消费金额3A
			DisBuf[0]=DispData[6];DisBuf[1]=DispData[Disp_A];//DisBuf[2]=DispData[1];
			DispSetData(SetParameterData.MaxMoney);
		break;
		case 9://冲洗时间
			DisBuf[0]=DispData[1];DisBuf[1]=DispData[Disp_B];
			DispSetData(SetParameterData.OnceCXTime);
		break;
		case 10://冲洗间隔
			DisBuf[0]=DispData[2];DisBuf[1]=DispData[Disp_B];
			DispSetData(SetParameterData.CXInterval);
		break;
		case 11://臭氧时间
			DisBuf[0]=DispData[3];DisBuf[1]=DispData[Disp_B];
			DispSetData(SetParameterData.OnceOzoneTime);
		break;
		case 12://臭氧间隔
			DisBuf[0]=DispData[4];DisBuf[1]=DispData[Disp_B];
			DispSetData(SetParameterData.OzoneInterval);
		break;
		case 13://年
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[1];
			DispSetData(SysTime.Year);
		break;
		case 14://月
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[2];
			DispSetData(SysTime.Month);
		break;
		case 15://日
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[3];
			DispSetData(SysTime.Day);
		break;
		case 16://时
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[4];
			DispSetData(SysTime.Hour);
		break;
		case 17://分
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[5];
			DispSetData(SysTime.Minute);
		break;
		case 18://秒
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[6];
			DispSetData(SysTime.Second);
		break;
		case 19://星期
			DisBuf[0]=DispData[Disp_F];DisBuf[1]=DispData[7];
//			if(SysTime.Week==0||SysTime.Week>7)DispSetData(7);
//			else 
			DispSetData(SysTime.Week);
		break;
		default:break;
	}
}



void  DispSetData(uint16_t int1)
{
	uint8_t ch1=0,ch2=0;
	ch1=int1/100;ch2=int1%100;
	if(ch1>9)DisBuf[2]=DispData[ch1/10];
	if(ch1)DisBuf[13]=DispData[ch1%10];
	if(ch1||ch2>9)DisBuf[12]=DispData[ch2/10];
	DisBuf[11]=DispData[ch2%10];
	if(SetCnt==8)//单次消费最大金额以分为单位
	{
		DisBuf[13]=DispData[ch1%10]|0x80;
		DisBuf[12]=DispData[ch2/10];
	}
}
//--------------1640显示
/********************Start函数*************************/
void I2CStart(void)
{
	DIN=1;
	CLK=1;
	_nop_();
	_nop_();
	DIN=1; 
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CLK=0;
	_nop_();
	_nop_();
}
/********************Stop函数*************************/
void I2CStop(void)
{
	CLK=1;
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
	DIN=1;
	_nop_();
	_nop_();
	CLK=0;
	DIN=0;

}

/***************发送8bit数据，从低位开始**************/
void I2CWritebyte(uint8_t oneByte)
{
  uint8_t i;
  for(i=0;i<8;i++)
  {
    CLK=0;
	if(oneByte&0x01) 
	  DIN=1;
	else 
	  DIN=0;
	_nop_();
	_nop_();
	_nop_();
	CLK=1;
	oneByte=oneByte>>1;
	}
	CLK=0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
}

/************显示函数，数码管1~16位显示0~F************/
void Update_Display(void)
{
 uint8_t i;

 I2CStart();
 I2CWritebyte(0x40);	//数据命令设置：普通模式，地址自动加一
 I2CStop();

 I2CStart();
 I2CWritebyte(0xc0);	//地址命令设置：初始地址00H
 for(i=0;i<16;i++)	//发送16位显示数据
 {
   I2CWritebyte(DisBuf[i]);
 }
 I2CStop();

 I2CStart();
 I2CWritebyte(0x8c);	//显示控制：显示开，脉冲宽度设为11/16
 I2CStop();
}
