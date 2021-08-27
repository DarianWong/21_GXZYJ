#include "w_work.h"

bit ZSFlag=0,MSFlag=0,WashFlag=0,QSFlag=0;
bit TDS1flag=0,TDS2flag=0;
uint16_t TDS1Pluse=0,TDS2Pluse=0,CSTDS=0,YSTDS=0,TDSTemp=0;
uint16_t	TDS5ms1=0,TDS5ms2=0;
uint16_t FlowPulse=0,WaterYield_L=0;//�����ƣ�ˮ��,WaterYield=0
uint8_t	SoundNum=0,SoundFlag=0;
uint8_t	WashTime=0;
extern	bit Sys1sFlag,Sys100msFlag,Sys200msFLag,Sys1mFlag;
uint16_t LowWaterData=0,MiddleWaterData=0,HighWaterData=0,LowWaterDataOk=0,MiddleWaterDataOk=0,HighWaterDataOk=0,LeakData=0;
uint8_t	 WaterLevelCnt=0;
uint8_t LeakCnt=0;
bit WaterLowOk=0,WaterMiddleOk=0,WaterHighOk=0;
uint8_t	ErrorTemp=0;//���ϴ���
uint8_t	WaterState=0,SellDelayTime=0;//ˮλ״̬
bit SellState=0;//����״̬
uint8_t OzoneTime=0,OzoneTime1h=0,OzoneTime1m=0;
bit SZKeyFlag=0,SZFlag=0,TZKeyFlag=0,AutoChange=0,TestKeyFlag=0,TestFlag=0,LeakFlag=0;
bit SetPriceFlag=0,SetTimeFlag=0,SetOzoneFlag=0;
uint8_t	SetCnt=0,TestCnt=0;
uint8_t	Year=0,Month=0,Day=0,Hour=0,Minute=0,Second=0,Week=0;
//bit AlterParameter1=0,AlterParameter2=0;
uint8_t WorkStateByte=0,WorkStateLast=0;
uint8_t ZSTime1s=0,CXInterval1m=0,ZSTime1m=0;
//uint16_t ZSTime1h=0;//��ˮʱ��ʱ��
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
uint16_t InflowCloseTime=0;//��ѹ���ضϿ��ǵĽ�ˮ����ʱ�ر�ʱ��
bit InflowValveCloseFlag=0;

#ifdef	TestServer
extern	uint8_t code Domain[];
extern	uint8_t code Port[];
#else
extern	uint8_t code Domain[];  //����,��һ�ֽ�Ϊ��������http://testmanage.waterer.cn/
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
{ uint8_t		FirstChar;   //��ͷ      1B
	uint16_t	ZSTime;			//��ˮʱ��
	uint32_t	ZSYield;		//��ˮ��
	uint8_t		LXLife1;	//1����о�����ٷֱ�
	uint8_t		LXLife2;	//		1B
	uint8_t 	LXLife3;	//					1B
	uint8_t		LXLife4;	//							1B
	uint8_t 	LXLife5;	//				1B
	uint8_t		ZSTime1m;//��ˮ������
	uint8_t 	LastChar; //��β      1B
}UsedWater;

//union ActDataGu
//{
//	uint8_t DataStr[22];
//	
//}EpromDataGu;
struct 
{ uint8_t		FirstChar;   //��ͷ      1B
	uint16_t	CardPrice1;			//ˢ������
	uint16_t	CardPrice2;			//ˢ������
	uint16_t	VirtualPrice1;	//ɨ�뵥��
	uint16_t	VirtualPrice2;	//ɨ�뵥��
	uint16_t	Pluse1;					//1�ſ�������������
	uint16_t	Pluse2;					//2�ſ�������������
	uint16_t	Pluse3;					//��ˮ������������
	uint16_t	MaxMoney;				//��󵥴γ�ˮ��
	
	uint8_t		ParameterLock;	//�豸������
//	uint8_t		Confiscate;		//û��ʱ��				1B
	uint8_t 	OnceCXTime;	//���γ�ϴʱ��					1B
	uint8_t		CXInterval;	//��ϴ���							1B
	uint8_t		HaltState;//ͣ��״̬
		uint8_t 	OnceOzoneTime;//��������ʱ��				1B
		uint8_t		OzoneInterval;//�����������ʱ��		1B
	uint8_t 	LastChar;    //��β      1B
}SetParameterData;

struct
{ uint16_t  TDSVal;      //TDSֵ
  uint16_t  TDSPlus;     //TDSֵ��Ӧ������ֵ
  uint8_t  TDSper;      //ÿ��TDS��Ӧ������
}code TdsData[6]={{0,0,0},{16,537,34},{50,1487,28},{100,2761,25},{200,4531,18},{300,5928,14}};//����У׼TDS���ݱ�

uint16_t CalTDS(uint16_t PlusNum)//����������PlusNum������TDSֵ������ 
{ uint8_t ch1,ch2;
  uint16_t int1;
  for(ch1=1;ch1<6;++ch1){if(PlusNum<TdsData[ch1].TDSPlus) break;}
  if(ch1<6)
  { int1=PlusNum-TdsData[ch1-1].TDSPlus;  //�󱾶�������
    ch2=int1%TdsData[ch1].TDSper;
    if(ch2>(TdsData[ch1].TDSper/2)) ch2=1;
    else ch2=0;
    int1/=TdsData[ch1].TDSper;    //���������Ӧ��TDS
    int1+=TdsData[ch1-1].TDSVal;  //�ӱ���ǰ�ߵ�TDSֵ
  }
  else      //TDS����300
  { int1=PlusNum-TdsData[5].TDSPlus;   //��ȥǰ�ߵ�
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
		case 1://����������
			if(TestKeyFlag==0)TZKeyFlag=1;//TZKeyFlag=0;TestKeyFlag=0;
			if(AutoChange)
			{
				AutoChange=0;AutoExit=0;
				if(SetParameterData.ParameterLock==0)
				{
					if(SetCnt>0&&SetCnt<9)SetPriceFlag=1;//����ǵ��ڵ��ۣ��������֮����Ҫ����0108�豸״̬ͬ��ָ��
					else if(SetCnt>10&&SetCnt<13)SetOzoneFlag=1;
					else if(SetCnt>12&&SetCnt<20)SetTimeFlag=1;//�������ʱ��
					else {}
					switch(SetCnt)
					{
						case 0:
							
						break;
						case 1://1�ſ�ˢ������
							++SetParameterData.CardPrice1;
							if(SetParameterData.CardPrice1>250)SetParameterData.CardPrice1=1;
						break;
						case 2://1�ſ�ɨ�뵥��
							++SetParameterData.CardPrice2;
							if(SetParameterData.CardPrice2>250)SetParameterData.CardPrice2=1;
						break;
						case 3://2�ſ�ˢ������
							++SetParameterData.VirtualPrice1;
							if(SetParameterData.VirtualPrice1>250)SetParameterData.VirtualPrice1=1;
						break;
						case 4://2�ſ�ɨ�뵥��
							++SetParameterData.VirtualPrice2;
							if(SetParameterData.VirtualPrice2>250)SetParameterData.VirtualPrice2=1;
						break;
						case 5://1�ſ�������������
							SetParameterData.Pluse1+=10;
							if(SetParameterData.Pluse1>2000)SetParameterData.Pluse1=100;
							else if(SetParameterData.Pluse1<100)SetParameterData.Pluse1=540;
							else{}
						break;
						case 6://2�ſ�������������
							SetParameterData.Pluse2+=10;
							if(SetParameterData.Pluse2>2000)SetParameterData.Pluse2=100;
							else if(SetParameterData.Pluse2<100)SetParameterData.Pluse2=540;
							else{}
						break;
						case 7://�ܾ�ˮ������������
							SetParameterData.Pluse3+=10;
							if(SetParameterData.Pluse3>2000)SetParameterData.Pluse3=100;
							else if(SetParameterData.Pluse3<100)SetParameterData.Pluse3=540;
							else{}
						break;
						case 8://��󵥴γ�ˮ��
							SetParameterData.MaxMoney+=10;
							if(SetParameterData.MaxMoney>9999||SetParameterData.MaxMoney<50)SetParameterData.MaxMoney=500;
						break;
						case 9://��ϴʱ��
							++SetParameterData.OnceCXTime;
							if(SetParameterData.OnceCXTime>240||SetParameterData.OnceCXTime<5)SetParameterData.OnceCXTime=5;					
						break;
						case 10://��ϴ���
							++SetParameterData.CXInterval;
							if(SetParameterData.CXInterval>240||SetParameterData.CXInterval<30)SetParameterData.CXInterval=30;
						break;
						case 11://��������ʱ��
							++SetParameterData.OnceOzoneTime;
							if(SetParameterData.OnceOzoneTime>24)SetParameterData.OnceOzoneTime=0;
						break;
						case 12://�����������
							++SetParameterData.OzoneInterval;
							if(SetParameterData.OzoneInterval>24)SetParameterData.OzoneInterval=0;
						break;
						case 13://��
							++SysTime.Year;
							if(SysTime.Year>99||SysTime.Year<20)SysTime.Year=20;
						break;
						case 14://��
							++SysTime.Month;
							if(SysTime.Month>12)SysTime.Month=1;
						break;
						case 15://��
							++SysTime.Day;
							if(SysTime.Day>31)SysTime.Day=1;
						break;
						case 16://ʱ
							++SysTime.Hour;
							if(SysTime.Hour>23)SysTime.Hour=0;
						break;
						case 17://��
							++SysTime.Minute;
							if(SysTime.Minute>59)SysTime.Minute=0;
						break;
						case 18://��
							++SysTime.Second;
							if(SysTime.Second>59)SysTime.Second=0;
						break;
						case 19://����
							++SysTime.Week;
							if(SysTime.Week>7)SysTime.Week=1;
						break;
						default:break;
					}
				
				}
			}	
		break;
		case 2://���ü�����
			if(TestKeyFlag==0)SZKeyFlag=1;//TestKeyFlag=0;SZKeyFlag=0;
		break;
		case 3://������ͬʱ����
			TestKeyFlag=1;TZKeyFlag=0;SZKeyFlag=0;
		break;
		default://������
//		Uart1Send(1);
			if(SZKeyFlag)
			{
				SZKeyFlag=0;
				if(TestFlag)//while(1);
				{
					if((TestCnt==0)&&SaveSetDataFlag)//�ڲ���״̬����һģʽ�³������ü�3�������ˮ��
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
					if(SaveSetDataFlag)//�������ü�3��ֱ�ӱ����˳�
					{
						SaveSetDataFlag=0;
						SetCnt=0;SZFlag=0;//�˴���Ҫ���ӱ�־λ�����ж�ˢ�����Ƿ��յ������յ��򱣴�EEPROM��δ�յ��򲻱��档
//						AlterParameter1=1;AlterParameter2=1;//�޸��豸���������лظ�������Ϊ�ɹ�
						//��Ҫ�����޸Ĳ���ָ���ˢ���壬2001ָ��
//						WriteEeprom(SetParameterAddress,sizeof(SetParameterData),SetParameterData);
						if(SetPriceFlag||SetTimeFlag)//����ʱ����ߵ���֮��Ĳ�����Ҫ�˳�֮����Ҫ���͸���ˮ��
						{
//								SetTimeFlag=0;
							W_Joint2001Data();
							W_Send2001Data_1();W_Send2001Data_2();
						}
						if(SetPriceFlag)
						{
							SetPriceFlag=0;
							WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //����SaveParameterFlag=0;WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);
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
						if(SetCnt>19){SetCnt=0;SZFlag=0;SetPriceFlag=0;SetTimeFlag=0;SetOzoneFlag=0;PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);}//�Զ��˳����������
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
					if(SetCnt>0&&SetCnt<9)SetPriceFlag=1;//����ǵ��ڵ��ۣ��������֮����Ҫ����0108�豸״̬ͬ��ָ��
					else if(SetCnt>10&&SetCnt<13)SetOzoneFlag=1;
					else if(SetCnt>12&&SetCnt<20)SetTimeFlag=1;//�������ʱ��
					else {}
					switch(SetCnt)
					{
						case 0:
							
						break;
						case 1://1�ſ�ˢ������
							++SetParameterData.CardPrice1;
							if(SetParameterData.CardPrice1>250)SetParameterData.CardPrice1=1;
						break;
						case 2://2�ſ�ˢ������
							++SetParameterData.CardPrice2;
							if(SetParameterData.CardPrice2>250)SetParameterData.CardPrice2=1;
						break;
						case 3://1�ſ�ɨ�뵥��
							++SetParameterData.VirtualPrice1;
							if(SetParameterData.VirtualPrice1>250)SetParameterData.VirtualPrice1=1;
						break;
						case 4://2�ſ�ɨ�뵥��
							++SetParameterData.VirtualPrice2;
							if(SetParameterData.VirtualPrice2>250)SetParameterData.VirtualPrice2=1;
						break;
						case 5://1�ſ�������������
							SetParameterData.Pluse1+=1;
							if(SetParameterData.Pluse1>2000)SetParameterData.Pluse1=100;
							else if(SetParameterData.Pluse1<100)SetParameterData.Pluse1=540;
							else{}
						break;
						case 6://2�ſ�������������
							SetParameterData.Pluse2+=1;
							if(SetParameterData.Pluse2>2000)SetParameterData.Pluse2=100;
							else if(SetParameterData.Pluse2<100)SetParameterData.Pluse2=540;
							else{}
						break;
						case 7://�ܾ�ˮ������������
							SetParameterData.Pluse3+=10;
							if(SetParameterData.Pluse3>2000)SetParameterData.Pluse3=100;
							else if(SetParameterData.Pluse3<100)SetParameterData.Pluse3=540;
							else{}
						break;
						case 8://��󵥴γ�ˮ��
							SetParameterData.MaxMoney+=10;
							if(SetParameterData.MaxMoney>9999||SetParameterData.MaxMoney<50)SetParameterData.MaxMoney=500;
						break;
						case 9://��ϴʱ��
							++SetParameterData.OnceCXTime;
							if(SetParameterData.OnceCXTime>240||SetParameterData.OnceCXTime<5)SetParameterData.OnceCXTime=5;					
						break;
						case 10://��ϴ���
							++SetParameterData.CXInterval;
							if(SetParameterData.CXInterval>240||SetParameterData.CXInterval<30)SetParameterData.CXInterval=30;
						break;
						case 11://��������ʱ��
							++SetParameterData.OnceOzoneTime;
							if(SetParameterData.OnceOzoneTime>24)SetParameterData.OnceOzoneTime=0;
						break;
						case 12://�����������
							++SetParameterData.OzoneInterval;
							if(SetParameterData.OzoneInterval>24)SetParameterData.OzoneInterval=0;
						break;
							case 13://��
								++SysTime.Year;
								if(SysTime.Year>99||SysTime.Year<20)SysTime.Year=20;
							break;
							case 14://��
								++SysTime.Month;
								if(SysTime.Month>12)SysTime.Month=1;
							break;
							case 15://��
								++SysTime.Day;
								if(SysTime.Day>31)SysTime.Day=1;
							break;
							case 16://ʱ
								++SysTime.Hour;
								if(SysTime.Hour>23)SysTime.Hour=0;
							break;
							case 17://��
								++SysTime.Minute;
								if(SysTime.Minute>59)SysTime.Minute=0;
							break;
							case 18://��
								++SysTime.Second;
								if(SysTime.Second>59)SysTime.Second=0;
							break;
							case 19://����
								++SysTime.Week;
								if(SysTime.Week>7)SysTime.Week=1;
							break;
						default:break;
					}
				}
				SoundFlag=1;AutoExit=0;
			}
			if(TestKeyFlag)TestKeyFlag=0;
			if(AutoExit>10)//�Զ��˳�ʱ�䵽���������ò���
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
//					WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //����
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
		if(LeakFlag||(SetParameterData.HaltState==1))//©ˮ������о��������ͣ��
		{
			if(LeakFlag)ErrorTemp|=0x08;else ErrorTemp|=0x02;W_CloseAll();
			ZSFlag=0;SellState=0;
		}
		else
		{
			ErrorTemp&=0xf7;
			if(DYKey)//�����ѹ���ضϿ�
			{
				ZSPumpClose;WashValveClose;WashTime=0;//��ѹ���ضϿ����ر���ˮ�úͳ�ϴ��
//				if(ZSFlag)WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);
				ZSFlag=0;
				if(GYKey)MSFlag=1;else MSFlag=0;
				ErrorTemp|=0x01;
				if(QSFlag==0){QSFlag=1;InflowCloseTime=600;}
				if(InflowCloseTime)InflowCloseTime--;
				else {InflowValveClose;InflowValveCloseFlag=1;}
			}
			else //��ѹ���رպ�
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
						if(UsedWater.ZSTime1m%6==0){WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);} //SoundFlag=1;��ˮ6���ӱ�����ˮ����ص�����
						if(UsedWater.ZSTime1m>=60)
						{
							UsedWater.ZSTime1m=0;UsedWater.ZSTime++;ZSTimeFlag=1;//����ͬ����ˮʱ��ָ��յ��ظ�������
						}
						if(CXInterval1m>=SetParameterData.CXInterval)//��ϴ���ʱ�䵽���г�ϴ
						{
							CXInterval1m=0;WashTime=SetParameterData.OnceCXTime;//SoundFlag=9;
						}
					}
				}
				if(GYKey)//��ѹ���ضϿ�����ʾˮ��
				{
					if(ZSFlag)
					{
						ZSFlag=0;WashTime=15;//WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);
					}
					if(WashTime==0)ZSPumpClose;//������ڳ�ϴ״̬��رճ�ϴ��
				}
				else
				{
					ZSFlag=1;ZSPumpOpen;
				}
			}
			
			if(WaterLowOk)//�����Һλ
			{
				if(WaterMiddleOk)
				{
					SellState=1;SellDelayTime=0;ErrorTemp&=0xfb;
					if(WaterHighOk)//ˮ��״̬
					{
						WaterState=5;WaterLevelState=3;InflowValveClose;MSFlag=1;
					}
					else//ˮλ������Һλ�͸�Һλ֮��
					{
						WaterLevelState=2;
						if(MSFlag){InflowValveClose;WaterState=6;}
						else {if(InflowValveCloseFlag==0)InflowValveOpen;WaterState=4;}
					}
				}
				else
				{
					MSFlag=0;
					if(WaterHighOk)//ˮλ����
					{
						InflowValveClose;ErrorTemp|=0x04;SellState=0;
					}
					else//ˮλ���ڵ�Һλ����Һλ֮��
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
				if(WaterMiddleOk||WaterHighOk)//��Һλδ��⵽�����Ǽ�⵽��Һλ���Һλ�ˣ����ʾˮλ����
				{
					InflowValveClose;ErrorTemp|=0x04;
				}
				else
				{
					if(InflowValveCloseFlag==0)InflowValveOpen;WaterState=2;WaterLevelState=0;ErrorTemp&=0xfb;SellDelayTime=180;//��Һλ����ʱP2״̬
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
//		ShowState&=0xdf;//ɱ��ͼ�겻��ʾ
		if(YSTDS&&CSTDS)
		{
			if(YSTDS>=CSTDS)
			{
				if((((YSTDS-CSTDS)*100/YSTDS)>=85)&&SellState)ShowState|=0x02;//�����ʴ���85%��ʾ����
				else ShowState&=0xfd;
			}
			else 
			{
				if((((CSTDS-YSTDS)*100/CSTDS)>=85)&&SellState)ShowState|=0x02;//�����ʴ���85%��ʾ����
				else ShowState&=0xfd;
			}
		}
		else {if(SellState)ShowState|=0x02;else ShowState&=0xfd;}//ֻҪ��һ��TDSΪ�����ʾ����
		if(WorkStateLast!=WorkStateByte)//״̬�ı䷢��״̬����������
		{
			WorkStateLast=WorkStateByte;StateFlag=1;
		}
		if(SZFlag)AutoExit++;//����״̬���ް�������10���Զ������˳�
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
void W_CheckWaterLevel(void)//ÿ���Ӽ��ˮλһ�Σ����һ�ν���10��ADת������ƽ��ֵ
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
void W_EepromData(void)//�ϵ���EEPROM�����Ƿ�����
{
	PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //��ϵͳ����
	if(SetParameterData.FirstChar!=0x55||SetParameterData.LastChar!=0xaa)//����ͷ��β����ȷ��ѳ�ʼ���ݱ�������
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
		WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //����
		
		memcpy(IportBuf,Domain,Domain[0]+1);     //����
    memcpy(&IportBuf[22],Port,5);IportBuf[27]=0x5a;//��28���ֽ��Ǳ�־�룬�����־�벻����0x5a�Ļ�����Ҫ��������IP��ַ
    WriteEeprom(SysIpPort,28,IportBuf); //����ȷ��IP��ַд�뵽EEPROM
		SoundFlag=3;
	}
	else {PReadEeprom(SysIpPort,28,IportBuf);SoundFlag=1;}//
	PReadEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar); //��ϵͳ����
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
		WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar); //������ˮ����ص�����
	}
	OzoneTime=SetParameterData.OnceOzoneTime*60;InflowCloseTime=600;
}
//*********************����****************************//
void OzoneControl(void)
{
//���������߼�
	if(SetParameterData.OzoneInterval)
	{
		if(OzoneTime1m>=60)
		{
			OzoneTime1m=0;OzoneTime1h++;
			if(OzoneTime1h>=SetParameterData.OzoneInterval)//�������ʱ�䵽����Ҫ��������
			{
				OzoneTime1h=0;OzoneTime=SetParameterData.OnceOzoneTime*60;
			}
		}
	}
	else OzoneTime=SetParameterData.OnceOzoneTime*60;//��������������Ϊ0��ֱ�Ӱѳ�������ʱ�������ʱ���ʱ��
	if(OzoneTime){--OzoneTime;OzoneOpen;ShowState|=0x20;}//��������ɱ��
	else {++OzoneTime1m;OzoneClose;ShowState&=0xdf;}
//���������߼�
}

void W_OpenAll(void)//�����и���
{
	ZSPumpOpen;WashValveOpen;InflowValveOpen;OzoneOpen;
}
void W_CloseAll(void)//�ر����и���
{
	ZSPumpClose;WashValveClose;InflowValveClose;WaterKeyClose;//OzoneClose;
}
void W_ExintConfig(void)
{
	IT0 = 1;                    //����INT0���ж����� (1:���½��� 0:�����غ��½���)
	EX0 = 1;                    //ʹ��INT0�ж�
	
	IT1 = 1;                    //����INT1���ж����� (1:���½��� 0:�����غ��½���)
	EX1 = 1;  
	
  INTCLKO = EX2;              //ʹ��INT2�ж�
}
//�жϷ������
void exint0(void) interrupt 0       //INT0�ж��������TDS���
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
//�жϷ������
void exint1(void) interrupt 2       //INT1�ж��������TDS���
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
	if((WaterState<5)||(QSFlag&&InflowCloseTime))FlowPulse++;//��ˮ״̬�²ż�����������
}
void W_WaterYield(void)
{
	if(FlowPulse>=SetParameterData.Pluse3){SendShowFlag=1;SendShowTime=1;}//��ˮ���ﵽһ��ʱ�������ݸ���ʾ��
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
//--------------------------------����
   if(SoundFlag)                     //sound_flug����������Ϊ������
   { ++SoundNum;                     //��ʱ��Ԫ+1
     if((SoundFlag%2)==1)            //��η����ķ����׶�
     { if(SoundNum<150)  BUZZER_Open;
       else                          //���ʱ�䵽
       { --SoundFlag;   BUZZER_Close;
         if(SoundFlag==0)
           {SoundNum=0;}             //�رձ�־���رշ���λ����
       }
     }
     else                              //��η����Ĳ������׶�
     { if(SoundNum>200)                //��������ʱ����
        {--SoundFlag;SoundNum=0;BUZZER_Close;}
     }
   }
	 else BUZZER_Close; 
}

