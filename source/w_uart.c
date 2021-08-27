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
uint8_t S1PacketNum=0,S2PacketNum=0,S3PacketNum=0,S4PacketNum=0;//���ݰ�����
uint8_t	S1DataLength=0,S2DataLength=0,S3DataLength=0,S4DataLength=0;
uint8_t	S1Receive1ms=0,S2Receive1ms=0,S3Receive1ms=0,S4Receive1ms=0;
uint8_t S2Begin=0,S3Begin=0,S4Begin=0;
uint16_t ZhenNum=0;

//extern bit AlterParameter1,AlterParameter2;

uint8_t code Cdatas[4] _at_ 0x7ff5;
//#define TestServer
#ifdef	TestServer
//uint8_t code Domain[]={"\x015testmanage.waterer.cn"};  //����,��һ�ֽ�Ϊ��������http://testmanage.waterer.cn/
uint8_t code Domain[]={"\x015testmanage.wateree.cn"};
uint8_t code Port[]={"09999"};
#else
uint8_t code Domain[]={"\x00ds0.waterer.cn"};  //����,��һ�ֽ�Ϊ��������http://testmanage.waterer.cn/
uint8_t code Port[]={"09999"};
#endif


bit PowerDown=0;//ģ��ϵ��־
uint8_t PowerDown1s=0,SaveLiData1s=0;//�ϵ��ʱ��
bit NetWork=0,RorS_Flag=0;
uint8_t CommVal=0,NetOverTime=0;
bit YuanipFlag=0;
uint8_t NewIPTime=0;
uint8_t IportBuf[30]={0};
bit PowerOn=0,ShutDown=0;//���ػ�ָ��
bit SaveParameterFlag=0;
bit StateFlag=0,HeartFlag=0,TimingFlag=0,ZSTimeFlag=0,SaveTimeFlag=0;//ʱ��ͬ��ָ���־��״̬����־����������־,
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
bit ForwardFlag=0,BroadcastFlag=0;//ת����־,�㲥��־
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

//extern struct 
//{ uint8_t		FirstChar;   //��ͷ      1B
//	uint16_t	CardPrice1;			//ˢ������
//	uint16_t	CardPrice2;			//ˢ������
//	uint16_t	VirtualPrice1;	//ɨ�뵥��
//	uint16_t	VirtualPrice2;	//ɨ�뵥��
//	uint16_t	Pluse1;					//1�ſ�������������
//	uint16_t	Pluse2;					//2�ſ�������������
//	uint16_t	MaxMoney;				//��󵥴γ�ˮ��
//	
//	uint8_t		ParameterLock;	//�豸������
////	uint8_t		Confiscate;		//û��ʱ��				1B
//	uint8_t 	OnceCXTime;	//���γ�ϴʱ��					1B
//	uint8_t		CXInterval;	//��ϴ���							1B
//	uint8_t		HaltState;//ͣ��״̬
//	uint8_t 	OnceOzoneTime;//��������ʱ��				1B
//	uint8_t		OzoneInterval;//�����������ʱ��		1B
//	uint8_t 	LastChar;    //��β      1B
//}SetParameterData;
extern struct 
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

void FrameHead()//�̶�֡ͷ
{
	UartSendDataBuf[0]=0x7e;UartSendDataBuf[3]=0x20;UartSendDataBuf[4]=SheClass;UartSendDataBuf[9]=1;
				//ͷ									Э��汾						�豸����								��ˮ��
}
void SendDataJoint(void)
{
	SendDataInterval=DataInterval;//�������͵����ݶ���Ҫ��2s�ļ��ʱ��
	memset(UartSendDataBuf,0,50);//��ʼ�����ͻ�����
	FrameHead();
	if(TimingFlag)//ÿСʱУʱһ��
	{
		TimingFlag=0;
		UartSendDataBuf[2]=14;UartSendDataBuf[10]=3;UartSendDataBuf[11]=1;//���󱱾�ʱ��
		TimingTime=TimeInterval;
	}
	else if(ZSTimeFlag)
	{
		ZSTimeFlag=0;PushZhenNum(UartSendDataBuf);ZhenNumFlag=1;UartSendDataBuf[3]=0x21;UartSendDataBuf[2]=18;UartSendDataBuf[10]=7;Int_Array(UsedWater.ZSTime,&UartSendDataBuf[15]);//Int_Array(5,&UartSendDataBuf[15]);////������ˮʱ��
	}
	else if(StateFlag)
	{
		StateFlag=0;
		UartSendDataBuf[2]=30;UartSendDataBuf[10]=5;
		UartSendDataBuf[17]=SetParameterData.OnceCXTime;//UartSendDataBuf[20]=Temperature;//��������ʱ�䣬�����¶�
		UartSendDataBuf[25]=WorkStateByte;//����״̬�ֽ�
//		UartSendDataBuf[7]=0x10;
		UartSendDataBuf[28]=CommVal;//�ź�ǿ��
		Int_Array(SetParameterData.CardPrice1,&UartSendDataBuf[18]);//ˢ������
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
		Int_Array(WaterYield_L,&UartSendDataBuf[26]);//��ˮ��
		StateTime=StateInterval;
	}
	else if(HeartFlag)
	{
		UartSendDataBuf[2]=16;UartSendDataBuf[10]=1;HeartFlag=0;HeartTime=HeartInterval;//���ȣ���������
	}
	else return;
	memcpy(&UartSendDataBuf[11],Cdatas,4);//�����豸��
	if(ZhenNumFlag==0)PushZhenNum(UartSendDataBuf);//����֡��
	ZhenNumFlag=0;
	PushCRC(UartSendDataBuf);//����CRC
	SendFixedLength(2,UartSendDataBuf);
	if(HeartCnt>=5)
	{
		HeartCnt=0;NetProce=0;PowerDown=0;M800Power=0;NetWork=0;
		W_Joint2001Data();W_Send2001Data_1();W_Send2001Data_2();//���͸�������ˮ����ʾ�豸����
	}//������Ҫ�������ݸ���ʾ��
	else HeartCnt++;
}
void Uart1DataDispose()//����ʾ��ͨѶ
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
//ת��ָ����δ���
void Uart2DataDispose()//�������ͨѶ
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
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3]))//�豸����ͬ
//�����豸�ŵ�ָ���е�½�ظ�ָ��0202��ת��ָ��0301����ȡ��������ָ��0302������ָ��0304����ȡ״ָ̬��0305����ϴʱ������ָ��0306������ʱ������ָ��0307���豸������ָ��0309
//��󵥴γ�ˮ��ָ��0310������������ָ��0311����ֹ��ˮָ��0313����������ָ��0315����̨���ָ��0316
				{
					if(UartDataDisBuf[9]==2||UartDataDisBuf[9]==0x03)HeartCnt=0;
					switch(UartDataDisBuf[9])
					{
						case 2://�յ���½�ظ�����Ҫת������ˮ�ڣ�����ʾ��
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
								WriteEeprom(WaterAddress,sizeof(UsedWater),&UsedWater.FirstChar);// SoundFlag=5;//������ˮ����ص�����
								W_Joint2001Data();W_Send2001Data_3();
//								case 7://0207
//									
//								break;
							}
						break;
						case 3:
							memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//�����ݿ���������2�ķ��ͻ�������׼�����͸�������
							if((UartDataDisBuf[7]&0xf0)==0)BroadcastFlag=1;//����ǹ㲥ָ���ִ�������Ҫ���������ظ�
							switch(UartDataDisBuf[10])
							{
								case 1://0301ת��ָ����Ҫ��������
									NetOverTime=0;//�������лظ�������ѵ��߼�ʱ������
									if(UartDataDisBuf[21]>0)   //������
									{ ch1=UartDataDisBuf[21];	//��������
										memcpy(&IportBuf[1],&UartDataDisBuf[22],ch1);  //��������
										IportBuf[0]=ch1;    //��������
										YuanipFlag=1;       //����������־
										NewIPTime=0;       //����������
									}
									else if(UartDataDisBuf[15]|UartDataDisBuf[16]|UartDataDisBuf[17]|UartDataDisBuf[18])
									{
										IportBuf[0]=15;
										IportBuf[1]=UartDataDisBuf[15]/100+'0';//��λ
										ch1=UartDataDisBuf[15]%100;
										IportBuf[2]=ch1/10+'0';IportBuf[3]=ch1%10+'0';//ʮλ����λ
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
										NewIPTime=0;       //����������
									}
									else continue;
									
									//�˿�port------------------
									port1=UartDataDisBuf[19];port1*=0x100;port1+=UartDataDisBuf[20];
									IportBuf[22]=(uint8_t)(port1/10000)+'0';
									port1%=10000;
									ch1=port1/100;
									IportBuf[23]=ch1/10+'0';IportBuf[24]=ch1%10+'0';
									ch1=port1%100;
									IportBuf[25]=ch1/10+'0';IportBuf[26]=ch1%10+'0';
									
									NetProce=0;PowerDown=0;M800Power=0;NetWork=0;       //��ģ��ϵ���������NetWork=0;
//									UartDataDisBuf[9]=4;PushCRC(UartDataDisBuf);
//									memcpy(UartSendDataBuf,UartDataDisBuf,17);
//									SendFixedLength2(UartSendDataBuf);
								break;
								case 5://0305������ֱ�ӻظ�״̬����������
									StateFlag=1;return;
								break;
								case 6://0306����ϴ���ã�����ֱ��ִ�в����棬Ȼ��ظ�������
									if(UartDataDisBuf[15]>240||UartDataDisBuf[15]<5)SetParameterData.OnceCXTime=10;
									else SetParameterData.OnceCXTime=UartDataDisBuf[15];
									if(UartDataDisBuf[16]>240||UartDataDisBuf[16]<30)SetParameterData.CXInterval=60;
									else SetParameterData.CXInterval=UartDataDisBuf[16];
									SaveParameterFlag=1;SoundFlag=1;
								break;
//								case 7://0307���������ã�����ֱ��ִ�в����棬Ȼ��ظ�������
//									if(UartDataDisBuf[15]>120||UartDataDisBuf[15]<1)SetParameterData.OnceOzoneTime=10;
//									else SetParameterData.OnceOzoneTime=UartDataDisBuf[15];
//									if(UartDataDisBuf[16]>240||UartDataDisBuf[16]<30)SetParameterData.OzoneInterval=60;
//									else SetParameterData.OzoneInterval=UartDataDisBuf[16];
//									SaveParameterFlag=1;SoundFlag=1;
//								break;
								case 9://0309�豸��������ִ�в��ظ�
									SetParameterData.ParameterLock=UartDataDisBuf[15];SaveParameterFlag=1;
								break;
								case 0x17://0317�豸ͣ��ָ��
									if(UartDataDisBuf[15]==1)//��ʾ��о���������ˣ���Ҫͣ��
									{
										SetParameterData.HaltState=1;ErrorTemp|=0x02;;//�豸��о��������ͣ������״̬ΪE82Uart2Send(SetParameterData.HaltState)
									}
									else {SetParameterData.HaltState=0;ErrorTemp&=0xfd;}//Uart2Send(SetParameterData.HaltState);
									SaveParameterFlag=1;ZSTimeFlag=1;SoundFlag=1;
//									WriteEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar);
//									PReadEeprom(SetParameterAddress,sizeof(SetParameterData),&SetParameterData.FirstChar); //��ϵͳ����
								break;
								case 2://��ȡ��������ָ��0302
									ForwardFlag=1;
								break;
								case 4://���ػ�ָ��0304
									ForwardFlag=1;
									if(UartDataDisBuf[46]&0x04)//����ǿ��ػ�ָ��
									{
										if(UartDataDisBuf[37]==0x01){SendFixedLength(3,UartSendDataBuf);SendFixedLength(4,UartSendDataBuf);if(ShutDown)while(1);}//����ָ��PowerOn=1;WorkStateByte|=0x01;
										if(UartDataDisBuf[37]==0x02){ShutDown=1;}//�ػ�ָ��
										SoundFlag=1;
									}
								break;
								case 0x10://�޸���󵥴γ�ˮ��
									ForwardFlag=1;//�޸���󵥴γ�ˮ��
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
									if(UartDataDisBuf[7]&0x10||BroadcastFlag)//һ�ų�ˮ��ָ��
									{
										SetParameterData.Pluse1=Array_Int(&UartDataDisBuf[15]);SaveParameterFlag=1;
										if(SetParameterData.Pluse1>9999||SetParameterData.Pluse1<100)SetParameterData.Pluse1=450;
									}
									if(UartDataDisBuf[7]&0x20||BroadcastFlag)//һ�ų�ˮ��ָ��
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
									if(UartDataDisBuf[7]&0x10||BroadcastFlag)//һ�ų�ˮ��ָ��
									{
										SetParameterData.CardPrice1=Array_Int(&UartDataDisBuf[15]);
										if(SetParameterData.CardPrice1==0)SetParameterData.CardPrice1=20;
										SetParameterData.VirtualPrice1=Array_Int(&UartDataDisBuf[17]);
										if(SetParameterData.VirtualPrice1==0)SetParameterData.VirtualPrice1=20;
										SaveParameterFlag=1;
									}
									if(UartDataDisBuf[7]&0x20||BroadcastFlag)//һ�ų�ˮ��ָ��
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
//								if(UartDataDisBuf[10]==2||UartDataDisBuf[10]==4||(UartDataDisBuf[10]>=0x10&&UartDataDisBuf[10]<=0x16))//�������Ҫת������ˮ�ڵ�����
//								{
//									memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//�����ݿ���������2�ķ��ͻ�������׼�����͸�������
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
//									if(UartDataDisBuf[7]&0x10||(UartDataDisBuf[7]&0xf0)==0)SendFixedLength(3,UartSendDataBuf);//һ�ų�ˮ��ָ��
//									if(UartDataDisBuf[7]&0x20||(UartDataDisBuf[7]&0xf0)==0)SendFixedLength(4,UartSendDataBuf);//һ�ų�ˮ��ָ��
//								}
								break;
							}
							if(ForwardFlag)//ת������ˮ�ڽ��д���
							{
								ForwardFlag=0;
								if(UartDataDisBuf[7]&0x10||BroadcastFlag)SendFixedLength(3,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);}//һ�ų�ˮ��ָ��
								if(UartDataDisBuf[7]&0x20||BroadcastFlag)SendFixedLength(4,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);}//2�ų�ˮ��ָ��
							}
							if(BroadcastFlag)
							{
								BroadcastFlag=0;UartSendDataBuf[9]=4;if(UartSendDataBuf[10]!=4)UartSendDataBuf[2]=16;//�޸Ļظ��������ͼ����ݳ���,0304�Ļظ���ֱ��ԭ���ݷ���
								SendDataPackage(2,UartSendDataBuf);//�ظ����ݸ�������
							}
						break;
						default:break;
					}
				}
				else//�����ظ���0201��Уʱ�ظ�0203�����ƻظ�0204��״̬�ظ�0205���������ݻظ�1204����ˮͬ���ظ�0207
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
										SaveTimeFlag=1;//ÿ���ϵ�����������һ��ʱ��
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
						case 0x12://1204��1209������������ָ��Ļظ���
							if(UartDataDisBuf[10]==9||UartDataDisBuf[10]==4)ForwardFlag=1;HeartCnt=0;//1204
						break;
						default:break;
					}
					if(ForwardFlag)
					{
						ForwardFlag=0;
						memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);//�����ݿ��������ͻ�����
						if(UartDataDisBuf[7]&0x10||(UartDataDisBuf[7]&0xf0)==0)//һ�ų�ˮ��ָ��
						{
							SendFixedLength(3,UartSendDataBuf);//SendFixedLength(1,UartDataDisBuf);
						}
						if(UartDataDisBuf[7]&0x20||(UartDataDisBuf[7]&0xf0)==0)//2�ų�ˮ��ָ��
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
void Uart3DataDispose()//���ˮ��ͨѶ
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
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3])&&(UartDataDisBuf[7]&0x10))//�豸����ͬ���ҳ�ˮ�ں�Ϊ1��ת����������
				{
					memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);
					SendFixedLength(2,UartSendDataBuf);//��1�ų�ˮ�ڷ��͵�����ת����������
				}
				else
				{
					if(UartDataDisBuf[9]==0x20&&UartDataDisBuf[10]==1)//2001ָ���ǳ�ˮ���������ͬ��ָ��
					{
						OffLineData1=Array_Int(&UartDataDisBuf[23]);//ȡ1�ſڵ�������������
						W_Joint2001Data();
						W_Send2001Data_1();
					}
				}
			}
		}
	}
}
void Uart4DataDispose()//���ˮ��ͨѶ
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
				if((UartDataDisBuf[11]==Cdatas[0])&&(UartDataDisBuf[12]==Cdatas[1])&&(UartDataDisBuf[13]==Cdatas[2])&&(UartDataDisBuf[14]==Cdatas[3])&&(UartDataDisBuf[7]&0x20))//�豸����ͬ���ҳ�ˮ�ں�Ϊ1��ת����������
				{
					memcpy(UartSendDataBuf,UartDataDisBuf,UartDataDisBuf[2]+1);
					SendFixedLength(2,UartSendDataBuf);//��1�ų�ˮ�ڷ��͵�����ת����������
				}
				else
				{
					if(UartDataDisBuf[9]==0x20&&UartDataDisBuf[10]==1)//2001ָ���ǳ�ˮ���������ͬ��ָ��
					{
						OffLineData2=Array_Int(&UartDataDisBuf[23]);//ȡ1�ſڵ�������������
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
	SendDataPackage(3,UartSendDataBuf);//ת����1�ų�ˮ��
}
void W_Send2001Data_2()
{
	Int_Array(SetParameterData.Pluse2,&UartSendDataBuf[17]);
	Int_Array(SetParameterData.CardPrice2,&UartSendDataBuf[19]);
	Int_Array(SetParameterData.VirtualPrice2,&UartSendDataBuf[21]);
	PushZhenNum(UartSendDataBuf);
	UartSendDataBuf[7]=0x20;
	SendDataPackage(4,UartSendDataBuf);//ת����2�ų�ˮ��
}
void W_Send2001Data_3()//���͸���ʾ��
{
	UartSendDataBuf[2]=37;
	UartSendDataBuf[15]=ShowState;//��ʾ�豸״̬
	if(ErrorTemp>1)UartSendDataBuf[16]=ErrorTemp;//����״̬
	else UartSendDataBuf[16]=0;
	UartSendDataBuf[17]=WaterLevelState;//ˮλ״̬
	memcpy(&UartSendDataBuf[18],&SysTime.Hour,4);//����ʱ��
//	UartSendDataBuf[18]=SysTime.Hour;
//	UartSendDataBuf[19]=SysTime.Minute;
//	UartSendDataBuf[20]=SysTime.Week;
	memcpy(&UartSendDataBuf[11],Cdatas,4);//�����豸��
	memcpy(&UartSendDataBuf[22],&UsedWater.ZSYield,9);//����ˮ��
	SendDataPackage(1,UartSendDataBuf);//ת������ʾ��
}
void W_Joint2001Data()
{
	uint8_t i=0;
//	for(i=0;i<50;i++)UartSendDataBuf[i]=0;
	memset(UartSendDataBuf,0,31);
	memcpy(UartSendDataBuf,Str2001,sizeof(Str2001));//�������ݰ�ͷ
	memcpy(&UartSendDataBuf[11],Cdatas,4);//�����豸��
	if(NetWork)UartSendDataBuf[25]=1;//�豸����״̬
	Int_Array(SetParameterData.MaxMoney,&UartSendDataBuf[15]);
	UartSendDataBuf[15]|=0x80;
}
/*
//-----------------------------------------------------------------
//                  C51���ַ�������������            
// ��������    : strsearch ()
// ��������    : ��ָ���������������ҵ���ͬ������
// ��ڲ���   �� ptr2=Ҫ�ҵ�����, ptr1 ��ǰ����
// ���ڲ��� �� 0-û�� �ҵ�   >1 ���ҵ�
//----------------------------------------------------------------
*/
uint8_t strsearch(uint8_t *ptr2,uint8_t *ptr1_at)//���ַ���*ptr2��*ptr1�е�λ��
//����������������ַ���*ptr2�Ƿ���ȫ������*ptr1��
//����:  0  û���ҵ�
//       1-255 �ӵ�N���ַ���ʼ��ͬ
{ uint8_t i,j,k;

  if(ptr2[0]==0) return(0);
  for(i=0,j=0;i<100;i++)
  { if(ptr1_at[i]==ptr2[j])    //�ҵ���һ���ַ�60-2
    { k=i;
      do
      { if(ptr2[j]==0) return(i+1);     //�Ƚ������е��ַ�����ȷ
        if(ptr1_at[k]!=ptr2[j]) break;		//���ȣ��˳�����
        k++;j++ ;																							//�Ƚ���һ��
      } while(k<100) ;//60-2
      j=0;
    }
  }
  return(0);
}	
uint8_t SearchID()     //���ҿ�ID��
{ uint8_t ch1,ch2,Error;

//0123456  7  8  901234567890123456789  0  1  2  345
//AT+CCID\0d\0d\0a898607b6151730454252\0d\0a\0d\0aOK

  Error=1;
  if(strsearch("OK",S2RecBuf))       //����OK
  { ch1=strsearch("CCID",S2RecBuf);  //����CCID
    if(ch1>0)                       //�ҵ�CCID
    { for(ch2=0;ch2<10;++ch2)       //CCID���һس�����
        if(S2RecBuf[ch1+ch2]==0x0d) break;   //����CCID���0d0a
      if(ch2<10) 
      { ch1+=ch2;
        for(ch2=0;ch1<8;++ch2)
          if(S2RecBuf[ch1+ch2]>0x1f) break;    //Ӧ��������,����2f,��ֹ�Ժ���չֻҪ���ڿ����ַ����ǳɹ�
        if(ch2<8)
        { ch1+=ch2;
          if(S2RecBuf[ch1+20]==0x0d)
          { memcpy(SimStr,&S2RecBuf[ch1],20);  //ȡIF����
            SimStr[20]=0;      //��β�ַ�
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
void InitModel()//�����봦������
{ 
	uint8_t ch1,ch2;
	ch1=NetProce&0xf0;       //��λ=1Ѱ��,=2����͸��,=3����IP,=4�ɹ�
  switch(ch1)
	{
		case 0:          //�ϵ��͵����
      ch2=NetProce&0x0f;
			if(ch2==4||ch2==5)
			{
				Uart2SendStr("AT\x0d\x0a");
			}
			switch(ch2)
			{
				case 0:
					Uart2SendStr("+++"); //"+++"֮������500ms�������˳�͸������������
				break;
				case 6:
					AutoReconnection++;
				if(AutoReconnection>=6)//6��û�������ɹ������ģ��ϵ�����
				{
					AutoReconnection=0;
//��ʱ����
					NetProce=0;PowerDown=0;M800Power=0;NetWork=0;//�ϵ��ʱ��
				}
				Uart2SendStr("AT+CIPSHUT\x0d\x0a"); 
				break;
				case 7:
					memset(S2RecBuf,0,110);//��ʼ�����ջ�����
					S2RecCount=0;    //for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;����������  //Ϊ��������׼��  
					Uart2SendStr("AT+CCID\x0d\x0a");
					
				break;
				case 0x0b:
					if(SearchID()==0)     //���ҿ�ID����
					{
						RorS_Flag=0; NetProce=0x10;  //���뷢���������
					}
				break;
				default: break;
			}
			NetProce++;
    break;
    case 0x10:                  //Ѱ��
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //����������
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
        S2RecCount=0;      //Ϊ��������׼��
				Uart2SendStr("AT+CSQ\x0d\x0a");//����Ѱ������
        RorS_Flag=1;
      }
      else
      { RorS_Flag=0;
        ch1=strsearch("+CSQ:",S2RecBuf);								//Ӧ���ܵ�"+CSQ: 26,0"(����26Ϊǿ��,�������10) ��+COPS: 0,0,"CHINA MOBILE"
        if(ch1>0)         //�ҵ�+CSQ
        { if(S2RecBuf[ch1+6]==',')  CommVal=S2RecBuf[ch1+5]-'0';      //����ź�ǿ��ֻ��һλ��
          else
          { CommVal=S2RecBuf[ch1+5]-'0';
            CommVal*=10;
            CommVal+=(S2RecBuf[ch1+6]-'0');
          }
          if((CommVal>7)&&(CommVal!=99))   NetProce=0x20;//�ź�ǿ��Ϊ99��ʾδ֪�򲻿ɲ�
        }
        if(NetProce<0x20)
        { ++NetProce;
          if(NetProce>0x1c) NetProce=0x0;
        }          
      }
      break;
    case 0x20:                     //͸��
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //����������
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
//				RestData(S2RecBuf);
        S2RecCount=0;      //Ϊ��������׼��
				Uart2SendStr("AT+CIPMODE=1\x0d\x0a");//����͸������
        RorS_Flag=1;
      }
      else
      { RorS_Flag=0;
        if(ch1=strsearch("OK",S2RecBuf))	NetProce=0x30;	//Ӧ���յ�OK
        else//�ȴ�
        { ++NetProce;
          ch2=NetProce%0x10;
          if(ch2>2) NetProce=0;
        }          
      }
      break;
    case 0x30:                       //��IP
      if(RorS_Flag==0)
      { //strcpy(UartSendDataBuf,"AT+CIPSTART=\"TCP\",\"39.106.74.186\",9999\x0d\x0a"); S2TransLen=40; //����IP
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
        memcpy(&UartSendDataBuf[ch1],&IportBuf[22],5);    //�˿�
        ch1+=5;UartSendDataBuf[ch1]=0x0d;
        ++ch1;UartSendDataBuf[ch1]=0x0a;
        ++ch1;
        S2TransLen=ch1;	
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
        S2RecCount=0;      //Ϊ��������׼��
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
					NetProce=0;							//Ӧ���յ�CONNECT
          RorS_Flag=0;  break;
				}
				else if(ch1=strsearch("CONNECT\x0d\x0a",S2RecBuf))
        {	NetProce=0x40;							//Ӧ���յ�CONNECT
          RorS_Flag=0;   
					AutoReconnection=0;//͸�����óɹ�����������������		
//					memset(S1TransBuf,0,50);//��ʼ������1���ͻ�����
//					memcpy(S1TransBuf,Str301,11);  //�豸�������ݰ�֡ͷ
//					S1TransBuf[11]=0x01;PushCRC(S1TransBuf); 
//					SendFixedLength1(S1TransBuf);//�����豸����ָ������壬������Ҫ�ϴ���½����״̬��
					if(YuanipFlag)          //�������IP
          { YuanipFlag=0;IportBuf[27]=0x5a;
            WriteEeprom(SysIpPort,28,IportBuf); 
          }
        }
        else
        { 
//					if(SecondByte%2) 
					++NetProce;
          ch2=NetProce%0x10;
          if(ch2>8)//��ָ��������Ӧʱ��Ϊ9��
          { NetProce=0;
            if(YuanipFlag)
            { ++NewIPTime;
              if(NewIPTime>3) while(1);    //���3�β��ɹ�,����ת����IP
            }
          }
        }          
      }
      break;
		case 0x40:                       //��½,��Sim���ִ�
      if(RorS_Flag==0)
      { 
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
				memset(UartSendDataBuf,0,50);//��ʼ�����ջ�����
        memcpy(UartSendDataBuf,Str102,11);  //��½��ͷ
        memcpy(&UartSendDataBuf[11],Cdatas,4);  //�豸��
        UartSendDataBuf[4]=SheClass;               //�豸����	���ˮ����ˮ��ʱ��Ʒ���
        memcpy(&UartSendDataBuf[15],SimStr,20);    //Sim�ִ�   
				OffLineData=OffLineData1+OffLineData2;
				Int_Array(OffLineData,&UartSendDataBuf[37]);
        NetOverTime=0;     //���¿�ʼ������ʱ
        S2RecCount=0;      //Ϊ��������׼��
//        PushCRC(UartSendDataBuf);   
//				SendFixedLength(UartSendDataBuf);
				PushZhenNum(UartSendDataBuf);
				SendDataPackage(2,UartSendDataBuf);
        RorS_Flag=1;NetProce&=0xf0;
//				Uart1Send(0xa1);
      }
      else
      { 
				if((S2RecBuf[0]==0x7e)&&(S2RecBuf[9]==2)&&(S2RecBuf[10]==2)&&(CompCRC(S2RecBuf)==0))   //����ǻظ���202
        {	NetProce=0x50;	NetWork=1;							//
          NetOverTime=0;     //���¿�ʼ������ʱ
					TimingFlag=1;HeartFlag=1;ZSTimeFlag=1;//ParameterFlag=1;�ϵ�������������͵����ݶ��ϴ�һ��		
          if(YuanipFlag)          //�������IP
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
              if(NewIPTime>3) while(1);    //���3�β��ɹ�,����ת����IP
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


//���Ͷ������ݣ��������ֽڱ�ʾ����
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
	PushCRC(Str);//����CRC
	SendFixedLength(uartx,Str);
}

//�������з���CRC-----------------------
void PushCRC(uint8_t Str[])
{ uint8_t ch1;
  uint16_t int1;
  ch1=Str[2];    //����
  ch1-=2;        //CRC����
  int1=CRC_Calculate(ch1,&Str[1]);  //����CRC
  ++ch1;        //ָ���һ��CRC
  Str[ch1]=(uint8_t)(int1/0x100);
  ++ch1;
  Str[ch1]=(uint8_t)(int1%0x100);
}
//�������з���֡���--------------------------
void PushZhenNum(uint8_t Str[])
{  Str[5]=(uint8_t)(ZhenNum/0x100);
   Str[6]=(uint8_t)(ZhenNum%0x100);
   ++ZhenNum;            //��һ�ε�֡��
}
//=======================================
//����CRC------------------------------------
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
//�Ƚ����ݰ��е�CRC-----------------------
bit CompCRC(uint8_t Str[])
{ uint8_t ch1,ch2;
  uint16_t int1;

  if(Str[0]!=0x7e) return 1;  //�����ͷ����
  ch1=Str[2];                 //���͵ĳ���
  ch1-=2;                     //CRC�ĳ���
  int1=CRC_Calculate(ch1,&Str[1]); //����CRC
  ch2=(uint8_t)(int1/0x100);    //CRC���ֽ�
  ++ch1;                      //ָ��CRC��һ�ֽ�
  if(ch2!=Str[ch1])  return 1; //CRC��һ�ֽ�δͨ��
  ++ch1;
  ch2=(uint8_t)(int1%0x100);     //CRC���ֽ�
  if(ch2!=Str[ch1])  return 1; //CRC��2�ֽ�δͨ��
  return 0;
}

void ReceiveTimeOut()//���ճ�ʱ����115200�����ʣ�11520�ֽ�/��,û������Դ���11���ֽ�
{
	if(S1ReceiveFlag)//��������ϵĳ�ʱʱ��Ӧ�ô�������������ʱ�䣬����ӳ��֣��ϴε����ݻ�δ�����꣬�������ݽ�������ˣ��ͻ�Ѻ�����յ�������©����
	{
		S1Receive1ms++;
		if(S1Receive1ms>10)//�������3���붼û�н��յ����ݵĻ�����Ϊ���ݽ�����ϣ���Ҫ��������ϱ�־������λ�����������������ݴ���
		{
			S1Receive1ms=0;S1ReceiveFinish=1;S1ReceiveFlag=0;S1ReceiveLength=0;//S1DataLength=0;S1HeadFlag=0;
		}
	}
	else S1Receive1ms=0;
//	if(S2ReceiveFlag)//��������ϵĳ�ʱʱ��Ӧ�ô�������������ʱ�䣬����ӳ��֣��ϴε����ݻ�δ�����꣬�������ݽ�������ˣ��ͻ�Ѻ�����յ�������©����
//	{
//		S2Receive1ms++;
//		if(S2Receive1ms>100)//�������3���붼û�н��յ����ݵĻ�����Ϊ���ݽ�����ϣ���Ҫ��������ϱ�־������λ�����������������ݴ���
//		{
//			S2Receive1ms=0;if(NetWork){S2HeadFlag=0;S2ReceiveFinish=1;S2PacketNum=0;S2ReceiveLength=0;S2ReceiveFlag=0;S2DataLength=0;}
//		}
//	}
//	else S2Receive1ms=0;

		if(S2ReceiveFlag)       //������ڽ���
    { ++S2Receive1ms;
      if(S2Receive1ms>10)   
      { S2ReceiveFinish=1;S2PacketNum=0;S2ReceiveFlag=0;S2ReceiveLength=0;S2Begin=0;}
    }
		else S2Receive1ms=0;
		if(S3ReceiveFlag)       //������ڽ���
    { ++S3Receive1ms;
      if(S3Receive1ms>10)   
      { S3ReceiveFinish=1;S3PacketNum=0;S3ReceiveFlag=0;S3ReceiveLength=0;S3Begin=0;}
    }
		else S3Receive1ms=0;
		if(S4ReceiveFlag)       //������ڽ���
    { ++S4Receive1ms;
      if(S4Receive1ms>10)   
      { S4ReceiveFinish=1;S4PacketNum=0;S4ReceiveFlag=0;S4ReceiveLength=0;S4Begin=0;}
    }
		else S4Receive1ms=0;
//	if(S3ReceiveFlag)//��������ϵĳ�ʱʱ��Ӧ�ô�������������ʱ�䣬����ӳ��֣��ϴε����ݻ�δ�����꣬�������ݽ�������ˣ��ͻ�Ѻ�����յ�������©����
//	{
//		S3Receive1ms++;
//		if(S3Receive1ms>100)//�������3���붼û�н��յ����ݵĻ�����Ϊ���ݽ�����ϣ���Ҫ��������ϱ�־������λ�����������������ݴ���
//		{
//			S3Receive1ms=0;S3HeadFlag=0;S3ReceiveFinish=1;S3PacketNum=0;S3ReceiveLength=0;S3ReceiveFlag=0;S3DataLength=0;
//		}
//	}
//	else S3Receive1ms=0;
//	if(S4ReceiveFlag)//��������ϵĳ�ʱʱ��Ӧ�ô�������������ʱ�䣬����ӳ��֣��ϴε����ݻ�δ�����꣬�������ݽ�������ˣ��ͻ�Ѻ�����յ�������©����
//	{
//		S4Receive1ms++;
//		if(S4Receive1ms>100)//�������3���붼û�н��յ����ݵĻ�����Ϊ���ݽ�����ϣ���Ҫ��������ϱ�־������λ�����������������ݴ���
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
				if(S2Begin==3)              //4������
				{ S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
					--S2DataLength;
					if(S2DataLength==0)
					{ S2Begin=0;S2ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
						S2PacketNum++;if(S2PacketNum>7)S2PacketNum=0;//������һ�����ݣ���һ������ڽ��ջ������һ��
						return;
					}
				}
				if(S2Begin==2)               //3���ҳ���
				{ ++S2Begin; S2DataLength=DATA_uart-2; 
					S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
				}
				if(S2Begin==1)               //2����0
				{ if(DATA_uart==0)
					{ ++S2Begin;
						S2RecDataBuf[S2PacketNum][S2ReceiveLength++]=DATA_uart;
					}
					else 
					{ S2Begin=0;S2ReceiveLength=0;}
				}
				if(S2Begin==0)                //1������7e
				{ if(DATA_uart==0x7e) 
					{ S2Begin=1;S2ReceiveLength=0;
						S2RecDataBuf[S2PacketNum][S2ReceiveLength]=DATA_uart; ++S2ReceiveLength;
					}
				}
				S2ReceiveFlag=1;S2Receive1ms=0;      //���ձ�־���ַ��м�ļ������,Ϊ�޴����ַ��˳���׼��
				if(S2ReceiveLength>=49) S2ReceiveLength=0;//������յ���ͨѶ���ݣ���ÿ��������50���ֽ�
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
			if(S3Begin==3)              //4������
			{ S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
				--S3DataLength;
				if(S3DataLength==0)
				{ S3Begin=0;S3ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
					S3PacketNum++;if(S3PacketNum>7)S3PacketNum=0;//������һ�����ݣ���һ������ڽ��ջ������һ��
					return;
				}
			}
			if(S3Begin==2)               //3���ҳ���
			{ ++S3Begin; S3DataLength=DATA_uart-2; 
				S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
			}
			if(S3Begin==1)               //2����0
			{ if(DATA_uart==0)
				{ ++S3Begin;
					S3RecDataBuf[S3PacketNum][S3ReceiveLength++]=DATA_uart;
				}
				else 
				{ S3Begin=0;S3ReceiveLength=0;}
			}
			if(S3Begin==0)                //1������7e
			{ if(DATA_uart==0x7e) 
				{ S3Begin=1;S3ReceiveLength=0;
					S3RecDataBuf[S3PacketNum][S3ReceiveLength]=DATA_uart; ++S3ReceiveLength;
				}
			}
			S3ReceiveFlag=1;S3Receive1ms=0;      //���ձ�־���ַ��м�ļ������,Ϊ�޴����ַ��˳���׼��
			if(S3ReceiveLength>=49) S3ReceiveLength=0;//������յ���ͨѶ���ݣ���ÿ��������50���ֽ�
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
		if(S4Begin==3)              //4������
		{ S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
			--S4DataLength;
			if(S4DataLength==0)
			{ S4Begin=0;S4ReceiveLength=0;//S2ProceFlag=1;S2RecFlag=0;
				S4PacketNum++;if(S4PacketNum>7)S4PacketNum=0;//������һ�����ݣ���һ������ڽ��ջ������һ��
				return;
			}
		}
		if(S4Begin==2)               //3���ҳ���
		{ ++S4Begin; S4DataLength=DATA_uart-2; 
			S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
		}
		if(S4Begin==1)               //2����0
		{ if(DATA_uart==0)
			{ ++S4Begin;
				S4RecDataBuf[S4PacketNum][S4ReceiveLength++]=DATA_uart;
			}
			else 
			{ S4Begin=0;S4ReceiveLength=0;}
		}
		if(S4Begin==0)                //1������7e
		{ if(DATA_uart==0x7e) 
			{ S4Begin=1;S4ReceiveLength=0;
				S4RecDataBuf[S4PacketNum][S4ReceiveLength]=DATA_uart; ++S4ReceiveLength;
			}
		}
		S4ReceiveFlag=1;S4Receive1ms=0;      //���ձ�־���ַ��м�ļ������,Ϊ�޴����ַ��˳���׼��
		if(S4ReceiveLength>=49) S4ReceiveLength=0;//������յ���ͨѶ���ݣ���ÿ��������50���ֽ�
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
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xCC;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	ES=1;
}
void W_Uart2Config(void)//24Mhz
{
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xCC;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	IE2 |= 0x01;
}
void W_Uart3Config(void)//24Mhz
{
	S3CON = 0x10;		//8λ����,�ɱ䲨����
	S3CON &= 0xBF;		//����3ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xCC;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
  IE2 |= 0x08;
}
void W_Uart4Config(void)//24Mhz
{
	S4CON = 0x10;		//8λ����,�ɱ䲨����
	S4CON &= 0xBF;		//����4ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xCC;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
  IE2 |= 0x10;	//��������4�ж�
}
void W_UartConfig(void)
{
	W_Uart1Config();
	W_Uart2Config();
	W_Uart3Config();
	W_Uart4Config();
}
//-------------PCF8563-----------
void delay()    //��΢��ʱ  6usԼ>4.7us
{
_nop_();
  _nop_();
   _nop_();
    _nop_();
         _nop_();
          _nop_();
}

void iic_start()   //�����ź�
{
	SDA_clock=1;
	SCL_clock=1;
	delay();
	SDA_clock=0;
	delay();
	SCL_clock=0;
}

void iic_stop()  //ֹͣ�ź�
{
	SDA_clock=0;
	SCL_clock=1;
	delay();
	SDA_clock=1;
	delay();
	SCL_clock=0;
}

void iic_ack()   //Ӧ���ź�
{
	uint8_t i=0;
	SCL_clock=1;
	delay();
	while((SDA_clock==1)&&(i<255))
	i++;
	SCL_clock=0;
	delay();
}

void iic_send_byte(uint8_t bat)  //��������
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

uint8_t iic_rev()  //��������
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

void iic_send_add_byte(uint8_t add,uint8_t bat) //��ĳ����ַ����ĳ����
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

uint8_t iic_rec_add_byte(uint8_t add) //��ĳ����ַ��������
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

//-----------ʱ��Ԥ�趨ֵ---------uint8_t Sec,
void time_init(uint8_t Year,uint8_t Moom,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec,uint8_t Week)
{
	Sec=dec_bcd(Sec);Min=dec_bcd(Min);Hour=dec_bcd(Hour);Day=dec_bcd(Day);Moom=dec_bcd(Moom);Year=dec_bcd(Year);
	if(Week==7)Week=0;
	iic_send_add_byte(0x02,Sec);         //50��
	iic_send_add_byte(0x03,Min);         //59����    
	iic_send_add_byte(0x04,Hour);        //19Сʱ             
	iic_send_add_byte(0x05,Day);  		//1��
	iic_send_add_byte(0x06,Week);  		//����
	iic_send_add_byte(0x07,Moom);      //10��  20���͵�
	iic_send_add_byte(0x08,Year);      //2017��
}
//----------------PCF8563��ʼ��-----------------
void pcf_init()
{
	iic_send_add_byte(0x00,0x00);  //����ʱ��
}
uint8_t dec_bcd(uint8_t bat)
{
	uint8_t ch1;
	ch1=bat/10*0x10+bat%10;
	return ch1;
}
//----------------BCDת10����-----------
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
	SysTime.Second=bcd_dec(0x7f&iic_rec_add_byte(0x02));    //��ȡ��
	SysTime.Minute=bcd_dec(0x7f&iic_rec_add_byte(0x03));   //��ȡ���Ӳ�ת��Ϊʮ������ʾ��
	SysTime.Hour=bcd_dec(0x3f&iic_rec_add_byte(0x04));  //��ȡСʱ��ת��Ϊʮ������ʾ��
	SysTime.Day=bcd_dec(0x3f&iic_rec_add_byte(0x05));   //��ȡ������ת��Ϊʮ������ʾ��
	SysTime.Week=bcd_dec(0x07&iic_rec_add_byte(0x06));   //��ȡ���ڲ�ת��Ϊʮ������ʾ��
	if(SysTime.Week==0)SysTime.Week=7;
	SysTime.Month=bcd_dec(0x1f&iic_rec_add_byte(0x07));  //��ȡ�²�ת��Ϊʮ������ʾ��
	SysTime.Year=bcd_dec(0xff&iic_rec_add_byte(0x08));    //��ȡ�겢ת��Ϊʮ������ʾ��
	if(SysTime.Second>59||SysTime.Minute>59||SysTime.Hour>23||SysTime.Day>31||SysTime.Month>12||SysTime.Year>50){SysTime.Year=21;SysTime.Month=5;SysTime.Day=20;SysTime.Hour=0;SysTime.Minute=0;SysTime.Second=0;SysTime.Week=4;}//���ʱ�䲻��ȷ����ʱ���ʼ��Ϊ20��12��1��0ʱ
}
//-------------PCF8563-----------