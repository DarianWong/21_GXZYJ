#include "w_uart.h"

bit busy1=0,busy2=0,busy3=0,busy4=0;

uint8_t S1RecBuf[50]={0};
uint8_t S2RecBuf[110]={0};
uint8_t UartDataDisBuf[50]={0};
uint8_t UartSendDataBuf[50]={0};
uint8_t S2RecDataBuf[10][50]={0};
uint8_t S3RecDataBuf[8][50]={0};
uint8_t S4RecDataBuf[8][50]={0};

//uint8_t S3RecBuf[50]={0};
//uint8_t S4RecBuf[50]={0};
bit S1ReceiveFlag=0,S2ReceiveFlag=0,S3ReceiveFlag=0,S4ReceiveFlag=0;
bit S1ReceiveFinish=0,S2ReceiveFinish=0,S3ReceiveFinish=0,S4ReceiveFinish=0;
//bit S1ReceiveLegth=0,S2ReceiveLegth=0,S3ReceiveLegth=0,S4ReceiveLegth=0;
uint8_t S1ReceiveLength=0,S2ReceiveLength=0,S3ReceiveLength=0,S4ReceiveLength=0;
uint8_t S1PacketNum=0,S2PacketNum=0,S3PacketNum=0,S4PacketNum=0;//数据包个数
uint8_t	S1DataLength=0,S2DataLength=0,S3DataLength=0,S4DataLength=0;
uint8_t	S1Receive1ms=0,S2Receive1ms=0,S3Receive1ms=0,S4Receive1ms=0;
uint8_t S2Begin=0,S3Begin=0,S4Begin=0;
uint16_t ZhenNum=0;

//extern bit AlterParameter1,AlterParameter2;

uint8_t code Cdatas[4] _at_ 0x7ff5;
//#define TestServer
#ifdef	TestServer
//uint8_t code Domain[]={"\x015testmanage.waterer.cn"};  //域名,第一字节为域名长度http://testmanage.waterer.cn/
uint8_t code Domain[]={"\x015testmanage.wateree.cn"};
uint8_t code Port[]={"09999"};
#else
uint8_t code Domain[]={"\x00ds0.waterer.cn"};  //域名,第一字节为域名长度http://testmanage.waterer.cn/
uint8_t code Port[]={"09999"};
#endif


bit PowerDown=0;//模块断电标志
uint8_t PowerDown1s=0,SaveLiData1s=0;//断电计时器
bit NetWork=0,RorS_Flag=0;
uint8_t CommVal=0,NetOverTime=0;
bit YuanipFlag=0;
uint8_t NewIPTime=0;
uint8_t IportBuf[30]={0};
bit PowerOn=0,ShutDown=0;//开关机指令
bit SaveParameterFlag=0;
bit StateFlag=0,HeartFlag=0,TimingFlag=0,ZSTimeFlag=0,SaveTimeFlag=0;//时间同步指令标志，状态包标志，心跳包标志,
extern uint8_t SoundFlag;
uint8_t HeartCnt=0,NetProce=0,AutoReconnection=0;
uint8_t SimStr[21];
uint8_t S2RecCount=0,S2TransLen=0;//,S2DataLen=0
uint8_t StateTime=0,HeartTime=0,TimingTime=0;
extern uint8_t WorkStateByte,WorkStateLast,ShowState,ErrorTemp,WaterLevelState;
extern uint16_t	CSTDS,YSTDS,WaterYield_L;
uint8_t SendDataInterval=0;
bit ZhenNumFlag=0;
//extern uint8_t	Year,Month,Day,Hour,Minute,Second,Week;
uint8_t OffLineData1=0,OffLineData2=0,OffLineData=0;
bit ForwardFlag=0,BroadcastFlag=0;//转发标志,广播标志
extern bit SendShowFlag;
struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Week;
	
}SysTime;
struct 
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

//extern struct 
//{ uint8_t		FirstChar;   //字头      1B
//	uint16_t	CardPrice1;			//刷卡单价
//	uint16_t	CardPrice2;			//刷卡单价
//	uint16_t	VirtualPrice1;	//扫码单价
//	uint16_t	VirtualPrice2;	//扫码单价
//	uint16_t	Pluse1;					//1号口流量计脉冲数
//	uint16_t	Pluse2;					//2号口流量计脉冲数
//	uint16_t	MaxMoney;				//最大单次出水量
//	
//	uint8_t		ParameterLock;	//设备参数锁
////	uint8_t		Confiscate;		//没收时间				1B
//	uint8_t 	OnceCXTime;	//单次冲洗时间					1B
//	uint8_t		CXInterval;	//冲洗间隔							1B
//	uint8_t		HaltState;//停机状态
//	uint8_t 	OnceOzoneTime;//臭氧工作时间				1B
//	uint8_t		OzoneInterval;//臭氧工作间隔时间		1B
//	uint8_t 	LastChar;    //字尾      1B
//}SetParameterData;
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

void FrameHead()//固定帧头
{
	UartSendDataBuf[0]=0x7e;UartSendDataBuf[3]=0x20;UartSendDataBuf[4]=SheClass;UartSendDataBuf[9]=1;
				//头									协议版本						设备类型								出水口
}
void SendDataJoint(void)
{
	SendDataInterval=DataInterval;//主动发送的数据都需要有2s的间隔时间
	memset(UartSendDataBuf,0,50);//初始化发送缓冲区
	FrameHead();
	if(TimingFlag)//每小时校时一次
	{
		TimingFlag=0;
		UartSendDataBuf[2]=14;UartSendDataBuf[10]=3;UartSendDataBuf[11]=1;//请求北京时间
		TimingTime=TimeInterval;
	}
	else if(ZSTimeFlag)
	{
		ZSTimeFlag=0;PushZhenNum(UartSendDataBuf);ZhenNumFlag=1;UartSendDataBuf[3]=0x21;UartSendDataBuf[2]=18;UartSendDataBuf[10]=7;Int_Array(UsedWater.ZSTime,&UartSendDataBuf[15]);//Int_Array(5,&UartSendDataBuf[15]);////放入制水时间
	}
	else if(StateFlag)
	{
		StateFlag=0;
		UartSendDataBuf[2]=30;UartSendDataBuf[10]=5;
		UartSendDataBuf[17]=SetParameterData.OnceCXTime;//UartSendDataBuf[20]=Temperature;//臭氧工作时间，机箱温度
		UartSendDataBuf[25]=WorkStateByte;//工作状态字节
//		UartSendDataBuf[7]=0x10;
		UartSendDataBuf[28]=CommVal;//信号强度
		Int_Array(SetParameterData.CardPrice1,&UartSendDataBuf[18]);//刷卡单价
//		Int_Array(100,&UartSendDataBuf[21]);
//		Int_Array(10,&UartSendDataBuf[23]);
		if(CSTDS>YSTDS)
		{
			Int_Array(CSTDS,&UartSendDataBuf[21]);
			Int_Array(YSTDS,&UartSendDataBuf[23]);
		}
		else
		{
			Int_Array(YSTDS,&UartSendDataBuf[21]);
			Int_Array(CSTDS,&UartSendDataBuf[23]);
		}
		Int_Array(WaterYield_L,&UartSendDataBuf[26]);//制水量
		StateTime=StateInterval;
	}
	else if(HeartFlag)
	{
		UartSendDataBuf[2]=16;UartSendDataBuf[10]=1;HeartFlag=0;HeartTime=HeartInterval;//长度，数据类型
	}
	else return;
	memcpy(&UartSendDataBuf[11],Cdatas,4);//放入设备号
	if(ZhenNumFlag==0)PushZhenNum(UartSendDataBuf);//放入帧序
	ZhenNumFlag=0;
	PushCRC(UartSendDataBuf);//放入CRC
	SendFixedLength(2,UartSendDataBuf);
	if(HeartCnt>=5)
	{
		HeartCnt=0;NetProce=0;PowerDown=0;M800Power=0;NetWork=0;
		W_Joint2001Data();W_Send2001Data_1();W_Send2001Data_2();//发送给两个出水口提示设备离线
	}//断网需要发送数据给显示板
	else HeartCnt++;
}
void Uart1DataDispose()//与显示板通讯
{
	if(S1ReceiveFinish)
	{
		S1ReceiveFinish=0;
		if(CompCRC(S1RecBuf)==0)
		{
			if(S1RecBuf[0]==0x7e&&S1RecBuf[9]==0x20&&S1RecBuf[10]==0x01){SendShowFlag=0;}//SoundFlag=5;
		}
		memset(S1RecBuf,0,50);
	}
}
//转网指令暂未完成
void Uart2DataDispose()//与服务器通讯
{
	uint8_t ch1=0,ch2=0;
	uint16_t int1=0,port1=0;
	uint32_t long1=0;
//	struct SetParameter *pSetParameter;
//	Uart1Send(1);
	if(S2ReceiveFinish)
	{
		S2ReceiveFinish=0;//HeartTime=HeartInterval;//Uart2Send(2);
		for(ch2=0;ch2<8;ch2++)
		{	//Uart1Send(3);
			if(S2RecDataBuf[ch2][0]==0x7e)
			{
				memcpy(UartDataDisBuf,&S2RecDataBuf[ch2][0],50);
				memset(&S2RecDataBuf[ch2][0],0,50);
//				SendFixedLength(1,UartDataDisBuf);
				if(CompCRC(UartDataDisBuf))continue;
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3]))//设备号相同
//包含设备号的指令有登陆回复指令0202，转网指令0301，获取离线数据指令0302，控制指令0304，获取状态指令0305，冲洗时间设置指令0306，臭氧时间设置指令0307，设备参数锁指令0309
//最大单次出水量指令0310，脉冲数设置指令0311，禁止打水指令0313，单价设置指令0315，后台余额指令0316
				{
					if(UartDataDisBuf[9]==2||UartDataDisBuf[9]==0x03)HeartCnt=0;
					switch(UartDataDisBuf[9])
					{
						case 2://收到登陆回复包需要转发给出水口，和显示板
							if(UartDataDisBuf[10]==7)//0207
							{
								UsedWater.ZSTime=0;
									memcpy(&UsedWater.LXLife1,&UartDataDisBuf[15],5);
//								UsedWater.LXLife1=UartDataDisBuf[15];
//								UsedWater.LXLife2=UartDataDisBuf[16];
//								UsedWater.LXLife3=UartDataDisBuf[17];
//								UsedWater.LXLife4=UartDataDisBuf[18];
//								UsedWater.LXLife5=UartDataDisBuf[19];
//								Uart1Send(UsedWater.LXLife1);Uart1Send(UsedWater.LXLife2);Uart1Send(UsedWater.LXLife3);Uart1Send(UsedWater.LXLife4);Uart1Send(UsedWater.LXLife5);
								WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);// SoundFlag=5;//保存与水量相关的数据
								W_Joint2001Data();W_Send2001Data_3();
//								case 7://0207
//									
//								break;
							}
						break;
						case 3:
							memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//将数据拷贝到串口2的发送缓冲区，准备发送给服务器
							if((UartDataDisBuf[7]&0xf0)==0)BroadcastFlag=1;//如果是广播指令，则执行完后需要给服务器回复
							switch(UartDataDisBuf[10])
							{
								case 1://0301转网指令需要重新配网
									NetOverTime=0;//服务器有回复数据则把掉线计时器清零
									if(UartDataDisBuf[21]>0)   //有域名
									{ ch1=UartDataDisBuf[21];	//域名长度
										memcpy(&IportBuf[1],&UartDataDisBuf[22],ch1);  //保存域名
										IportBuf[0]=ch1;    //域名长度
										YuanipFlag=1;       //更新域名标志
										NewIPTime=0;       //上网计数器
									}
									else if(UartDataDisBuf[15]|UartDataDisBuf[16]|UartDataDisBuf[17]|UartDataDisBuf[18])
									{
										IportBuf[0]=15;
										IportBuf[1]=UartDataDisBuf[15]/100+'0';//百位
										ch1=UartDataDisBuf[15]%100;
										IportBuf[2]=ch1/10+'0';IportBuf[3]=ch1%10+'0';//十位，个位
										IportBuf[4]='.';
										//ip2------------------------
										IportBuf[5]=UartDataDisBuf[16]/100+'0';
										ch1=UartDataDisBuf[16]%100;
										IportBuf[6]=ch1/10+'0';IportBuf[7]=ch1%10+'0';
										IportBuf[8]='.';
										//ip3------------------------
										IportBuf[9]=UartDataDisBuf[17]/100+'0';
										ch1=UartDataDisBuf[17]%100;
										IportBuf[10]=ch1/10+'0';IportBuf[11]=ch1%10+'0';
										IportBuf[12]='.';
										//ip4------------------------
										IportBuf[13]=UartDataDisBuf[18]/100+'0';
										ch1=UartDataDisBuf[18]%100;
										IportBuf[14]=ch1/10+'0';IportBuf[15]=ch1%10+'0';
										YuanipFlag=1;
										NewIPTime=0;       //上网计数器
									}
									else continue;
									
									//端口port------------------
									port1=UartDataDisBuf[19];port1*=0x100;port1+=UartDataDisBuf[20];
									IportBuf[22]=(uint8_t)(port1/10000)+'0';
									port1%=10000;
									ch1=port1/100;
									IportBuf[23]=ch1/10+'0';IportBuf[24]=ch1%10+'0';
									ch1=port1%100;
									IportBuf[25]=ch1/10+'0';IportBuf[26]=ch1%10+'0';
									
									NetProce=0;PowerDown=0;M800Power=0;NetWork=0;       //给模块断电重新上网NetWork=0;
//									UartDataDisBuf[9]=4;PushCRC(UartDataDisBuf);
//									memcpy(UartSendDataBuf,UartDataDisBuf,17);
//									SendFixedLength2(UartSendDataBuf);
								break;
								case 5://0305，主板直接回复状态包给服务器
									StateFlag=1;return;
								break;
								case 6://0306，冲洗设置，主板直接执行并保存，然后回复服务器
									if(UartDataDisBuf[15]>240||UartDataDisBuf[15]<5)SetParameterData.OnceCXTime=10;
									else SetParameterData.OnceCXTime=UartDataDisBuf[15];
									if(UartDataDisBuf[16]>240||UartDataDisBuf[16]<30)SetParameterData.CXInterval=60;
									else SetParameterData.CXInterval=UartDataDisBuf[16];
									SaveParameterFlag=1;SoundFlag=1;
								break;
//								case 7://0307，臭氧设置，主板直接执行并保存，然后回复服务器
//									if(UartDataDisBuf[15]>120||UartDataDisBuf[15]<1)SetParameterData.OnceOzoneTime=10;
//									else SetParameterData.OnceOzoneTime=UartDataDisBuf[15];
//									if(UartDataDisBuf[16]>240||UartDataDisBuf[16]<30)SetParameterData.OzoneInterval=60;
//									else SetParameterData.OzoneInterval=UartDataDisBuf[16];
//									SaveParameterFlag=1;SoundFlag=1;
//								break;
								case 9://0309设备参数锁，执行并回复
									SetParameterData.ParameterLock=UartDataDisBuf[15];SaveParameterFlag=1;
								break;
								case 0x17://0317设备停机指令
									if(UartDataDisBuf[15]==1)//表示滤芯寿命到期了，需要停机
									{
										SetParameterData.HaltState=1;ErrorTemp|=0x02;;//设备滤芯寿命到期停机故障状态为E82Uart2Send(SetParameterData.HaltState)
									}
									else {SetParameterData.HaltState=0;ErrorTemp&=0xfd;}//Uart2Send(SetParameterData.HaltState);
									SaveParameterFlag=1;ZSTimeFlag=1;SoundFlag=1;
//									WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);
//									PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //读系统数据
								break;
								case 2://获取离线数据指令0302
									ForwardFlag=1;
								break;
								case 4://开关机指令0304
									ForwardFlag=1;
									if(UartDataDisBuf[46]&0x04)//如果是开关机指令
									{
										if(UartDataDisBuf[37]==0x01){SendFixedLength(3,UartSendDataBuf);SendFixedLength(4,UartSendDataBuf);if(ShutDown)while(1);}//开机指令PowerOn=1;WorkStateByte|=0x01;
										if(UartDataDisBuf[37]==0x02){ShutDown=1;}//关机指令
										SoundFlag=1;
									}
								break;
								case 0x10://修改最大单次出水量
									ForwardFlag=1;//修改最大单次出水量
//									SetParameterData.MaxMoney=(UartDataDisBuf[15]&0x7f)*0x100+UartDataDisBuf[16];
//									SetParameterData.MaxMoney=Array_Int(&UartDataDisBuf[15]);
									if(UartDataDisBuf[16]&0x80)
									{
										SetParameterData.MaxMoney=(UartDataDisBuf[16]%0x80)*0x100+UartDataDisBuf[15];
									}
									else SetParameterData.MaxMoney=UartDataDisBuf[15]*100;
									if(SetParameterData.MaxMoney>9999||SetParameterData.MaxMoney<50)SetParameterData.MaxMoney=500;
									SoundFlag=1;SaveParameterFlag=1;
								break;
								case 0x11://0311
									ForwardFlag=1;
									if(UartDataDisBuf[7]&0x10||BroadcastFlag)//一号出水口指令
									{
										SetParameterData.Pluse1=Array_Int(&UartDataDisBuf[15]);SaveParameterFlag=1;
										if(SetParameterData.Pluse1>9999||SetParameterData.Pluse1<100)SetParameterData.Pluse1=450;
									}
									if(UartDataDisBuf[7]&0x20||BroadcastFlag)//一号出水口指令
									{
										SetParameterData.Pluse2=Array_Int(&UartDataDisBuf[15]);SaveParameterFlag=1;
										if(SetParameterData.Pluse2>9999||SetParameterData.Pluse2<100)SetParameterData.Pluse2=450;
									}
								break;
								case 0x13://0313
									ForwardFlag=1;
								break;
								case 0x15://0315
									ForwardFlag=1;
									if(UartDataDisBuf[7]&0x10||BroadcastFlag)//一号出水口指令
									{
										SetParameterData.CardPrice1=Array_Int(&UartDataDisBuf[15]);
										if(SetParameterData.CardPrice1==0)SetParameterData.CardPrice1=20;
										SetParameterData.VirtualPrice1=Array_Int(&UartDataDisBuf[17]);
										if(SetParameterData.VirtualPrice1==0)SetParameterData.VirtualPrice1=20;
										SaveParameterFlag=1;
									}
									if(UartDataDisBuf[7]&0x20||BroadcastFlag)//一号出水口指令
									{
										SetParameterData.CardPrice2=Array_Int(&UartDataDisBuf[15]);
										if(SetParameterData.CardPrice2==0)SetParameterData.CardPrice2=20;
										SetParameterData.VirtualPrice2=Array_Int(&UartDataDisBuf[17]);
										if(SetParameterData.VirtualPrice2==0)SetParameterData.VirtualPrice2=20;
										SaveParameterFlag=1;
									}
								break;
								case 0x16://0316
									ForwardFlag=1;
								break;
								case 0x18://0318
									ForwardFlag=1;
								break;
								default:
//								if(UartDataDisBuf[10]==2||UartDataDisBuf[10]==4||(UartDataDisBuf[10]>=0x10&&UartDataDisBuf[10]<=0x16))//如果是需要转发给出水口的数据
//								{
//									memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//将数据拷贝到串口2的发送缓冲区，准备发送给服务器
//									switch(UartDataDisBuf[10])
//									{
//										case 4:
//										break;
//										case 0x10:
//										break;
//										case 0x11:
//										break;
//										case 0x15:
//										break;
//										default:break;
//									}
//									if(UartDataDisBuf[7]&0x10||(UartDataDisBuf[7]&0xf0)==0)SendFixedLength(3,UartSendDataBuf);//一号出水口指令
//									if(UartDataDisBuf[7]&0x20||(UartDataDisBuf[7]&0xf0)==0)SendFixedLength(4,UartSendDataBuf);//一号出水口指令
//								}
								break;
							}
							if(ForwardFlag)//转发给出水口进行处理
							{
								ForwardFlag=0;
								if(UartDataDisBuf[7]&0x10||BroadcastFlag)SendFixedLength(3,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);}//一号出水口指令
								if(UartDataDisBuf[7]&0x20||BroadcastFlag)SendFixedLength(4,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);}//2号出水口指令
							}
							if(BroadcastFlag)
							{
								BroadcastFlag=0;UartSendDataBuf[9]=4;if(UartSendDataBuf[10]!=4)UartSendDataBuf[2]=16;//修改回复数据类型及数据长度,0304的回复包直接原数据返回
								SendDataPackage(2,UartSendDataBuf);//回复数据给服务器
							}
						break;
						default:break;
					}
				}
				else//心跳回复包0201，校时回复0203，控制回复0204，状态回复0205，离线数据回复1204，制水同步回复0207
				{
					switch(UartDataDisBuf[9])
					{
						case 2:HeartCnt=0;
							switch(UartDataDisBuf[10])
							{
								case 1://0201
									HeartCnt=0;
								break;
								case 3://0203
									ForwardFlag=1;
									memcpy(&SysTime.Year,&UartDataDisBuf[12],6);
									if(UartDataDisBuf[18]>7||UartDataDisBuf[18]<=1)SysTime.Week=7;
									else SysTime.Week=UartDataDisBuf[18]-1;
//									Year=UartDataDisBuf[12];Month=UartDataDisBuf[13];Day=UartDataDisBuf[14];Hour=UartDataDisBuf[15];Minute=UartDataDisBuf[16];Second=UartDataDisBuf[17];Week=UartDataDisBuf[18];
									if(SaveTimeFlag==0)
									{
										SaveTimeFlag=1;//每次上电连上网保存一次时间
										time_init(SysTime.Year,SysTime.Month,SysTime.Day,SysTime.Hour,SysTime.Minute,SysTime.Second,SysTime.Week);
										W_Joint2001Data();W_Send2001Data_3();
									}SoundFlag=1;
								break;
								case 4://0204
									ForwardFlag=1;
								break;
								case 5://0205
									HeartCnt=0;WaterYield_L=0;
								break;
								case 8://0208
									ForwardFlag=1;
								break;
								case 9://0209
									ForwardFlag=1;
								break;
								default:break;
							}
						break;
						case 0x12://1204和1209都是离线数据指令的回复包
							if(UartDataDisBuf[10]==9||UartDataDisBuf[10]==4)ForwardFlag=1;HeartCnt=0;//1204
						break;
						default:break;
					}
					if(ForwardFlag)
					{
						ForwardFlag=0;
						memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//将数据拷贝到发送缓冲区
						if(UartDataDisBuf[7]&0x10||(UartDataDisBuf[7]&0xf0)==0)//一号出水口指令
						{
							SendFixedLength(3,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);
						}
						if(UartDataDisBuf[7]&0x20||(UartDataDisBuf[7]&0xf0)==0)//2号出水口指令
						{
							SendFixedLength(4,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);
						}
					}
				}
			}
		}
		if(SaveParameterFlag)
		{
			SaveParameterFlag=0;WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);//SoundFlag=5;
		}
	}
}
uint8_t Str2001[]={0x7e,0,12+18,0x20,SheClass,0,0,0,0,0x20,0x01};
void Uart3DataDispose()//与出水口通讯
{
	uint8_t ch1=0,ch2=0;
//	uint16_t int1=0,port1=0;
//	uint32_t long1=0;
	if(S3ReceiveFinish)
	{
		S3ReceiveFinish=0;
		for(ch2=0;ch2<8;ch2++)
		{
			if(S3RecDataBuf[ch2][0]==0x7e)
			{
				memcpy(UartDataDisBuf,&S3RecDataBuf[ch2][0],50);
				memset(&S3RecDataBuf[ch2][0],0,50);
				if(CompCRC(UartDataDisBuf))continue;
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3])&&(UartDataDisBuf[7]&0x10))//设备号相同并且出水口号为1则转发给服务器
				{
					memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);
					SendFixedLength(2,UartSendDataBuf);//将1号出水口发送的数据转发给服务器
				}
				else
				{
					if(UartDataDisBuf[9]==0x20&&UartDataDisBuf[10]==1)//2001指令是出水口请求参数同步指令
					{
						OffLineData1=Array_Int(&UartDataDisBuf[23]);//取1号口的离线数据条数
						W_Joint2001Data();
						W_Send2001Data_1();
					}
				}
			}
		}
	}
}
void Uart4DataDispose()//与出水口通讯
{
	uint8_t ch1=0,ch2=0;
	if(S4ReceiveFinish)
	{
		S4ReceiveFinish=0;
		for(ch2=0;ch2<8;ch2++)
		{
			if(S4RecDataBuf[ch2][0]==0x7e)
			{
				memcpy(UartDataDisBuf,&S4RecDataBuf[ch2][0],50);
				memset(&S4RecDataBuf[ch2][0],0,50);
				if(CompCRC(UartDataDisBuf))continue;
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3])&&(UartDataDisBuf[7]&0x20))//设备号相同并且出水口号为1则转发给服务器
				{
					memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);
					SendFixedLength(2,UartSendDataBuf);//将1号出水口发送的数据转发给服务器
				}
				else
				{
					if(UartDataDisBuf[9]==0x20&&UartDataDisBuf[10]==1)//2001指令是出水口请求参数同步指令
					{
						OffLineData2=Array_Int(&UartDataDisBuf[23]);//取1号口的离线数据条数
						W_Joint2001Data();
						W_Send2001Data_2();
					}
				}
			}
		}
	}
}
void UartDataDispose()
{
	Uart1DataDispose();
	Uart2DataDispose();
	Uart3DataDispose();
	Uart4DataDispose();
}
void W_Send2001Data_1()
{
	Int_Array(SetParameterData.Pluse1,&UartSendDataBuf[17]);
	Int_Array(SetParameterData.CardPrice1,&UartSendDataBuf[19]);
	Int_Array(SetParameterData.VirtualPrice1,&UartSendDataBuf[21]);
	PushZhenNum(UartSendDataBuf);
	UartSendDataBuf[7]=0x10;
	SendDataPackage(3,UartSendDataBuf);//转发给1号出水口
}
void W_Send2001Data_2()
{
	Int_Array(SetParameterData.Pluse2,&UartSendDataBuf[17]);
	Int_Array(SetParameterData.CardPrice2,&UartSendDataBuf[19]);
	Int_Array(SetParameterData.VirtualPrice2,&UartSendDataBuf[21]);
	PushZhenNum(UartSendDataBuf);
	UartSendDataBuf[7]=0x20;
	SendDataPackage(4,UartSendDataBuf);//转发给2号出水口
}
void W_Send2001Data_3()//发送给显示屏
{
	UartSendDataBuf[2]=37;
	UartSendDataBuf[15]=ShowState;//显示设备状态
	if(ErrorTemp>1)UartSendDataBuf[16]=ErrorTemp;//故障状态
	else UartSendDataBuf[16]=0;
	UartSendDataBuf[17]=WaterLevelState;//水位状态
	memcpy(&UartSendDataBuf[18],&SysTime.Hour,4);//放入时间
//	UartSendDataBuf[18]=SysTime.Hour;
//	UartSendDataBuf[19]=SysTime.Minute;
//	UartSendDataBuf[20]=SysTime.Week;
	memcpy(&UartSendDataBuf[11],Cdatas,4);//放入设备号
	memcpy(&UartSendDataBuf[22],&UsedWater.ZSYield,9);//放入水量
	SendDataPackage(1,UartSendDataBuf);//转发给显示屏
}
void W_Joint2001Data()
{
	uint8_t i=0;
//	for(i=0;i<50;i++)UartSendDataBuf[i]=0;
	memset(UartSendDataBuf,0,31);
	memcpy(UartSendDataBuf,Str2001,sizeof(Str2001));//放入数据包头
	memcpy(&UartSendDataBuf[11],Cdatas,4);//放入设备号
	if(NetWork)UartSendDataBuf[25]=1;//设备在线状态
	Int_Array(SetParameterData.MaxMoney,&UartSendDataBuf[15]);
	UartSendDataBuf[15]|=0x80;
}
/*
//-----------------------------------------------------------------
//                  C51中字符串函数的扩充            
// 函数名称    : strsearch ()
// 函数功能    : 在指定的数组里连续找到相同的内容
// 入口参数   ： ptr2=要找的内容, ptr1 当前数组
// 出口参数 ： 0-没有 找到   >1 查找到
//----------------------------------------------------------------
*/
uint8_t strsearch(uint8_t *ptr2,uint8_t *ptr1_at)//查字符串*ptr2在*ptr1中的位置
//本函数是用来检查字符串*ptr2是否完全包含在*ptr1中
//返回:  0  没有找到
//       1-255 从第N个字符开始相同
{ uint8_t i,j,k;

  if(ptr2[0]==0) return(0);
  for(i=0,j=0;i<100;i++)
  { if(ptr1_at[i]==ptr2[j])    //找到第一个字符60-2
    { k=i;
      do
      { if(ptr2[j]==0) return(i+1);     //比较完所有的字符，正确
        if(ptr1_at[k]!=ptr2[j]) break;		//不等，退出重找
        k++;j++ ;																							//比较下一个
      } while(k<100) ;//60-2
      j=0;
    }
  }
  return(0);
}	
uint8_t SearchID()     //查找卡ID号
{ uint8_t ch1,ch2,Error;

//0123456  7  8  901234567890123456789  0  1  2  345
//AT+CCID\0d\0d\0a898607b6151730454252\0d\0a\0d\0aOK

  Error=1;
  if(strsearch("OK",S2RecBuf))       //查找OK
  { ch1=strsearch("CCID",S2RecBuf);  //查找CCID
    if(ch1>0)                       //找到CCID
    { for(ch2=0;ch2<10;++ch2)       //CCID后找回车换行
        if(S2RecBuf[ch1+ch2]==0x0d) break;   //查找CCID后的0d0a
      if(ch2<10) 
      { ch1+=ch2;
        for(ch2=0;ch1<8;++ch2)
          if(S2RecBuf[ch1+ch2]>0x1f) break;    //应该是数字,大于2f,防止以后扩展只要大于控制字符就是成功
        if(ch2<8)
        { ch1+=ch2;
          if(S2RecBuf[ch1+20]==0x0d)
          { memcpy(SimStr,&S2RecBuf[ch1],20);  //取IF串号
            SimStr[20]=0;      //结尾字符
            Error=0;
          }
        }
      }
    }
  } 
	if(Error)NetProce=0;
	return Error;
}
uint8_t code Str102[]={0x7e,0,12+32,0x20,SheClass,0x00,0x00,0,0x00,0x01,0x02};
void InitModel()//放在秒处理函数里
{ 
	uint8_t ch1,ch2;
	ch1=NetProce&0xf0;       //高位=1寻网,=2设置透传,=3设置IP,=4成功
  switch(ch1)
	{
		case 0:          //断电送电进程
      ch2=NetProce&0x0f;
			if(ch2==4||ch2==5)
			{
				Uart2SendStr("AT\x0d\x0a");
			}
			switch(ch2)
			{
				case 0:
					Uart2SendStr("+++"); //"+++"之后需间隔500ms发数据退出透传才能起到作用
				break;
				case 6:
					AutoReconnection++;
				if(AutoReconnection>=6)//6次没有联网成功，则给模块断电重启
				{
					AutoReconnection=0;
//暂时屏蔽
					NetProce=0;PowerDown=0;M800Power=0;NetWork=0;//断电计时器
				}
				Uart2SendStr("AT+CIPSHUT\x0d\x0a"); 
				break;
				case 7:
					memset(S2RecBuf,0,110);//初始化接收缓冲区
					S2RecCount=0;    //for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;接收区清零  //为接收做好准备  
					Uart2SendStr("AT+CCID\x0d\x0a");
					
				break;
				case 0x0b:
					if(SearchID()==0)     //查找卡ID串号
					{
						RorS_Flag=0; NetProce=0x10;  //进入发送命令进程
					}
				break;
				default: break;
			}
			NetProce++;
    break;
    case 0x10:                  //寻网
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //接收区清零
				memset(S2RecBuf,0,110);//初始化接收缓冲区
        S2RecCount=0;      //为接收做好准备
				Uart2SendStr("AT+CSQ\x0d\x0a");//发送寻网命令
        RorS_Flag=1;
      }
      else
      { RorS_Flag=0;
        ch1=strsearch("+CSQ:",S2RecBuf);								//应该受到"+CSQ: 26,0"(其中26为强度,必须大于10) 和+COPS: 0,0,"CHINA MOBILE"
        if(ch1>0)         //找到+CSQ
        { if(S2RecBuf[ch1+6]==',')  CommVal=S2RecBuf[ch1+5]-'0';      //如果信号强度只有一位数
          else
          { CommVal=S2RecBuf[ch1+5]-'0';
            CommVal*=10;
            CommVal+=(S2RecBuf[ch1+6]-'0');
          }
          if((CommVal>7)&&(CommVal!=99))   NetProce=0x20;//信号强度为99表示未知或不可测
        }
        if(NetProce<0x20)
        { ++NetProce;
          if(NetProce>0x1c) NetProce=0x0;
        }          
      }
      break;
    case 0x20:                     //透传
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //接收区清零
				memset(S2RecBuf,0,110);//初始化接收缓冲区
//				RestData(S2RecBuf);
        S2RecCount=0;      //为接收做好准备
				Uart2SendStr("AT+CIPMODE=1\x0d\x0a");//发送透传命令
        RorS_Flag=1;
      }
      else
      { RorS_Flag=0;
        if(ch1=strsearch("OK",S2RecBuf))	NetProce=0x30;	//应该收到OK
        else//等待
        { ++NetProce;
          ch2=NetProce%0x10;
          if(ch2>2) NetProce=0;
        }          
      }
      break;
    case 0x30:                       //送IP
      if(RorS_Flag==0)
      { //strcpy(UartSendDataBuf,"AT+CIPSTART=\"TCP\",\"39.106.74.186\",9999\x0d\x0a"); S2TransLen=40; //发送IP
                         //012345678901 2345 67 8
        strcpy(UartSendDataBuf,"AT+CIPSTART=\"TCP\",\"");
        //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21  22 23 24 25 26  
        //n 0 3 9 . 1 0 6 . 0 7  4  .  1  8  6 \"                  0  9   9  9  9   
        ch1=IportBuf[0];
        memcpy(&UartSendDataBuf[19],&IportBuf[1],ch1);
        ch1+=19;
        UartSendDataBuf[ch1]='"';
        ++ch1;UartSendDataBuf[ch1]=',';
        ++ch1;
        memcpy(&UartSendDataBuf[ch1],&IportBuf[22],5);    //端口
        ch1+=5;UartSendDataBuf[ch1]=0x0d;
        ++ch1;UartSendDataBuf[ch1]=0x0a;
        ++ch1;
        S2TransLen=ch1;	
				memset(S2RecBuf,0,110);//初始化接收缓冲区
        S2RecCount=0;      //为接收做好准备
				for(ch1=0;ch1<S2TransLen;++ch1)
				{
					Uart2Send(UartSendDataBuf[ch1]);
				}
        RorS_Flag=1;
      }
      else
      { 
				if(ch1=strsearch("ALREADY CONNECT",S2RecBuf))
				{
					NetProce=0;							//应该收到CONNECT
          RorS_Flag=0;  break;
				}
				else if(ch1=strsearch("CONNECT\x0d\x0a",S2RecBuf))
        {	NetProce=0x40;							//应该收到CONNECT
          RorS_Flag=0;   
					AutoReconnection=0;//透传设置成功后重连计数器清零		
//					memset(S1TransBuf,0,50);//初始化串口1发送缓冲区
//					memcpy(S1TransBuf,Str301,11);  //设备在线数据包帧头
//					S1TransBuf[11]=0x01;PushCRC(S1TransBuf); 
//					SendFixedLength1(S1TransBuf);//发送设备在线指令给主板，主板需要上传登陆包及状态包
					if(YuanipFlag)          //如果是新IP
          { YuanipFlag=0;IportBuf[27]=0x5a;
            WriteEeprom(SysIpPort,28,IportBuf); 
          }
        }
        else
        { 
//					if(SecondByte%2) 
					++NetProce;
          ch2=NetProce%0x10;
          if(ch2>8)//该指令的最大响应时间为9秒
          { NetProce=0;
            if(YuanipFlag)
            { ++NewIPTime;
              if(NewIPTime>3) while(1);    //如果3次不成功,放弃转网的IP
            }
          }
        }          
      }
      break;
		case 0x40:                       //登陆,送Sim卡字串
      if(RorS_Flag==0)
      { 
				memset(S2RecBuf,0,110);//初始化接收缓冲区
				memset(UartSendDataBuf,0,50);//初始化接收缓冲区
        memcpy(UartSendDataBuf,Str102,11);  //登陆包头
        memcpy(&UartSendDataBuf[11],Cdatas,4);  //设备号
        UartSendDataBuf[4]=SheClass;               //设备类型	多出水口售水机时间计费型
        memcpy(&UartSendDataBuf[15],SimStr,20);    //Sim字串   
				OffLineData=OffLineData1+OffLineData2;
				Int_Array(OffLineData,&UartSendDataBuf[37]);
        NetOverTime=0;     //重新开始心跳计时
        S2RecCount=0;      //为接收做好准备
//        PushCRC(UartSendDataBuf);   
//				SendFixedLength(UartSendDataBuf);
				PushZhenNum(UartSendDataBuf);
				SendDataPackage(2,UartSendDataBuf);
        RorS_Flag=1;NetProce&=0xf0;
//				Uart1Send(0xa1);
      }
      else
      { 
				if((S2RecBuf[0]==0x7e)&&(S2RecBuf[9]==2)&&(S2RecBuf[10]==2)&&(CompCRC(S2RecBuf)==0))   //如果是回复的202
        {	NetProce=0x50;	NetWork=1;							//
          NetOverTime=0;     //重新开始心跳计时
					TimingFlag=1;HeartFlag=1;ZSTimeFlag=1;//ParameterFlag=1;上电后所有主动发送的数据都上传一遍		
          if(YuanipFlag)          //如果是新IP
          { YuanipFlag=0;
            WriteEeprom(SysIpPort,27,IportBuf); 
          }
					//Uart1Send(0xa1); 
        }
        else
        { ++NetProce;
          ch2=NetProce%0x10;//Uart1Send(0xa2); 
          if(ch2>10)
          { NetProce=0;
            if(YuanipFlag)
            { ++NewIPTime;
              if(NewIPTime>3) while(1);    //如果3次不成功,放弃转网的IP
            }
          }
        } 
//				for(ch1=0;ch1<110;ch1++)Uart1Send(S2RecBuf[ch1]); 
				        
      } 
			
      break;
			default:break;
	}
}
void Int_Array(uint16_t int1,uint8_t *pArray)
{
	*pArray=int1/0x100;
	*(pArray+1)=int1%0x100;
}
uint16_t Array_Int(uint8_t *pArray)
{
	uint16_t int1=0;
	int1=*pArray*0x100+*(pArray+1);
	return int1;
}


//发送定长数据，第三个字节表示长度
void SendFixedLength(uint8_t uartx,uint8_t Str[])
{
	uint8_t ch1;
	for(ch1=0;ch1<Str[2]+1;ch1++)
	{
		switch(uartx)
		{
			case 1:Uart1Send(Str[ch1]);break;
			case 2:Uart2Send(Str[ch1]);break;
			case 3:Uart3Send(Str[ch1]);break;
			case 4:Uart4Send(Str[ch1]);break;
			default:break;
		}
	}
}
void SendDataPackage(uint8_t uartx,uint8_t Str[])
{
	PushCRC(Str);//放入CRC
	SendFixedLength(uartx,Str);
}

//在数组中放入CRC-----------------------
void PushCRC(uint8_t Str[])
{ uint8_t ch1;
  uint16_t int1;
  ch1=Str[2];    //长度
  ch1-=2;        //CRC长度
  int1=CRC_Calculate(ch1,&Str[1]);  //计算CRC
  ++ch1;        //指向第一个CRC
  Str[ch1]=(uint8_t)(int1/0x100);
  ++ch1;
  Str[ch1]=(uint8_t)(int1%0x100);
}
//在数组中放入帧序号--------------------------
void PushZhenNum(uint8_t Str[])
{  Str[5]=(uint8_t)(ZhenNum/0x100);
   Str[6]=(uint8_t)(ZhenNum%0x100);
   ++ZhenNum;            //下一次的帧序
}
//=======================================
//计算CRC------------------------------------
uint16_t CRC_Calculate(uint8_t Length,uint8_t *Address)
{ uint8_t i,j;
  uint16_t  CRC;
  CRC=0xffff;
  for(i=0;i<Length;i++)
  { CRC^=*Address++;
    for(j=0;j<8;j++)
    { if(CRC&0x01) CRC=(CRC>>1)^0x8408;
      else CRC>>=1;
    }
  }
  return ~CRC;
}
//比较数据包中的CRC-----------------------
bit CompCRC(uint8_t Str[])
{ uint8_t ch1,ch2;
  uint16_t int1;

  if(Str[0]!=0x7e) return 1;  //如果包头不对
  ch1=Str[2];                 //发送的长度
  ch1-=2;                     //CRC的长度
  int1=CRC_Calculate(ch1,&Str[1]); //计算CRC
  ch2=(uint8_t)(int1/0x100);    //CRC高字节
  ++ch1;                      //指向CRC第一字节
  if(ch2!=Str[ch1])  return 1; //CRC第一字节未通过
  ++ch1;
  ch2=(uint8_t)(int1%0x100);     //CRC低字节
  if(ch2!=Str[ch1])  return 1; //CRC第2字节未通过
  return 0;
}

void ReceiveTimeOut()//接收超时处理，115200波特率，11520字节/秒,没毫秒可以传输11个字节
{
	if(S1ReceiveFlag)//检查接收完毕的超时时间应该大于主函数运行时间，否则坑出现，上次的数据还未处理完，又有数据接收完毕了，就会把后面接收到的数据漏处理。
	{
		S1Receive1ms++;
		if(S1Receive1ms>10)//如果连续3毫秒都没有接收到数据的话则认为数据接收完毕，需要将接收完毕标志进行置位，便于主函数的数据处理
		{
			S1Receive1ms=0;S1ReceiveFinish=1;S1ReceiveFlag=0;S1ReceiveLength=0;//S1DataLength=0;S1HeadFlag=0;
		}
	}
	else S1Receive1ms=0;
//	if(S2ReceiveFlag)//检查接收完毕的超时时间应该大于主函数运行时间，否则坑出现，上次的数据还未处理完，又有数据接收完毕了，就会把后面接收到的数据漏处理。
//	{
//		S2Receive1ms++;
//		if(S2Receive1ms>100)//如果连续3毫秒都没有接收到数据的话则认为数据接收完毕，需要将接收完毕标志进行置位，便于主函数的数据处理
//		{
//			S2Receive1ms=0;if(NetWork){S2HeadFlag=0;S2ReceiveFinish=1;S2PacketNum=0;S2ReceiveLength=0;S2ReceiveFlag=0;S2DataLength=0;}
//		}
//	}
//	else S2Receive1ms=0;

		if(S2ReceiveFlag)       //如果正在接收
    { ++S2Receive1ms;
      if(S2Receive1ms>10)   
      { S2ReceiveFinish=1;S2PacketNum=0;S2ReceiveFlag=0;S2ReceiveLength=0;S2Begin=0;}
    }
		else S2Receive1ms=0;
		if(S3ReceiveFlag)       //如果正在接收
    { ++S3Receive1ms;
      if(S3Receive1ms>10)   
      { S3ReceiveFinish=1;S3PacketNum=0;S3ReceiveFlag=0;S3ReceiveLength=0;S3Begin=0;}
    }
		else S3Receive1ms=0;
		if(S4ReceiveFlag)       //如果正在接收
    { ++S4Receive1ms;
      if(S4Receive1ms>10)   
      { S4ReceiveFinish=1;S4PacketNum=0;S4ReceiveFlag=0;S4ReceiveLength=0;S4Begin=0;}
    }
		else S4Receive1ms=0;
//	if(S3ReceiveFlag)//检查接收完毕的超时时间应该大于主函数运行时间，否则坑出现，上次的数据还未处理完，又有数据接收完毕了，就会把后面接收到的数据漏处理。
//	{
//		S3Receive1ms++;
//		if(S3Receive1ms>100)//如果连续3毫秒都没有接收到数据的话则认为数据接收完毕，需要将接收完毕标志进行置位，便于主函数的数据处理
//		{
//			S3Receive1ms=0;S3HeadFlag=0;S3ReceiveFinish=1;S3PacketNum=0;S3ReceiveLength=0;S3ReceiveFlag=0;S3DataLength=0;
//		}
//	}
//	else S3Receive1ms=0;
//	if(S4ReceiveFlag)//检查接收完毕的超时时间应该大于主函数运行时间，否则坑出现，上次的数据还未处理完，又有数据接收完毕了，就会把后面接收到的数据漏处理。
//	{
//		S4Receive1ms++;
//		if(S4Receive1ms>100)//如果连续3毫秒都没有接收到数据的话则认为数据接收完毕，需要将接收完毕标志进行置位，便于主函数的数据处理
//		{
//			S4Receive1ms=0;S4HeadFlag=0;S4ReceiveFinish=1;S4PacketNum=0;S4ReceiveLength=0;S4ReceiveFlag=0;S4DataLength=0;
//		}
//	}
//	else S4Receive1ms=0;
}
void UartIsr() interrupt 4
{
	uint8_t ch1;
    if (TI)
    {
        TI = 0;
        busy1 = 0;
    }
    if (RI)
    {
        RI = 0;
				ch1=SBUF;
				S1ReceiveFlag=1;S1Receive1ms=0;
				S1RecBuf[S1ReceiveLength++]=ch1;if(S1ReceiveLength>=50)S1ReceiveLength=0;
    }
}
void Uart2Isr() interrupt 8
{
	uint8_t DATA_uart;
    if (S2CON & 0x02)
    {
        S2CON &= ~0x02;
        busy2 = 0;
    }
    if (S2CON & 0x01)
    {
			S2CON &= ~0x01;//S2BUF;
			DATA_uart=S2BUF;//S2BUF=ch1;
			if(NetWork)
			{
				if(S2Begin==3)              //4、接收
				{ S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
					--S2DataLength;
					if(S2DataLength==0)
					{ S2Begin=0;S2ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
						S2PacketNum++;if(S2PacketNum>7)S2PacketNum=0;//接收完一条数据，下一条存放在接收缓存的下一行
						return;
					}
				}
				if(S2Begin==2)               //3、找长度
				{ ++S2Begin; S2DataLength=DATA_uart-2; 
					S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
				}
				if(S2Begin==1)               //2、找0
				{ if(DATA_uart==0)
					{ ++S2Begin;
						S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
					}
					else 
					{ S2Begin=0;S2ReceiveLength=0;}
				}
				if(S2Begin==0)                //1、先找7e
				{ if(DATA_uart==0x7e) 
					{ S2Begin=1;S2ReceiveLength=0;
						S2RecDataBuf[S2PacketNum][S2ReceiveLength]=DATA_uart; ++S2ReceiveLength;
					}
				}
				S2ReceiveFlag=1;S2Receive1ms=0;      //接收标志和字符中间的间隔清零,为无传输字符退出做准备
				if(S2ReceiveLength>=49) S2ReceiveLength=0;//如果接收的是通讯数据，则每条最多接收50个字节
			}
			else {S2RecBuf[S2RecCount++]=DATA_uart;if(S2RecCount>=99)S2RecCount=0;}
    }
}
void Uart3Isr() interrupt 17
{
	uint8_t DATA_uart;
    if (S3CON & 0x02)
    {
        S3CON &= ~0x02;
        busy3 = 0;
    }
    if (S3CON & 0x01)
    {
        S3CON &= ~0x01;//S3BUF;
			DATA_uart=S3BUF;
			if(S3Begin==3)              //4、接收
			{ S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
				--S3DataLength;
				if(S3DataLength==0)
				{ S3Begin=0;S3ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
					S3PacketNum++;if(S3PacketNum>7)S3PacketNum=0;//接收完一条数据，下一条存放在接收缓存的下一行
					return;
				}
			}
			if(S3Begin==2)               //3、找长度
			{ ++S3Begin; S3DataLength=DATA_uart-2; 
				S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
			}
			if(S3Begin==1)               //2、找0
			{ if(DATA_uart==0)
				{ ++S3Begin;
					S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
				}
				else 
				{ S3Begin=0;S3ReceiveLength=0;}
			}
			if(S3Begin==0)                //1、先找7e
			{ if(DATA_uart==0x7e) 
				{ S3Begin=1;S3ReceiveLength=0;
					S3RecDataBuf[S3PacketNum][S3ReceiveLength]=DATA_uart; ++S3ReceiveLength;
				}
			}
			S3ReceiveFlag=1;S3Receive1ms=0;      //接收标志和字符中间的间隔清零,为无传输字符退出做准备
			if(S3ReceiveLength>=49) S3ReceiveLength=0;//如果接收的是通讯数据，则每条最多接收50个字节
    }
}
void Uart4Isr() interrupt 18
{
	uint8_t DATA_uart;
	if (S4CON & 0x02)
	{
			S4CON &= ~0x02;
			busy4 = 0;
	}
	if (S4CON & 0x01)
	{
			S4CON &= ~0x01;//S4BUF;
		DATA_uart=S4BUF;
		if(S4Begin==3)              //4、接收
		{ S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
			--S4DataLength;
			if(S4DataLength==0)
			{ S4Begin=0;S4ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
				S4PacketNum++;if(S4PacketNum>7)S4PacketNum=0;//接收完一条数据，下一条存放在接收缓存的下一行
				return;
			}
		}
		if(S4Begin==2)               //3、找长度
		{ ++S4Begin; S4DataLength=DATA_uart-2; 
			S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
		}
		if(S4Begin==1)               //2、找0
		{ if(DATA_uart==0)
			{ ++S4Begin;
				S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
			}
			else 
			{ S4Begin=0;S4ReceiveLength=0;}
		}
		if(S4Begin==0)                //1、先找7e
		{ if(DATA_uart==0x7e) 
			{ S4Begin=1;S4ReceiveLength=0;
				S4RecDataBuf[S4PacketNum][S4ReceiveLength]=DATA_uart; ++S4ReceiveLength;
			}
		}
		S4ReceiveFlag=1;S4Receive1ms=0;      //接收标志和字符中间的间隔清零,为无传输字符退出做准备
		if(S4ReceiveLength>=49) S4ReceiveLength=0;//如果接收的是通讯数据，则每条最多接收50个字节
	}
}
void Uart1Send(char dat)
{
    while (busy1);
    busy1 = 1;
    SBUF = dat;
}

//void Uart1SendStr(char *p)
//{
//    while (*p)
//    {
//        Uart1Send(*p++);
//    }
//}
void Uart2Send(char dat)
{
    while (busy2);
    busy2 = 1;
    S2BUF = dat;
}

void Uart2SendStr(char *p)
{
    while (*p)
    {
        Uart2Send(*p++);
    }
}
void Uart3Send(char dat)
{
    while (busy3);
    busy3 = 1;
    S3BUF = dat;
}

//void Uart3SendStr(char *p)
//{
//    while (*p)
//    {
//        Uart3Send(*p++);
//    }
//}
void Uart4Send(char dat)
{
    while (busy4);
    busy4 = 1;
    S4BUF = dat;
}

//void Uart4SendStr(char *p)
//{
//    while (*p)
//    {
//        Uart4Send(*p++);
//    }
//}
void W_Uart1Config(void)//24Mhz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xCC;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES=1;
}
void W_Uart2Config(void)//24Mhz
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xCC;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2 |= 0x01;
}
void W_Uart3Config(void)//24Mhz
{
	S3CON = 0x10;		//8位数据,可变波特率
	S3CON &= 0xBF;		//串口3选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xCC;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
  IE2 |= 0x08;
}
void W_Uart4Config(void)//24Mhz
{
	S4CON = 0x10;		//8位数据,可变波特率
	S4CON &= 0xBF;		//串口4选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xCC;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
  IE2 |= 0x10;	//开启串口4中断
}
void W_UartConfig(void)
{
	W_Uart1Config();
	W_Uart2Config();
	W_Uart3Config();
	W_Uart4Config();
}
//-------------PCF8563-----------
void delay()    //略微延时  6us约>4.7us
{
_nop_();
  _nop_();
   _nop_();
    _nop_();
         _nop_();
          _nop_();
}

void iic_start()   //启动信号
{
	SDA_clock=1;
	SCL_clock=1;
	delay();
	SDA_clock=0;
	delay();
	SCL_clock=0;
}

void iic_stop()  //停止信号
{
	SDA_clock=0;
	SCL_clock=1;
	delay();
	SDA_clock=1;
	delay();
	SCL_clock=0;
}

void iic_ack()   //应答信号
{
	uint8_t i=0;
	SCL_clock=1;
	delay();
	while((SDA_clock==1)&&(i<255))
	i++;
	SCL_clock=0;
	delay();
}

void iic_send_byte(uint8_t bat)  //发送数据
{
	uint8_t i,temp;
	temp=bat;
	for(i=0;i<=7;i++)
	{
	  temp=temp<<1;
	  SCL_clock=0;
	  SDA_clock=CY;
	  delay();
	  SCL_clock=1;
	  delay();
	}
	SCL_clock=0;
	delay();
	SDA_clock=1;
	delay();
}

uint8_t iic_rev()  //接受数据
{
  uint8_t temp,i;
  SCL_clock=0;
  delay();
  SDA_clock=1;
  for(i=0;i<=7;i++)
  {
   SCL_clock=1;
   delay();
   temp=(temp<<1)|SDA_clock;
   SCL_clock=0;
   delay();
  }
  delay();
  return temp;
}

void iic_send_add_byte(uint8_t add,uint8_t bat) //向某个地址发送某数据
{
	iic_start();
	iic_send_byte(0xa2);
	iic_ack();
	iic_send_byte(add);
	iic_ack();
	iic_send_byte(bat);
	iic_ack();
	iic_stop();
}

uint8_t iic_rec_add_byte(uint8_t add) //从某个地址读出数据
{
   uint8_t temp;
   iic_start();
   iic_send_byte(0xa2);
   iic_ack();
   iic_send_byte(add);
   iic_ack();
   iic_start();
   iic_send_byte(0xa3);
   iic_ack();
   temp=iic_rev();
   iic_stop();
   return temp;
}

//-----------时间预设定值---------uint8_t Sec,
void time_init(uint8_t Year,uint8_t Moom,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec,uint8_t Week)
{
	Sec=dec_bcd(Sec);Min=dec_bcd(Min);Hour=dec_bcd(Hour);Day=dec_bcd(Day);Moom=dec_bcd(Moom);Year=dec_bcd(Year);
	if(Week==7)Week=0;
	iic_send_add_byte(0x02,Sec);         //50秒
	iic_send_add_byte(0x03,Min);         //59分钟    
	iic_send_add_byte(0x04,Hour);        //19小时             
	iic_send_add_byte(0x05,Day);  		//1号
	iic_send_add_byte(0x06,Week);  		//星期
	iic_send_add_byte(0x07,Moom);      //10月  20世纪的
	iic_send_add_byte(0x08,Year);      //2017年
}
//----------------PCF8563初始化-----------------
void pcf_init()
{
	iic_send_add_byte(0x00,0x00);  //启动时钟
}
uint8_t dec_bcd(uint8_t bat)
{
	uint8_t ch1;
	ch1=bat/10*0x10+bat%10;
	return ch1;
}
//----------------BCD转10进制-----------
uint8_t bcd_dec(uint8_t bat)
{
	uint8_t temp1,temp2,tol;
	temp1=bat&0x0f;
	temp2=(bat&0xf0)>>4;
	tol=temp2*10+temp1;
	return tol; 
}
void Readtime()
{
	SysTime.Second=bcd_dec(0x7f&iic_rec_add_byte(0x02));    //读取秒
	SysTime.Minute=bcd_dec(0x7f&iic_rec_add_byte(0x03));   //读取分钟并转化为十进制显示。
	SysTime.Hour=bcd_dec(0x3f&iic_rec_add_byte(0x04));  //读取小时并转化为十进制显示。
	SysTime.Day=bcd_dec(0x3f&iic_rec_add_byte(0x05));   //读取天数并转化为十进制显示。
	SysTime.Week=bcd_dec(0x07&iic_rec_add_byte(0x06));   //读取星期并转化为十进制显示。
	if(SysTime.Week==0)SysTime.Week=7;
	SysTime.Month=bcd_dec(0x1f&iic_rec_add_byte(0x07));  //读取月并转化为十进制显示。
	SysTime.Year=bcd_dec(0xff&iic_rec_add_byte(0x08));    //读取年并转化为十进制显示。
	if(SysTime.Second>59||SysTime.Minute>59||SysTime.Hour>23||SysTime.Day>31||SysTime.Month>12||SysTime.Year>50){SysTime.Year=21;SysTime.Month=5;SysTime.Day=20;SysTime.Hour=0;SysTime.Minute=0;SysTime.Second=0;SysTime.Week=4;}//如果时间不正确，则时间初始化为20年12月1日0时
}
//-------------PCF8563-----------