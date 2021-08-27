#include "w_work.h"

bit ZSFlag=0,MSFlag=0,WashFlag=0,QSFlag=0;
bit TDS1flag=0,TDS2flag=0;
uint16_t TDS1Pluse=0,TDS2Pluse=0,CSTDS=0,YSTDS=0,TDSTemp=0;
uint16_t	TDS5ms1=0,TDS5ms2=0;
uint16_t FlowPulse=0,WaterYield_L=0;//流量计，水量,WaterYield=0
uint8_t	SoundNum=0,SoundFlag=0;
uint8_t	WashTime=0;
extern	bit Sys1sFlag,Sys100msFlag,Sys200msFLag,Sys1mFlag;
uint16_t LowWaterData=0,MiddleWaterData=0,HighWaterData=0,LowWaterDataOk=0,MiddleWaterDataOk=0,HighWaterDataOk=0,LeakData=0;
uint8_t	 WaterLevelCnt=0;
uint8_t LeakCnt=0;
bit WaterLowOk=0,WaterMiddleOk=0,WaterHighOk=0;
uint8_t	ErrorTemp=0;//故障代码
uint8_t	WaterState=0,SellDelayTime=0;//水位状态
bit SellState=0;//售卖状态
uint8_t OzoneTime=0,OzoneTime1h=0,OzoneTime1m=0;
bit SZKeyFlag=0,SZFlag=0,TZKeyFlag=0,AutoChange=0,TestKeyFlag=0,TestFlag=0,LeakFlag=0;
bit SetPriceFlag=0,SetTimeFlag=0,SetOzoneFlag=0;
uint8_t	SetCnt=0,TestCnt=0;
uint8_t	Year=0,Month=0,Day=0,Hour=0,Minute=0,Second=0,Week=0;
//bit AlterParameter1=0,AlterParameter2=0;
uint8_t WorkStateByte=0,WorkStateLast=0;
uint8_t ZSTime1s=0,CXInterval1m=0,ZSTime1m=0;
//uint16_t ZSTime1h=0;//制水时计时器
extern bit StateFlag,HeartFlag,PowerDown,ZSTimeFlag,NetWork;
extern uint8_t NetProce;
bit NetTimeFlag;
extern uint8_t UartSendDataBuf[50];
uint8_t AutoExit=0;
uint8_t ShowEquipmentTime=0,ShowEquipmentCnt=0;
uint8_t WaterLevelState=0,ShowState=0,LastWaterLevel=0,LastShowState=0,LastErrorTemp=0;
bit SendShowFlag=0;
uint8_t SendShowTime=0;
extern uint8_t AutoChange1ms,SZKeyTime100ms;
extern bit SaveSetDataFlag;
uint16_t InflowCloseTime=0;//低压开关断开是的进水阀延时关闭时间
bit InflowValveCloseFlag=0;

#ifdef	TestServer
extern	uint8_t code Domain[];
extern	uint8_t code Port[];
#else
extern	uint8_t code Domain[];  //域名,第一字节为域名长度http://testmanage.waterer.cn/
extern	uint8_t code Port[];
#endif
extern uint8_t IportBuf[30];
////uint8_t PowerDown1s;,PowerDown

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
	uint16_t	ZSTime;			//制水时间
	uint32_t	ZSYield;		//制水量
	uint8_t		LXLife1;	//1级滤芯寿命百分比
	uint8_t		LXLife2;	//		1B
	uint8_t 	LXLife3;	//					1B
	uint8_t		LXLife4;	//							1B
	uint8_t 	LXLife5;	//				1B
	uint8_t		ZSTime1m;//制水分钟数
	uint8_t 	LastChar; //字尾      1B
}UsedWater;

//union ActDataGu
//{
//	uint8_t DataStr[22];
//	
//}EpromDataGu;
struct 
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

struct
{ uint16_t  TDSVal;      //TDS值
  uint16_t  TDSPlus;     //TDS值对应的脉冲值
  uint8_t  TDSper;      //每个TDS对应脉冲数
}code TdsData[6]={{0,0,0},{16,537,34},{50,1487,28},{100,2761,25},{200,4531,18},{300,5928,14}};//重新校准TDS数据表

uint16_t CalTDS(uint16_t PlusNum)//根据脉冲数PlusNum，计算TDS值并返回 
{ uint8_t ch1,ch2;
  uint16_t int1;
  for(ch1=1;ch1<6;++ch1){if(PlusNum<TdsData[ch1].TDSPlus) break;}
  if(ch1<6)
  { int1=PlusNum-TdsData[ch1-1].TDSPlus;  //求本段脉冲数
    ch2=int1%TdsData[ch1].TDSper;
    if(ch2>(TdsData[ch1].TDSper/2)) ch2=1;
    else ch2=0;
    int1/=TdsData[ch1].TDSper;    //本段脉冲对应的TDS
    int1+=TdsData[ch1-1].TDSVal;  //加本段前边的TDS值
  }
  else      //TDS大于300
  { int1=PlusNum-TdsData[5].TDSPlus;   //减去前边的
    int1/=TdsData[5].TDSper;
    int1+=TdsData[5].TDSVal;
  }
  return int1;
}


//WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr);
void W_Menu(void)
{
	uint8_t KeyValue=0;
	if(SZKey)KeyValue&=0xfd;
	else KeyValue|=0x02;
	if(TZKey)KeyValue&=0xfe;
	else KeyValue|=0x01;
//	Uart1Send(KeyValue);
	if(KeyValue!=1)AutoChange1ms=0;
	switch(KeyValue)
	{
		case 1://调整键按下
			if(TestKeyFlag==0)TZKeyFlag=1;//TZKeyFlag=0;TestKeyFlag=0;
			if(AutoChange)
			{
				AutoChange=0;AutoExit=0;
				if(SetParameterData.ParameterLock==0)
				{
					if(SetCnt>0&&SetCnt<9)SetPriceFlag=1;//如果是调节单价，调节完成之后需要发送0108设备状态同步指令
					else if(SetCnt>10&&SetCnt<13)SetOzoneFlag=1;
					else if(SetCnt>12&&SetCnt<20)SetTimeFlag=1;//如果调节时间
					else {}
					switch(SetCnt)
					{
						case 0:
							
						break;
						case 1://1号口刷卡单价
							++SetParameterData.CardPrice1;
							if(SetParameterData.CardPrice1>250)SetParameterData.CardPrice1=1;
						break;
						case 2://1号口扫码单价
							++SetParameterData.CardPrice2;
							if(SetParameterData.CardPrice2>250)SetParameterData.CardPrice2=1;
						break;
						case 3://2号口刷卡单价
							++SetParameterData.VirtualPrice1;
							if(SetParameterData.VirtualPrice1>250)SetParameterData.VirtualPrice1=1;
						break;
						case 4://2号口扫码单价
							++SetParameterData.VirtualPrice2;
							if(SetParameterData.VirtualPrice2>250)SetParameterData.VirtualPrice2=1;
						break;
						case 5://1号口流量计脉冲数
							SetParameterData.Pluse1+=10;
							if(SetParameterData.Pluse1>2000)SetParameterData.Pluse1=100;
							else if(SetParameterData.Pluse1<100)SetParameterData.Pluse1=540;
							else{}
						break;
						case 6://2号口流量计脉冲数
							SetParameterData.Pluse2+=10;
							if(SetParameterData.Pluse2>2000)SetParameterData.Pluse2=100;
							else if(SetParameterData.Pluse2<100)SetParameterData.Pluse2=540;
							else{}
						break;
						case 7://总净水流量计脉冲数
							SetParameterData.Pluse3+=10;
							if(SetParameterData.Pluse3>2000)SetParameterData.Pluse3=100;
							else if(SetParameterData.Pluse3<100)SetParameterData.Pluse3=540;
							else{}
						break;
						case 8://最大单次出水量
							SetParameterData.MaxMoney+=10;
							if(SetParameterData.MaxMoney>9999||SetParameterData.MaxMoney<50)SetParameterData.MaxMoney=500;
						break;
						case 9://冲洗时间
							++SetParameterData.OnceCXTime;
							if(SetParameterData.OnceCXTime>240||SetParameterData.OnceCXTime<5)SetParameterData.OnceCXTime=5;					
						break;
						case 10://冲洗间隔
							++SetParameterData.CXInterval;
							if(SetParameterData.CXInterval>240||SetParameterData.CXInterval<30)SetParameterData.CXInterval=30;
						break;
						case 11://臭氧工作时间
							++SetParameterData.OnceOzoneTime;
							if(SetParameterData.OnceOzoneTime>24)SetParameterData.OnceOzoneTime=0;
						break;
						case 12://臭氧工作间隔
							++SetParameterData.OzoneInterval;
							if(SetParameterData.OzoneInterval>24)SetParameterData.OzoneInterval=0;
						break;
						case 13://年
							++SysTime.Year;
							if(SysTime.Year>99||SysTime.Year<20)SysTime.Year=20;
						break;
						case 14://月
							++SysTime.Month;
							if(SysTime.Month>12)SysTime.Month=1;
						break;
						case 15://日
							++SysTime.Day;
							if(SysTime.Day>31)SysTime.Day=1;
						break;
						case 16://时
							++SysTime.Hour;
							if(SysTime.Hour>23)SysTime.Hour=0;
						break;
						case 17://分
							++SysTime.Minute;
							if(SysTime.Minute>59)SysTime.Minute=0;
						break;
						case 18://秒
							++SysTime.Second;
							if(SysTime.Second>59)SysTime.Second=0;
						break;
						case 19://星期
							++SysTime.Week;
							if(SysTime.Week>7)SysTime.Week=1;
						break;
						default:break;
					}
				
				}
			}	
		break;
		case 2://设置键按下
			if(TestKeyFlag==0)SZKeyFlag=1;//TestKeyFlag=0;SZKeyFlag=0;
		break;
		case 3://两个键同时按下
			TestKeyFlag=1;TZKeyFlag=0;SZKeyFlag=0;
		break;
		default://松手了
//		Uart1Send(1);
			if(SZKeyFlag)
			{
				SZKeyFlag=0;
				if(TestFlag)//while(1);
				{
					if((TestCnt==0)&&SaveSetDataFlag)//在测试状态测试一模式下长按设置键3秒清楚制水量
					{
						UsedWater.ZSYield=0;
						WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);
						while(1);
					}
					TestCnt++;
//					if(TestCnt==3)Audio_Play(25);
					if(TestCnt>2){while(1);}
				}
				else
				{
					if(SaveSetDataFlag)//长按设置键3秒直接保存退出
					{
						SaveSetDataFlag=0;
						SetCnt=0;SZFlag=0;//此处需要增加标志位，来判断刷卡板是否收到，若收到则保存EEPROM，未收到则不保存。
//						AlterParameter1=1;AlterParameter2=1;//修改设备参数，若有回复，则认为成功
						//需要发送修改参数指令给刷卡板，2001指令
//						WriteEeprom(SetParameterAddress,sizeof(SetParameterData),SetParameterData);
						if(SetPriceFlag||SetTimeFlag)//设置时间或者单价之类的参数需要退出之后需要发送给出水口
						{
//								SetTimeFlag=0;
							W_Joint2001Data();
							W_Send2001Data_1();W_Send2001Data_2();
						}
						if(SetPriceFlag)
						{
							SetPriceFlag=0;
							WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //保存SaveParameterFlag=0;WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);
						}
						if(SetTimeFlag)
						{
							SetTimeFlag=0;time_init(SysTime.Year,SysTime.Month,SysTime.Day,SysTime.Hour,SysTime.Minute,SysTime.Second,SysTime.Week);//SoundFlag=3;
							W_Joint2001Data();W_Send2001Data_3();
						}
						if(SetOzoneFlag)
						{
							SetOzoneFlag=0;WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); OzoneTime=SetParameterData.OnceOzoneTime*60;
						}
					}
					else
					{
						SetCnt++;
						if(SetCnt>19){SetCnt=0;SZFlag=0;SetPriceFlag=0;SetTimeFlag=0;SetOzoneFlag=0;PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);}//自动退出不保存参数
						else SZFlag=1;
					}
				}
				SoundFlag=1;AutoExit=0;//
			}
			if(TZKeyFlag)
			{
				TZKeyFlag=0;
				if(SZFlag==0)
				{
					if(ShowEquipmentCnt>=2){ShowEquipmentCnt=0;ShowEquipmentTime=0;}
					else {ShowEquipmentCnt++;ShowEquipmentTime=2;}
					
				}
				if(SetParameterData.ParameterLock==0)
				{
					if(SetCnt>0&&SetCnt<9)SetPriceFlag=1;//如果是调节单价，调节完成之后需要发送0108设备状态同步指令
					else if(SetCnt>10&&SetCnt<13)SetOzoneFlag=1;
					else if(SetCnt>12&&SetCnt<20)SetTimeFlag=1;//如果调节时间
					else {}
					switch(SetCnt)
					{
						case 0:
							
						break;
						case 1://1号口刷卡单价
							++SetParameterData.CardPrice1;
							if(SetParameterData.CardPrice1>250)SetParameterData.CardPrice1=1;
						break;
						case 2://2号口刷卡单价
							++SetParameterData.CardPrice2;
							if(SetParameterData.CardPrice2>250)SetParameterData.CardPrice2=1;
						break;
						case 3://1号口扫码单价
							++SetParameterData.VirtualPrice1;
							if(SetParameterData.VirtualPrice1>250)SetParameterData.VirtualPrice1=1;
						break;
						case 4://2号口扫码单价
							++SetParameterData.VirtualPrice2;
							if(SetParameterData.VirtualPrice2>250)SetParameterData.VirtualPrice2=1;
						break;
						case 5://1号口流量计脉冲数
							SetParameterData.Pluse1+=1;
							if(SetParameterData.Pluse1>2000)SetParameterData.Pluse1=100;
							else if(SetParameterData.Pluse1<100)SetParameterData.Pluse1=540;
							else{}
						break;
						case 6://2号口流量计脉冲数
							SetParameterData.Pluse2+=1;
							if(SetParameterData.Pluse2>2000)SetParameterData.Pluse2=100;
							else if(SetParameterData.Pluse2<100)SetParameterData.Pluse2=540;
							else{}
						break;
						case 7://总净水流量计脉冲数
							SetParameterData.Pluse3+=10;
							if(SetParameterData.Pluse3>2000)SetParameterData.Pluse3=100;
							else if(SetParameterData.Pluse3<100)SetParameterData.Pluse3=540;
							else{}
						break;
						case 8://最大单次出水量
							SetParameterData.MaxMoney+=10;
							if(SetParameterData.MaxMoney>9999||SetParameterData.MaxMoney<50)SetParameterData.MaxMoney=500;
						break;
						case 9://冲洗时间
							++SetParameterData.OnceCXTime;
							if(SetParameterData.OnceCXTime>240||SetParameterData.OnceCXTime<5)SetParameterData.OnceCXTime=5;					
						break;
						case 10://冲洗间隔
							++SetParameterData.CXInterval;
							if(SetParameterData.CXInterval>240||SetParameterData.CXInterval<30)SetParameterData.CXInterval=30;
						break;
						case 11://臭氧工作时间
							++SetParameterData.OnceOzoneTime;
							if(SetParameterData.OnceOzoneTime>24)SetParameterData.OnceOzoneTime=0;
						break;
						case 12://臭氧工作间隔
							++SetParameterData.OzoneInterval;
							if(SetParameterData.OzoneInterval>24)SetParameterData.OzoneInterval=0;
						break;
							case 13://年
								++SysTime.Year;
								if(SysTime.Year>99||SysTime.Year<20)SysTime.Year=20;
							break;
							case 14://月
								++SysTime.Month;
								if(SysTime.Month>12)SysTime.Month=1;
							break;
							case 15://日
								++SysTime.Day;
								if(SysTime.Day>31)SysTime.Day=1;
							break;
							case 16://时
								++SysTime.Hour;
								if(SysTime.Hour>23)SysTime.Hour=0;
							break;
							case 17://分
								++SysTime.Minute;
								if(SysTime.Minute>59)SysTime.Minute=0;
							break;
							case 18://秒
								++SysTime.Second;
								if(SysTime.Second>59)SysTime.Second=0;
							break;
							case 19://星期
								++SysTime.Week;
								if(SysTime.Week>7)SysTime.Week=1;
							break;
						default:break;
					}
				}
				SoundFlag=1;AutoExit=0;
			}
			if(TestKeyFlag)TestKeyFlag=0;
			if(AutoExit>10)//自动退出时间到，保存设置参数
			{
				SZFlag=0;SetCnt=0;SetPriceFlag=0;SetTimeFlag=0;SetOzoneFlag=0;
				PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);
//				if(SetPriceFlag||SetTimeFlag)
//				{
//					W_Joint2001Data();
//					W_Send2001Data_1();W_Send2001Data_2();
//				}
//				if(SetPriceFlag)
//				{
//					
//					WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //保存
//				}
//				if(SetTimeFlag)
//				{
//					time_init(SysTime.Year,SysTime.Month,SysTime.Day,SysTime.Hour,SysTime.Minute,SysTime.Second,SysTime.Week);//SoundFlag=3;
//					W_Joint2001Data();W_Send2001Data_3();
//				}
			}
		break;
	}
}
void W_WorkLogic(void)
{
	
	if(Sys1sFlag)
	{
		Sys1sFlag=0;W_CheckWaterLevel();//
		if(LeakFlag||(SetParameterData.HaltState==1))//漏水或者滤芯寿命到期停机
		{
			if(LeakFlag)ErrorTemp|=0x08;else ErrorTemp|=0x02;W_CloseAll();
			ZSFlag=0;SellState=0;
		}
		else
		{
			ErrorTemp&=0xf7;
			if(DYKey)//如果低压开关断开
			{
				ZSPumpClose;WashValveClose;WashTime=0;//低压开关断开，关闭制水泵和冲洗阀
//				if(ZSFlag)WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);
				ZSFlag=0;
				if(GYKey)MSFlag=1;else MSFlag=0;
				ErrorTemp|=0x01;
				if(QSFlag==0){QSFlag=1;InflowCloseTime=600;}
				if(InflowCloseTime)InflowCloseTime--;
				else {InflowValveClose;InflowValveCloseFlag=1;}
			}
			else //低压开关闭合
			{
				QSFlag=0;ErrorTemp&=0xfe;InflowValveCloseFlag=0;
				if(WashTime){WashTime--;WashValveOpen;WashFlag=1;}
				else {WashValveClose;WashFlag=0;}
				if(ZSFlag)
				{
					ZSTime1s++;
					if(ZSTime1s>=60)
					{
						ZSTime1s=0;CXInterval1m++;UsedWater.ZSTime1m++;
						if(UsedWater.ZSTime1m%6==0){WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);} //SoundFlag=1;制水6分钟保存与水量相关的数据
						if(UsedWater.ZSTime1m>=60)
						{
							UsedWater.ZSTime1m=0;UsedWater.ZSTime++;ZSTimeFlag=1;//发送同步制水时间指令，收到回复则清零
						}
						if(CXInterval1m>=SetParameterData.CXInterval)//冲洗间隔时间到进行冲洗
						{
							CXInterval1m=0;WashTime=SetParameterData.OnceCXTime;//SoundFlag=9;
						}
					}
				}
				if(GYKey)//高压开关断开，表示水满
				{
					if(ZSFlag)
					{
						ZSFlag=0;WashTime=15;//WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);
					}
					if(WashTime==0)ZSPumpClose;//如果不在冲洗状态则关闭冲洗阀
				}
				else
				{
					ZSFlag=1;ZSPumpOpen;
				}
			}
			
			if(WaterLowOk)//到达低液位
			{
				if(WaterMiddleOk)
				{
					SellState=1;SellDelayTime=0;ErrorTemp&=0xfb;
					if(WaterHighOk)//水满状态
					{
						WaterState=5;WaterLevelState=3;InflowValveClose;MSFlag=1;
					}
					else//水位处于中液位和高液位之间
					{
						WaterLevelState=2;
						if(MSFlag){InflowValveClose;WaterState=6;}
						else {if(InflowValveCloseFlag==0)InflowValveOpen;WaterState=4;}
					}
				}
				else
				{
					MSFlag=0;
					if(WaterHighOk)//水位故障
					{
						InflowValveClose;ErrorTemp|=0x04;SellState=0;
					}
					else//水位处于低液位和中液位之间
					{
						WaterLevelState=1;if(InflowValveCloseFlag==0)InflowValveOpen;ErrorTemp&=0xfb;
						if(SellDelayTime)
						{
							SellDelayTime--;SellState=0;WaterState=3;
						}
						else {SellState=1;WaterState=4;}
					}
				}
			}
			else
			{
				SellState=0;MSFlag=0;
				if(WaterMiddleOk||WaterHighOk)//低液位未检测到，但是检测到中液位或搞液位了，则表示水位故障
				{
					InflowValveClose;ErrorTemp|=0x04;
				}
				else
				{
					if(InflowValveCloseFlag==0)InflowValveOpen;WaterState=2;WaterLevelState=0;ErrorTemp&=0xfb;SellDelayTime=180;//低液位以下时P2状态
				}
			}
			if(SellState){WaterKeyOpen;}
			else {WaterKeyClose;}
		}
		WorkStateByte|=0x01;
		if(SellState)WorkStateByte|=0x02;
		else WorkStateByte&=0xfd;
		if(SetParameterData.HaltState==1)WorkStateByte|=0x80;
		else WorkStateByte&=0x7f;
		if(MSFlag){WorkStateByte|=0x08;}//StateLed|=0x10;
		else {WorkStateByte&=0xf7;}//StateLed&=0xef;
		if(ZSFlag){WorkStateByte|=0x10;ShowState|=0x10;}//StateLed|=0x04;
		else {WorkStateByte&=0xef;ShowState&=0xef;}//StateLed&=0xfb;
		if(ErrorTemp&0x04)WorkStateByte|=0x20;
		else WorkStateByte&=0xdf;
		if(QSFlag){WorkStateByte|=0x04;ShowState|=0x04;}//StateLed&=0xfd;
		else {WorkStateByte&=0xfb;ShowState&=0xfb;}//StateLed|=0x02;
		if(WashTime)ShowState|=0x08;
		else ShowState&=0xf7;
		if(NetWork)ShowState|=0x40;
		else ShowState&=0xbf;
//		ShowState&=0xdf;//杀菌图标不显示
		if(YSTDS&&CSTDS)
		{
			if(YSTDS>=CSTDS)
			{
				if((((YSTDS-CSTDS)*100/YSTDS)>=85)&&SellState)ShowState|=0x02;//脱盐率大于85%表示可饮
				else ShowState&=0xfd;
			}
			else 
			{
				if((((CSTDS-YSTDS)*100/CSTDS)>=85)&&SellState)ShowState|=0x02;//脱盐率大于85%表示可饮
				else ShowState&=0xfd;
			}
		}
		else {if(SellState)ShowState|=0x02;else ShowState&=0xfd;}//只要有一个TDS为零则表示可饮
		if(WorkStateLast!=WorkStateByte)//状态改变发送状态包给服务器
		{
			WorkStateLast=WorkStateByte;StateFlag=1;
		}
		if(SZFlag)AutoExit++;//设置状态下无按键操作10秒自动保存退出
		else AutoExit=0;
		if(ShowEquipmentTime)
		{
			ShowEquipmentTime--;
			if(ShowEquipmentTime==0)
			if(ShowEquipmentCnt>=2)ShowEquipmentCnt=0;
			else {ShowEquipmentCnt++;ShowEquipmentTime=2;}
		}
		W_WaterYield();
	}
	if(Sys1mFlag)
	{
		Sys1mFlag=0;OzoneControl();
	}
}
void W_SendShowState(void)
{
	if(LastShowState!=ShowState||LastWaterLevel!=WaterLevelState||LastErrorTemp!=ErrorTemp)
	{
		LastShowState=ShowState;LastWaterLevel=WaterLevelState;LastErrorTemp=ErrorTemp;
		SendShowFlag=1;SendShowTime=1;
	}
	if(SendShowFlag)
	{
		if(SendShowTime)
		{
			SendShowTime--;
			if(SendShowTime==0)
			{
				SendShowTime=5;W_Joint2001Data();W_Send2001Data_3();
			}
		}
	}
	else SendShowTime=0;
}
void W_CheckWaterLevel(void)//每秒钟检测水位一次，检测一次进行10次AD转换，求平均值
{
	uint8_t i=0;
	ProbePowerOpen;Delay1ms(1);
	for(i=0;i<5;i++)
	{
		LowWaterData+=GetADCResult(5);
		MiddleWaterData+=GetADCResult(6);
		HighWaterData+=GetADCResult(7);
	}
	LowWaterData=LowWaterData/5;
	MiddleWaterData=MiddleWaterData/5;
	HighWaterData=HighWaterData/5;
	ProbePowerClose;
	LowWaterDataOk+=LowWaterData;
	MiddleWaterDataOk+=MiddleWaterData;
	HighWaterDataOk+=HighWaterData;
	WaterLevelCnt++;
	if(WaterLevelCnt>=5)
	{
		WaterLevelCnt=0;
		
		LowWaterDataOk=LowWaterDataOk/5;
		MiddleWaterDataOk=MiddleWaterDataOk/5;
		HighWaterDataOk=HighWaterDataOk/5;
		
		if(LowWaterDataOk<0x400)WaterLowOk=1;//{LowWaterData=0;}
		else if(LowWaterDataOk>0x600)WaterLowOk=0;
		else{}
		if(MiddleWaterDataOk<0x400)WaterMiddleOk=1;//{LowWaterData=0;}
		else if(MiddleWaterDataOk>0x600)WaterMiddleOk=0;
		else{}
		if(HighWaterDataOk<0x400)WaterHighOk=1;//{LowWaterData=0;}
		else if(HighWaterDataOk>0x600)WaterHighOk=0;
		else{}
	}
	
//	Uart1Send(LowWaterData/0x100);Uart1Send(LowWaterData%0x100);
//	Uart1Send(MiddleWaterData/0x100);Uart1Send(MiddleWaterData%0x100);
//	Uart1Send(HighWaterData/0x100);Uart1Send(HighWaterData%0x100);
	LowWaterData=0;MiddleWaterData=0;HighWaterData=0;
	LeakData+=GetADCResult(4);LeakCnt++;
	if(LeakCnt>4)
	{
		LeakCnt=0;LeakData/=5;
//		Uart1Send(LeakData/0x100);
//		Uart1Send(LeakData%0x100);
		if(LeakData<0x200)LeakFlag=1;LeakData=0;
	}
}
void W_EepromData(void)//上电检查EEPROM数据是否正常
{
	PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //读系统数据
	if(SetParameterData.FirstChar!=0x55||SetParameterData.LastChar!=0xaa)//数据头和尾不正确则把初始数据保存起来
	{
		SetParameterData.FirstChar=0x55;
		SetParameterData.CardPrice1=20;
		SetParameterData.CardPrice2=20;
		SetParameterData.VirtualPrice1=20;
		SetParameterData.VirtualPrice2=20;
		SetParameterData.Pluse1=540;
		SetParameterData.Pluse2=540;
		SetParameterData.Pluse3=540;
		SetParameterData.MaxMoney=500;
		SetParameterData.ParameterLock=0;
//		SetParameterData.Confiscate=3;
		SetParameterData.OnceCXTime=15;
		SetParameterData.CXInterval=120;
		SetParameterData.HaltState=0;
		SetParameterData.OnceOzoneTime=2;
		SetParameterData.OzoneInterval=4;
		SetParameterData.LastChar=0xaa;	
		WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //保存
		
		memcpy(IportBuf,Domain,Domain[0]+1);     //域名
    memcpy(&IportBuf[22],Port,5);IportBuf[27]=0x5a;//第28个字节是标志码，如果标志码不等于0x5a的话则需要重新填入IP地址
    WriteEeprom(SysIpPort,28,IportBuf); //把正确的IP地址写入到EEPROM
		SoundFlag=3;
	}
	else {PReadEeprom(SysIpPort,28,IportBuf);SoundFlag=1;}//
	PReadEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar); //读系统数据
	if(UsedWater.FirstChar!=0x55||UsedWater.LastChar!=0xaa)
	{
		UsedWater.FirstChar=0x55;
		UsedWater.ZSTime=0;
		UsedWater.ZSYield=0;
		UsedWater.LXLife1=100;
		UsedWater.LXLife2=100;
		UsedWater.LXLife3=100;
		UsedWater.LXLife4=100;
		UsedWater.LXLife5=100;
		UsedWater.ZSTime1m=0;
		UsedWater.LastChar=0xaa;
		WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar); //保存与水量相关的数据
	}
	OzoneTime=SetParameterData.OnceOzoneTime*60;InflowCloseTime=600;
}
//*********************臭氧****************************//
void OzoneControl(void)
{
//臭氧控制逻辑
	if(SetParameterData.OzoneInterval)
	{
		if(OzoneTime1m>=60)
		{
			OzoneTime1m=0;OzoneTime1h++;
			if(OzoneTime1h>=SetParameterData.OzoneInterval)//臭氧间隔时间到，需要开启臭氧
			{
				OzoneTime1h=0;OzoneTime=SetParameterData.OnceOzoneTime*60;
			}
		}
	}
	else OzoneTime=SetParameterData.OnceOzoneTime*60;//如果臭氧工作间隔为0则直接把臭氧工作时间给工作时间计时器
	if(OzoneTime){--OzoneTime;OzoneOpen;ShowState|=0x20;}//开启臭氧杀菌
	else {++OzoneTime1m;OzoneClose;ShowState&=0xdf;}
//臭氧控制逻辑
}

void W_OpenAll(void)//打开所有负载
{
	ZSPumpOpen;WashValveOpen;InflowValveOpen;OzoneOpen;
}
void W_CloseAll(void)//关闭所有负载
{
	ZSPumpClose;WashValveClose;InflowValveClose;WaterKeyClose;//OzoneClose;
}
void W_ExintConfig(void)
{
	IT0 = 1;                    //设置INT0的中断类型 (1:仅下降沿 0:上升沿和下降沿)
	EX0 = 1;                    //使能INT0中断
	
	IT1 = 1;                    //设置INT1的中断类型 (1:仅下降沿 0:上升沿和下降沿)
	EX1 = 1;  
	
  INTCLKO = EX2;              //使能INT2中断
}
//中断服务程序
void exint0(void) interrupt 0       //INT0中断入口用于TDS检测
{
	if(ZSFlag)TDS1Pluse++;
//	{
//		if(TDS1flag==0)
//		{
//			TDS1flag=1;TDS5ms1=0;TDS1Pluse=0;
//		}
//		
//	}
	else TDS1Pluse=0;
}
//中断服务程序
void exint1(void) interrupt 2       //INT1中断入口用于TDS检测
{
	if(ZSFlag)TDS2Pluse++;
//	{
//		if(TDS2flag==0)
//		{
//			TDS2flag=1;TDS5ms2=0;TDS2Pluse=0;
//		}
//		
//	}
	else TDS2Pluse=0;
   
}
void INT2_Isr(void) interrupt 10
{
	if((WaterState<5)||(QSFlag&&InflowCloseTime))FlowPulse++;//补水状态下才记流量计脉冲
}
void W_WaterYield(void)
{
	if(FlowPulse>=SetParameterData.Pluse3){SendShowFlag=1;SendShowTime=1;}//制水量达到一升时发送数据给显示板
	WaterYield_L+=FlowPulse/SetParameterData.Pluse3;
	UsedWater.ZSYield+=FlowPulse/SetParameterData.Pluse3;
	FlowPulse=FlowPulse%SetParameterData.Pluse3;
}
void Delay1ms(uint16_t dat)		//@24.000MHz
{
	unsigned char i, j;
	while(dat)
	{
		i = 24;
		j = 85;
		do
		{
			while (--j);
		} while (--i);
		dat--;
	}
}
void SoundCnt(void)
{
//--------------------------------声音
   if(SoundFlag)                     //sound_flug的奇数个数为声音数
   { ++SoundNum;                     //计时单元+1
     if((SoundFlag%2)==1)            //多次发声的发声阶段
     { if(SoundNum<150)  BUZZER_Open;
       else                          //如果时间到
       { --SoundFlag;   BUZZER_Close;
         if(SoundFlag==0)
           {SoundNum=0;}             //关闭标志，关闭发声位电流
       }
     }
     else                              //多次发声的不发声阶段
     { if(SoundNum>200)                //不发声的时间间隔
        {--SoundFlag;SoundNum=0;BUZZER_Close;}
     }
   }
	 else BUZZER_Close; 
}

