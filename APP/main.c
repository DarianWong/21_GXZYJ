
#include "head.h"
//#define TestServer
#ifdef	TestServer
uchar code Domain[]={"\x015testmanage.wateree.cn"};
uchar code Port[]={"09999"};
#else
uchar code Domain[]={"\x00cs.waterer.cn"};  //����,��һ�ֽ�Ϊ��������http://testmanage.waterer.cn/
uchar code Port[]={"09999"};
//uchar code Domain[]={"\x013manage.waterchip.cn"};  //����,��һ�ֽ�Ϊ��������http://testmanage.waterer.cn/
//uchar code Port[]={"09999"};
#endif
uchar SoundNum=0;SoundFlag=0;
//-------------TDS------------
struct
{ uint  TDSVal;      //TDSֵ
  uint  TDSPlus;     //TDSֵ��Ӧ������ֵ
  uchar  TDSper;      //ÿ��TDS��Ӧ������
}
//code TdsData[6]={{0,0,0},{16,355,22},{50,1051,20},{100,1795,15},{200,3215,14},{300,4236,10}};//TdsData[6]={{0,0,0},{16,933,58},{50,2550,48},{100,4414,37},{200,7046,26},{300,8640,16}};  
//code TdsData[6]={{0,0,0},{26,854,33},{50,1487,26},{100,2761,25},{200,4531,18},{300,5928,14}};
code TdsData[6]={{0,0,0},{16,537,34},{50,1487,28},{100,2761,25},{200,4531,18},{300,5928,14}};
uint TDS1=0,TDS2=0;//TDS������ˮ TDS����ԭˮ ,,TDS3=0,TDS4=0	   TDS_formerly=0
uint TDS_array[2]={0};//={0,0}; //ԭˮTDS ��ˮTDS	 
uchar TDStime1=0,TDStime2=0;//����tds������ 	time100_ms2=0,,TDStime3=0,TDStime4=0
bit TDS1flag=0,TDS2flag=0;			// TDS_CS_flag=0;//,,TDS3flag=0,TDS4flag=0
uint CalTDS(uint PlusNum)//����������PlusNum������TDSֵ������ 
{ uchar ch1,ch2;
  uint int1;
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
//-------------TDS------------

xdata union ActDateGE //--------------ϵͳ�̶����ݽṹ
 { uchar DataStr[35];    //��Eprom  24B
  struct
  { uchar FirstChar;   //��ͷ      1B
		uint AreaCode;  //������   2B
//--------------------------------------------  
		uint YuKou;  		//Ԥ��1								2B		1A1
		uint Pluse;      //����     						2B     1A2  1-250   100   �� 10ml��Ӧ��������,��ʱ�䵱��
    uint CoinPluse;//Ӳ���������Ӧ�����		2B
		uint CardPrice;		//һ�ſ�ˢ������			2B
		uint VirtualPrice;//ɨ�뵥��						2B
		uint CoinPrice;		//Ͷ�ҵ���						2B
		
		uchar MaxCoin;			//����������ѽ��		1B
		uchar ParameterLock;//������
		uchar FlowRate1;		//ˢ����ˮ�ٶ�						1B
		uchar FlowRate2;		//ɨ���ˮ�ٶ�						1B
		uchar FlowRate3;		//Ͷ�ҳ�ˮ�ٶ�						1B
		uchar	Confiscate;		//û��ʱ��				1B
		uchar LedLock;		//������ʱ							1B
		uchar	PumpDelay;	//��ˮ��ˮ����Һλ�Ͽ�	1B
		uchar OnceCXTime;	//���γ�ϴʱ��					1B
		uchar	CXInterval;	//��ϴ���							1B
		uchar OnceOzoneTime;//��������ʱ��				1B
		uchar	OzoneInterval;//�����������ʱ��		1B
		uchar	StartHeat;//��ʼ�����¶�						1B
		uchar StopHeat;//ֹͣ�����¶�						1B
		uchar HeatTime;//�೤ʱ��δ�ﵽֹͣ�¶ȿ�������ԡ�Ե�	1B
		uchar	StartLEDHour;//���ƿ���ʱ����		1B
		uchar	StartLEDMinute;//���ƿ���������	1B
		uchar	StopLEDHour;		//���ƹر�ʱ����		1B
		uchar	StopLEDMinute;//���ƹرշ�����		1B
    uchar LastChar;    //��β      1B
  }DataNum;
}xdata EpromDataGu;

ulong xdata EquipmentNumber=0;//�豸��
uint Sys1ms=0;
bit Sys10msflag=0,Sys100msflag=0,Sys1sflag=0,Sys1mflag=0,SecFlag=0,Sys200msFlag=0,NetTimeFlag=0;
uchar Parameter1s=0,SellWater10ms=0;//��ˮ��ʱ�������ü�ʱ������ˮ�������ʱ���ÿ�һ��Ǯ
uint	SellWater1ms=0;
//�����ⲿ��
uint code Thermomete[101]=
{ 782, 774, 765,755,746,737,727,717,707,696,686,675,664,653,642,631,619,608,596,584,		//0~19
	572,560,548,536,524,512,500,489,478,467,456,446,435,425,414,404,394,384,374,365,		//20~39
	355,346,337,328,319,310,302,294,286,278,270,263,256,248,241,235,228,221,215,209,		//40~59
	203,197,192,186,181,176,171,166,161,156,152,148,143,139,135,131,128,124,120,117,		//60~79
	114,111,107,104,101,99,96,93,90,88,85,83,81,79,76,74,72,70,69,67,		//80~99
65};					//100	
uchar xdata Temperature=0;
bit TemperatureOk=0;
uchar xdata KeyValue=0;//��ֵ��bit7�����ã�bit6:���ü���bit5:�Ӽ���bit4:������bit3:ֹͣ2����bit2:��ʼ2����bit1:ֹͣ1����bit0:��ʼ1��
bit LowLevel_present=0,LowLevel_last=0,LowLevel_ok=0,HighLevel_present=0,HighLevel_last=0,HighLevel_ok=0,LowTension_present=0,LowTension_last=0,LowTension_ok=0;
uchar xdata LowLevel_num=0,HighLevel_num=0,LowTension_num=0;
bit SZFlag=0,KeyFlag=0,KeyChange=0,GZKeyFlag=0,ZTKeyFlag=0,ZJKeyFlag=0,JSKeyFlag=0,SZKeyFlag=0,AreaCodeKey=0,TestKeyFlag=0;
bit DebouncingOk=0;
uchar Debouncing1ms=0;
bit ZSFlag,MSFlag=0,QSFlag=0,CXFlag=0;
//bit	ZSTimeFlag=0;//ͬ����ˮʱ��ָ��
uchar ZSTime1s=0,ZSTime1m=0,CXInterval1m=0;//��ˮ���ʱ�����ּ�ʱ������ϴ�����ʱ��
uint ZSTime1h=0;//��ˮʱ��ʱ��
uchar WashTime=0;//��ϴʱ��
uchar OzoneTime=0,OzoneTime1s=0,OzoneTime1m=0;
uint OutWater1s=0,OutWater10ms=0;//��ˮ��ʱ������ˮʱ���������õ��ڸ�Һλ���ˮ�೤ʱ�俪ʼ��ˮ
bit Vendibility=0;//���۱�־
uchar Vendibility1s=0;//���ۼ�ʱ����ˮλ�����Һλ�������ˮ180���Ϊ����״̬
uchar StateLed=0;//״̬��
uchar ErrorCode=0;//���ϴ���
bit LevelError=0;//Һλ���ϱ�־
uchar WaterLevel=0;//ˮλ״̬

bit ShutDown=0,PowerOn=0;//�ػ���־
bit SaveSetData=0;//�������ò�����־
uchar SetCnt=0;
uchar AreaCodeKey100ms=0,AreaCnt=0,ShowEquipmentTime=0,TestKey100ms=0;//�����밴����ʱ��,������λ��ѡ��
uchar Flicker100ms=0,Flicker10ms=0;//��˸ʱ���ʱ��
bit FlickerFlag=0;//��˸��־
bit AutoChange=0,SetTimeFlag=0,NightFlag=0;
uchar AutoChange1ms=0,AutoExit=0;//�Զ��仯��ʱ��
bit HeatFlag=0;//���ټ��ȱ�־,SpeedHeat=0
uchar T100ms=0;//���ȼ�ʱ��
uint HeatTime1s=0;
uint TempertureTemp=0;//�¶�

uchar SellState=0;//��ˮ����
bit TBFlag=0;//Ͷ�������壬Ͷ�Ҵ�ˮ��־	
bit CoinFlag=0,CardFlag=0,AnliangFlag=0,RemoteFlag=0;//����ȡˮ��־
bit MaxMoneyFlag=0,ConfiscateFlag=0;//������ѽ���־
uchar Confiscate=0,LedLockTime=0;//û��ʱ�䣬������ʱ
uint Balance=0,KouMoney=0,Expenditure=0;//���,�ۿ���,�������ѽ��
uint WaterPrice=0;//��ˮ����
ulong ShowMoney=0;//��ʾ���
uchar CardAreaCode[2]={0};//Ĭ��������Ϊ9999
uchar CardBuf[16]={0};//������
uchar UID[4]={0};//��ID
uchar CardSer[4]={0};//����Ŀ�UID
uchar CardCode[4]={0};//����
uchar RecCardCode[4]={0};//���տ���
uchar BlackCard[4]={0};//�ڿ�����
uchar CardKey[6]={0x03,0x67,0x04,0x01,0x05,0x09};//������
//uchar CardKey[6]={0x03,0x67,0x04,0x67,0x05,0x09};//������
//uchar code CardKey[6]={0x20,0x15,0x05,0x04,0x08,0x30};//̫��������
uchar CardType=0,RechargeType=0,BlackCardType=0,RecCardType=0;//������,���տ�����
bit RechargeOk=0,RechargeFlag=0,BlackCardFlag=0,OnceRead=0,WriteCardOk=0,YuKouOk=0,ReadCardData=0;//��ֵ�ɹ���־,�ڿ���־�����ζ�����־����������ֻ��һ�ο�
ulong CardMoney=0,RechargeMonney=0,RechargeFinishMoney=0,TotalMoney=0;//���ڽ��,��ֵ���,��ֵ��Ͻ��


//���ڲ���
//��������
uchar xdata EpromData[7];     //Eprom�зŵĹ�������

bit ReadSendflag=0;//��Ҫ������������������־���˱�־��λ����Ҫ��ȡ��Ҫ���͵��������ݵ�ַ�������ݣ������ȡ��Ҫд����������ݵ�ַ��������
////---------------------------
//bit SendLishiFlag;       //��Ҫ������ʷ����
//bit SendLiWait;       //������ʷ��־ʱ�ĵȴ��ظ���־
//uint LiTimerUnit;     //�����ʱ��
////#define LishiTimer  30*60   //�����ʱ��30����
//bit ShanOverFlag=0;
//uchar JiluPoint=0;
uchar ShanBakNum;          //�������ʷ��¼����λ�ñ���
uchar LuBakNum;            //�������ʷ��¼�������е�λ�ñ���
//uchar S2ReTime;      //ͬһ����¼�������ʹ���
//uint xdata ZhenNum=0;//֡��
uchar S2ingTimer;    //S2��ʱ����һ����¼�ĵȴ�ʱ��
uchar JiluTemp[17]; //��������Ѽ�¼�������ݰ����õ�����:֡��4+����5+ʱ��6+�������1+����ǰ���2+���ѽ��2=20
uchar ZhenBakH=0,ZhenBakL=0;//

xdata union ActDateLS //�������ʷ��¼���ݽṹ
{ uchar DataByte[512];    //��Eprom  11B
  struct
  { uchar CardCode[4]; //����              4B
    uchar TimeByte[6]; //ʱ��:������ʱ���� 6B
    uchar TypeFlag;    //���:HΪ�������,LΪ������ 1B
    uchar OldMoney[3]; //����ǰ���        3B
		uchar	Cardtype;		//������							1B
    uchar YMoney[2];   //�������ѽ��      2B
  }DataNum[30];
}xdata RamDataLS;
//����4+ʱ��6+�������1+����ǰ���3++������1+���ѽ��2=17
//��������
bit YuanipFlag=0;
uchar NewIPTime=0;
uchar IportBuf[30]={0};
bit SaveFull=0,SendLishiFlag=0,SendLiWait=0,SaveLiData=0;//��ʷ���ݴ�����־
uint LiTimerUnit;     //�����ʱ��
bit ZhenNumFlag=0;
uint ZhenNum=0;
uchar SendShanNum=0,SendLuNum=0,ShanNum=0,LuNum=0;//��ʷ���ݷ��͵���������//��ʷ���ݷ��͵�����//��֮ǰ�ĵ��ŷ����������ʷ��¼����λ��,�������ʷ��¼�������е�λ��
uchar EpromData[7];     //Eprom�зŵĹ�������
uchar CommVal=0;      //�ź�ǿ��
uchar NetOverTime=0;     //���¿�ʼ������ʱ
uchar S2TransBuf[110];//���ͻ�����
uchar S2RecBuf[110];//���ݽ��ջ���
uchar S2RecDataBuf[8][50];//���ݽ��ջ���
uchar S2RecNum=0,S2DisposeNum=0;//���ڽ�����������,�������ݴ�������
uchar S2RecCount=0,S2DataLen=0,S2TransLen=0;
uchar S2Begin=0;
uchar S2ProceFlag=0;//���ݽ�����ɱ�־
uchar S2ingTimer,S2ReTime;    //S2��ʱ����һ����¼�ĵȴ�ʱ��//ͬһ����¼�������ʹ���
uchar NetProce=0,AutoReconnection=0;//��������״̬��1-5,�Զ�����
uchar S2RecFlag=0,S2CharSpace=0;//���ڽ��ձ�־���ַ���������ڽ��ճ�ʱ
bit PowerDown=0;//ģ��ϵ��־
uchar PowerDown1s=0,SaveLiData1s=0;//�ϵ��ʱ��
bit RorS_Flag=0,NetWork=0;//
bit busy=0,busy2=0;
bit TimingFlag=0,StateFlag=0,HeartFlag=0,Order0104=0;//ʱ��ͬ��ָ���־��״̬����־����������־,
bit ParameterFlag=0,SetPriceFlag=0;//����ͬ��ָ���־,���õ��۱�־�������������֮���Ͳ���ͬ��ָ��
bit ZSTimeFlag=0;//��ˮʱ��ͬ����־
uchar ParameterTime=0,StateTime=0,HeartTime=0;//�豸�������ͼ����ʱ����״̬����ʱ������������ʱ��
uchar HeartCnt=0;//����������������6�����ݰ�û���յ��ظ�����Ϊ��������Ҫ��������
uchar WorkStateByte=0,WorkStateLast=0;//�豸����״̬�ֽ�
uchar SendDataInterval=0;
uchar TestAn=0;
bit TestFlag=0;
ulong xdata TotalWater=0;//��ˮ��
uchar WaterFlowRate=0,DelayShowTime=0;
uint ShowWater=0;//��ʾˮ��
//���ڲ���

#include "rc522.h"
#include "rc522.c"
#include "sysinit.c"
#include "uart.c"
#include "check.c"

void main()
{
	SysInit();
	RFIDinit();
	CloseOutput();
	Update_Display();
//	Init_Display();//��ʼ��TM1640
	PReadEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //��ϵͳ����
	if(EpromDataGu.DataNum.FirstChar!=0x55||EpromDataGu.DataNum.LastChar!=0xaa)//����ͷ��β����ȷ��ѳ�ʼ���ݱ�������
	{
		EpromDataGu.DataNum.FirstChar=0x55;               //��ͷ
    EpromDataGu.DataNum.LastChar=0xaa;                //��β
		EpromDataGu.DataNum.AreaCode=9999;
		EpromDataGu.DataNum.YuKou=150;
		EpromDataGu.DataNum.Pluse=540;
		EpromDataGu.DataNum.CoinPluse=50;
		EpromDataGu.DataNum.CardPrice=400;
		EpromDataGu.DataNum.VirtualPrice=400;
		EpromDataGu.DataNum.CoinPrice=400;
		EpromDataGu.DataNum.MaxCoin=5;
		EpromDataGu.DataNum.ParameterLock=0;//������������0Ϊ�������豸���߷��������޸��豸������1Ϊ��������������
		EpromDataGu.DataNum.FlowRate1=50;//һ�ų�ˮ��ˮ���ٶ�
		EpromDataGu.DataNum.FlowRate2=50;
		EpromDataGu.DataNum.FlowRate3=50;
		EpromDataGu.DataNum.Confiscate=180;//û��ʱ����ʱ��Ϊ20��
		EpromDataGu.DataNum.LedLock=30;
		EpromDataGu.DataNum.PumpDelay=10;
		EpromDataGu.DataNum.OnceCXTime=10;
		EpromDataGu.DataNum.CXInterval=60;
		EpromDataGu.DataNum.OnceOzoneTime=10;
		EpromDataGu.DataNum.OzoneInterval=60;
		EpromDataGu.DataNum.StartHeat=2;
		EpromDataGu.DataNum.StopHeat=7;
		EpromDataGu.DataNum.HeatTime=5;
		EpromDataGu.DataNum.StartLEDHour=19;
		EpromDataGu.DataNum.StartLEDMinute=30;
		EpromDataGu.DataNum.StopLEDHour=23;
		EpromDataGu.DataNum.StopLEDMinute=30;
		WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //����
//20201124�޸���ˮʱ���ϴ�������о��������		
		ZSTime1h=0;SaveZStime();//������ˮʱ��
//		DataB_A();
//		WriteEeprom(SetDataAdd,38,SetParameter);
		time_init(0,0,8,29,4,20);
		memcpy(IportBuf,Domain,Domain[0]+1);     //����
    memcpy(&IportBuf[22],Port,5);
    WriteEeprom(SysIpPort,27,IportBuf); 
		SoundFlag=3;
	}
	else
	{
		PReadEeprom(SysIpPort,27,IportBuf);
		ZSTime1h=IapReadByte(ShuiSector)*0x100+IapReadByte(ShuiSector+1);//��ȡ��ˮʱ��
		SoundFlag=1;
	}
	AreaIntToChar();
	ReadEquipment();WDT_CONTR=0x35;
	if(LowTension==0&&HighLevel==0)WashTime=EpromDataGu.DataNum.OnceCXTime;//�ϵ����ѹ���رպϣ���ˮλ��ˮ����򿪳�ϴ��
//	ZSTime1h=2;
	while(1)
	{
//		si=1;
		if(ShutDown)//������ڹػ�״̬
		{
			memset(DisBuf,0,16);Update_Display();WorkStateByte&=0xfe;if(PowerOn==0)WDT_CONTR=0x35;//�ر���ʾ
			CloseOutput();//�ر��������
		}
		else
		{
			WorkStateByte|=0x01;PowerOn=0;WDT_CONTR=0x35;
				CheckKey();
//20201102���Ӳ���ģʽ
			if(TestFlag)
			{
				OpenOutput();
			}
			else
			{
				CheckLevel();
				if(Sys100msflag)
				{
					Sys100msflag=0;
	//�����¶�ת��
					T100ms++;
					TempertureTemp+=bsearchWithoutRecursion(Thermomete,0,100,GetADCResult(7)*4);
					if(T100ms>=20)
					{
						T100ms=0;
						Temperature=TempertureTemp/20;TempertureTemp=0;TemperatureOk=1;
					}
				}
				if(Sys1sflag){Sys1sflag=0;SecDispose();}//SendData(ZSTime1h);�봦������������ʱ��û�ռ�ʱ����ˮ��⣬���ƿ��ƣ����ȿ��ƣ��������ƣ��������ݴ���
				if(Sys200msFlag)
				{
					Sys200msFlag=0;if((CoinFlag==0)&&(AnliangFlag==0)&&(RemoteFlag==0)&&(LevelError==0))AutoRead();
				}
			}
			CoinCheck();
			Show();
		}
		if(NetWork)//����״̬
		{
			if(SendDataInterval==0){SendDataJoint();}//����������ݼ��Ϊ0�����Ƿ���������Ҫ����
			if(S2ProceFlag){S2ProceFlag=0;UartDataTranslate();}//����ͨѶ
			if(SellState==0)SendLishiPrg();
		}
		else
		{
			if(NetTimeFlag)//����ʱ�䵽
			{
				NetTimeFlag=0;if(PowerDown)InitModel();//
			}
			LostConnection();//���߽������ݴ���//����״̬
		}
		
	}
}
void tm0_isr() interrupt 1
{
	Sys1ms++;
	SellWater();//��ˮ����
	if(Sys1ms%5==0)
	{
//		CoinCheck();//Ͷ�Ҽ��Ŵ˴������ⲻ������Ҫ������������������
	}
	if(Sys1ms%10==0)
	{
		Sys10msflag=1;
		Flicker10ms++;
		if(Flicker10ms>100)Flicker10ms=0;//1��
// -------------------------------------����2��������
    if(S2RecFlag)       //������ڽ���
    { ++S2CharSpace;
      if(S2CharSpace>8)   
      { S2ProceFlag=1;S2RecNum=0;/*S2RecCounBak=S2RecCount;*/ S2RecFlag=0;S2RecCount=0;S2Begin=0;}//TwoPackage=0; S2RecInterval=1;
    }
		else S2CharSpace=0;
	}
	if(Sys1ms%100==0)
	{
		Sys100msflag=1;//100ms��־NetTimeFlag=1;
		if(SendDataInterval)SendDataInterval--;//���ݰ����
		if(AreaCodeKey)
		{
			AreaCodeKey100ms++;
			if(AreaCodeKey100ms>30){SZFlag=1;SetCnt=33;AreaCnt=3;}//��������������ҳ��
			
		}
		else AreaCodeKey100ms=0;
//20201102���Ӳ���ģʽ
		if(TestKeyFlag)TestKey100ms++;//����ǲ��԰�������
		else TestKey100ms=0;
		Flicker100ms++;
		if(Flicker100ms>=5){Flicker100ms=0;FlickerFlag=~FlickerFlag;}
	}
	if(Sys1ms%200==0){Sys200msFlag=1;NetTimeFlag=1;}//
	if(Sys1ms%1000==0)
	{
		Sys1sflag=1;SecFlag=1;
		if(WashTime){WashTime--;CXValve=1;CXFlag=1;}
		else {CXValve=0;CXFlag=0;}
		if(PowerDown==0)PowerDown1s++;
		if(PowerDown1s==2){M800Power=1;}//�ϵ�2����
		if(PowerDown1s>=10){PowerDown1s=0;PowerDown=1;}
		if(ZSFlag)
		{
			if(TDS1flag)	//��ʼ����ԭˮTDS
			{
				TDStime1++;if(TDStime1>=5){TDStime1=0;if(CalTDS(TDS1)>=999)TDS_array[0]=999;else TDS_array[0]=CalTDS(TDS1);TDS1=0;TDS1flag=0;}//5S����һ��	//����ԭˮtds	//TDS���
			}
			if(TDS2flag)	//��ʼ����ԭˮTDS
			{
				TDStime2++;if(TDStime2>=5){TDStime2=0;if(CalTDS(TDS2)>=999)TDS_array[1]=999;else TDS_array[1]=CalTDS(TDS2);TDS2=0;TDS2flag=0;}//5S����һ��	//����ԭˮtds	//TDS���
			}
		}
		else {TDS1=0;TDStime1=0;TDS1flag=0;TDS2=0;TDStime2=0;TDS2flag=0;}
		if(SZFlag)AutoExit++;//����״̬���ް�������10���Զ������˳�
		else AutoExit=0;
		if(HeartTime)HeartTime--;//��������ʱ��
		else HeartFlag=1;
		if(WorkStateLast!=WorkStateByte)
		{
			WorkStateLast=WorkStateByte;StateFlag=1;
		}
//		SendData(WorkStateByte);
	}
	if(Sys1ms>=60000)//1���Ӽ�ʱ��
	{
		Sys1ms=0;Sys1mflag=1;
		if(StateTime)StateTime--;
		else StateFlag=1;
		Parameter1s++;
		if(Parameter1s>=60)//1Сʱ����һ��
		{
			Parameter1s=0;if(ParameterTime)ParameterTime--;else {ParameterFlag=1;TimingFlag=1;}//ÿСʱ����һ���豸����ָ��,������һ��Уʱָ��
		}
	}
 //--------------------------------����
   if(SoundFlag)                     //sound_flug����������Ϊ������
   { ++SoundNum;                     //��ʱ��Ԫ+1
     if((SoundFlag%2)==1)            //��η����ķ����׶�
     { if(SoundNum<150)  Buzzer=1;
       else                          //���ʱ�䵽
       { --SoundFlag;   Buzzer=0;
         if(SoundFlag==0)
           {SoundNum=0;}             //�رձ�־���رշ���λ����
       }
     }
     else                              //��η����Ĳ������׶�
     { if(SoundNum>200)                //��������ʱ����
        {--SoundFlag;SoundNum=0;Buzzer=0;}
     }
   }
	 else Buzzer=0;   
	 if(KeyFlag)Debouncing1ms++;
	 if(Debouncing1ms>=8){Debouncing1ms=0;DebouncingOk=1;}//����10�����ʱ�ɹ�
	 if(ZJKeyFlag||JSKeyFlag)AutoChange1ms++;
	 else AutoChange1ms=0;
	 if(AutoChange1ms>=100){AutoChange1ms=0;AutoChange=1;} 
}
/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart() interrupt 4
{
    if (RI)
    {
        RI = 0;                 //���RIλ
//			Tests=SBUF;
//			RecOk=1;
    }
    if (TI)
    {
        TI = 0;                 //���TIλ
        busy = 0;               //��æ��־
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void SendData(uchar dat)
{
    while (busy);               //�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)
    busy = 1;
    SBUF = ACC;                 //д���ݵ�UART���ݼĴ���
}
void AutoRead()
{
	uchar status=0,ch1=0,ch2=0;
//	if(Sys200msFlag)
//	{
//		Sys200msFlag=0;
		RFIDinit();
		PcdHalt();
		PcdAntennaOff();Delay1ms(6);//�ر��ز�6ms
		PcdAntennaOn();Delay1ms(1);//�����ز�����ʱһ������ж���
//		PcdHalt();
		status=PcdRequest(0x52,UID);//Ѱ��
//		SendData(status);
		if(status==MI_OK)
		{
//			SendData(1);
			status=MI_ERR;status=PcdAnticoll(UID);//����ͻ
			if(status==MI_OK)//�����кųɹ�
			{
//				SendData(2);
				status=MI_ERR;status=PcdSelect(UID);//ѡ��
				if(status==MI_OK)//ѡ�񿨳ɹ�
				{
//					SendData(3);
					status=MI_ERR;status=PcdAuthState(0x60,4,CardKey,UID);//��֤������
					if(status==MI_OK)//��֤����ɹ�
					{
//						SendData(4);
						status=MI_ERR;status=ReadCard4_6(CardBuf);//����
						if(status==MI_OK)//�����ɹ�
						{
//							SendData(5);
							CardCode_Char(CardCode,&CardBuf[9]);//ȡ����
							ch1=CardBuf[0];
							if(ch1!=0xf5)//������
							{
								if(ch1>0xc7) ch2=CardBuf[4];    //���������199����������λ
								else if(ch1>0x99)ch2=CardBuf[1]&0xf0;      //0-199�������λ
								else ch2=0;
								if((ch1==CardAreaCode[0]&&ch2==CardAreaCode[1])||((ch1==0)&&(ch2==0)))//��������ȷ
								{
									CardMoney=ReadMoney(CardBuf);//SendData(CardMoney/0x10000%0x100);SendData(CardMoney%0x10000/0x100);SendData(CardMoney%0x10000%0x100);
									if(RechargeFlag)
									{
//										RechargeFlag=0;SendData(0xaa);
										if((memcmp(CardCode,RecCardCode,4)==0)&&CardType==RecCardType)
										{
											if(ch1==0)CalAreaVal(CardBuf);//д��������
											RechargeCard(CardBuf,RechargeMonney,RechargeType);//����ǳ�ֵָ�����г�ֵ
										}
									}
									else
									{
										if(BlackCardFlag&&(memcmp(CardCode,BlackCard,4)==0)&&(BlackCardType==CardType))//�������ڿ�&CardBuf[9]
										{
											CardBuf[0]=0xf5;Confiscate=5;
											if(WriteCard4_6(CardBuf)==MI_OK)
											{
												Audio_Play(13);BlackCardFlag=0;WriteCardOk=1;return;//д�ڿ��ɹ�֮��5����û��
											}
										}
										else//������
										{
											if(WriteCardOk==0)//���û��д���ɹ�����Ҫ����д��
											{
												if(YuKouOk)//����Ԥ�ۻ����
												{
													if(memcmp(CardSer,UID,4)==0)//�����ͬһ�ſ����򷵻����
													{
														CardMoney+=Balance;//
														SaveMoney(CardMoney,CardBuf);//���ת��Ϊ�������ݸ�ʽ������if(CardError==0)
														status=MI_ERR;status=WriteCard4_6(CardBuf);
														if(status==MI_OK)//���д���ɹ�
														{
														//	SendData(Balance/0x100);//SendData(Balance%0x100);SendData(CardMoney/0x10000%0x100);SendData(CardMoney%0x10000/0x100);SendData(CardMoney%0x10000%0x100);
//															SendData(6);
															YuKouOk=0;Balance=0;WriteCardOk=1;SellState=0;Order0104=1;LedLockTime=EpromDataGu.DataNum.LedLock;//��Ҫ���ͽ������ݰ�������������ʱ
															GZPump=0;SoundFlag=1;Audio_Play(3);//�رչ�װ�ã�����ллʹ��
														}
														else {SoundFlag=5;}//SendData(0xaa);д��ʧ�ܷ�����������
													}
												}
												else//��Ԥ��
												{
													
													TotalMoney=CardMoney;
													if(Vendibility)//����״̬��Ԥ��
													{
														if(CardMoney>EpromDataGu.DataNum.YuKou)
														{
															CardMoney-=EpromDataGu.DataNum.YuKou;KouMoney=EpromDataGu.DataNum.YuKou;
														}
														else
														{
															KouMoney=CardMoney;CardMoney=0;
														}
													}
													else KouMoney=0;//������״̬���ۿ�
													SaveMoney(CardMoney,CardBuf);//���ת��Ϊ�������ݸ�ʽ������if(CardError==0)
													status=MI_ERR;status=WriteCard4_6(CardBuf);
													if(status==MI_OK)//���д���ɹ�
													{
//														SendData(7);
														if(KouMoney)//����пۿ������ۿ�����
														{
															memcpy(CardSer,UID,4);  //���濨���к�
															Balance=KouMoney;YuKouOk=1;SellState=1;TotalWater=0;//SendData(SellState);
															WaterPrice=EpromDataGu.DataNum.CardPrice;Confiscate=EpromDataGu.DataNum.Confiscate;//����û�ռ�ʱ//
														}
														else	LedLockTime=EpromDataGu.DataNum.LedLock;//����ûǮ����������ʱ�ر�
														CardFlag=1;WriteCardOk=1;ReadCardData=1;LightLock=1;//��������
														ConsumeDataPackage(S2TransBuf);PushCRC(S2TransBuf);SendFixedLength(S2TransBuf);ReadCardData=0;////����CRCWriteCardOk=0;
														if(Vendibility)Audio_Play(0);//���Ż�ӭ����
														else {Audio_Play(9);if(TotalMoney)Confiscate=5;SellState=4;ShowMoney=TotalMoney;}//����������ˮ��������ʾ���5����
														SoundFlag=1;
													}
													else
													{
														SoundFlag=5;//SendData(0xff);//д��ʧ�ܷ�����������
													}
												}
											}
										}
									}
								}
								else{if(OnceRead==0)SoundFlag=3;}//�����벻��ȷ������������
							}
							else//�ڿ�����������
							{
								if(OnceRead==0){Audio_Play(13);SoundFlag=5;}//
							}
							OnceRead=1;//�����ɹ���������־��λ��
						}//����
					}//����
				}//ѡ��
			}//����ײ	
		}//Ѱ��
		else//���ÿ���
		{
			OnceRead=0;WriteCardOk=0;//SendData(0x11);
		}
//	}
}
//********************Ͷ��**********************//
void CoinCheck()//Ͷ�Ҽ��
{
	if(CardFlag||AnliangFlag||RemoteFlag||(Vendibility==0)||MaxMoneyFlag||LevelError||ShutDown||SendLishiFlag)TBControl=1;//ˢ����ɨ�룬������ʱ�ر�Ͷ����
	else
	{
		TBControl=0;//Ͷ��
		if(Coin==0)TBFlag=1;   //Ͷ����1SoundFlag=1;
		else
		{ //SoundFlag=1;
			if(TBFlag)              //����ղ��и�����
			{ TBFlag=0; 
//				if(CardFlag==0&&AnliangFlag==0&&RemoteFlag==0)//��ֹˢ��ʱͬʱͶ��
				{
					if(CoinFlag){Balance+=EpromDataGu.DataNum.CoinPluse;TotalMoney+=EpromDataGu.DataNum.CoinPluse;}//TotalMoney
					else
					{
						Audio_Play(0);CoinFlag=1;SellState=1;LightLock=1;Balance=EpromDataGu.DataNum.CoinPluse;TotalMoney=Balance;//CardFlag=0;AnliangFlag=0;RemoteFlag=0;TotalMoney=Balance;
					}
					if(TotalMoney>=(EpromDataGu.DataNum.MaxCoin*100)){if(MaxMoneyFlag==0)Audio_Play(2);MaxMoneyFlag=1;}
					if(SellState==2){GZPump=0;SellState=3;}ShowMoney=Balance;
					Confiscate=EpromDataGu.DataNum.Confiscate;
					WaterPrice=EpromDataGu.DataNum.CoinPrice;TotalWater=0;//Ͷ������ˮ��������ˮ����
				}
			}
		}
	}
}
//********************Ͷ��**********************//
//�������ݴ���
void UartDataTranslate()//ͨѶ���ݽ���,����״̬�²�ִ�д˴�
{
	uchar ch1,ch2=0;
	uchar i=0;
	uint port1,int1;
	ulong long1;
//	ch1=RecBuffer[1];//�����ݳ��ȸ�CH1
	for(ch2=0;ch2<8;ch2++)
	{
		if(S2RecDataBuf[ch2][0]==0x7E)
		{
			memcpy(S2RecBuf,&S2RecDataBuf[ch2][0],50);//�����ݿ��������ݽ��ջ��������д���
//			SendFixedLength2(S2RecBuf);SendData(ch2);SendData(S2RecNum);
			for(ch1=0;ch1<50;ch1++)S2RecDataBuf[ch2][ch1]=0;
			if(CompCRC(S2RecBuf))return;//���CRC���ԣ��������˳�
			if((S2RecBuf[11]==Cdatas[0])&&(S2RecBuf[12]==Cdatas[1])&&(S2RecBuf[13]==Cdatas[2])&&(S2RecBuf[14]==Cdatas[3]))//�豸����ͬ
			{
				if(S2RecBuf[9]==2||S2RecBuf[9]==0x03)HeartCnt=0;//�յ��������ظ����߿���ָ���������������
				switch(S2RecBuf[9])
				{
					case 3:
						S2RecBuf[2]=16;
						switch(S2RecBuf[10])
						{
							case 1://ת��ָ��	0301
								if(S2RecBuf[21]>0)   //������
								{ ch1=S2RecBuf[21];	//��������
									memcpy(&IportBuf[1],&S2RecBuf[22],ch1);  //��������
									IportBuf[0]=ch1;    //��������
									YuanipFlag=1;       //����������־
									NewIPTime=0;       //����������
								}
								else                  //������
								{ 
		//							ch1=S2RecBuf[15];ch1|=S2RecBuf[16];ch1|=S2RecBuf[17];ch1|=S2RecBuf[18];
									ch1=S2RecBuf[15]|S2RecBuf[16]|S2RecBuf[17]|S2RecBuf[18];
									if(ch1>0)           //��IP
									{ 
				//                      012345678901234 5678901   2   34
				//uchar code IpYuan[]={"039.106.074.186\",09999\x0d\x0a1"};
									//ip1------------------------
										IportBuf[0]=15;
										IportBuf[1]=S2RecBuf[15]/100+'0';//��λ
										ch1=S2RecBuf[15]%100;
										IportBuf[2]=ch1/10+'0';IportBuf[3]=ch1%10+'0';//ʮλ����λ
										IportBuf[4]='.';
										//ip2------------------------
										IportBuf[5]=S2RecBuf[16]/100+'0';
										ch1=S2RecBuf[16]%100;
										IportBuf[6]=ch1/10+'0';IportBuf[7]=ch1%10+'0';
										IportBuf[8]='.';
										//ip3------------------------
										IportBuf[9]=S2RecBuf[17]/100+'0';
										ch1=S2RecBuf[17]%100;
										IportBuf[10]=ch1/10+'0';IportBuf[11]=ch1%10+'0';
										IportBuf[12]='.';
										//ip4------------------------
										IportBuf[13]=S2RecBuf[18]/100+'0';
										ch1=S2RecBuf[18]%100;
										IportBuf[14]=ch1/10+'0';IportBuf[15]=ch1%10+'0';
										YuanipFlag=1;
										NewIPTime=0;       //����������
									}
									else   return;      //������,��IP
								}
								//�˿�port------------------
								port1=S2RecBuf[19];port1*=0x100;port1+=S2RecBuf[20];
								IportBuf[22]=(uchar)(port1/10000)+'0';
								port1%=10000;
								ch1=port1/100;
								IportBuf[23]=ch1/10+'0';IportBuf[24]=ch1%10+'0';
								ch1=port1%100;
								IportBuf[25]=ch1/10+'0';IportBuf[26]=ch1%10+'0';
								
								NetProce=0;PowerDown=0;NetWork=0;M800Power=0;       //��������
		//						return;
							break;
							case 2://��ȡ��������ָ��	0302
								if(NetProce>=0x50)          //���������״̬
								{ 
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
									if(ShanNum||LuNum||SaveFull)  //�������ʷ����
									{ 
										if(SaveFull)//��������Ѿ�����
										{
											if((SendShanNum*30+SendLuNum)<(ShanNum*30+LuNum))
											{
												SendShanNum=ShanNum;SendLuNum=LuNum;//�������ָ����ڷ���ָ�룬�ѱ���ָ�븳������ָ�룬��ѭ�Ƚ��ȳ�ԭ��
											}
										}
										S2ReTime=0;
										SendLishiFlag=1;          //������ʷ���ݱ�־
										LiTimerUnit=LishiTimer;   //��ഫ��ʱ��
									}
								}
							break;
							case 3://���µ��¶�����ָ��	0303	
								if((S2RecBuf[15]<41)&&(S2RecBuf[16]<41))
								{
									if(S2RecBuf[15]<S2RecBuf[16])
									{
										EpromDataGu.DataNum.StartHeat=S2RecBuf[15];
										EpromDataGu.DataNum.StopHeat=S2RecBuf[16];
									}
									else 
									{
										EpromDataGu.DataNum.StartHeat=S2RecBuf[16];
										EpromDataGu.DataNum.StopHeat=S2RecBuf[15];
									}
								}
								else 
								{
									EpromDataGu.DataNum.StartHeat=2;
									EpromDataGu.DataNum.StopHeat=7;
								}

								SaveSetData=1;
								SoundFlag=1;
							break;
							case 4://�������·�����ָ��	0304
								if(S2RecBuf[44]&0x40)
								{
									if((S2RecBuf[25]==3)&&(S2RecBuf[45]&0x18))   //�̶����ɨ���ˮ
									{ 
										int1=Array_Int(&S2RecBuf[30]);//ȡ���ѽ��
										if(int1)
										{
											if(S2RecBuf[7]&0x10)//1�ų�ˮ��
											{
												if(Vendibility)
												{
													if((CardFlag==0)&&(CoinFlag==0)&&(AnliangFlag==0))//һ�ų�ˮ�ڲ���ˢ����ˮ��Ͷ�Ҵ�ˮ������
													{
	//													if(AnliangFlag1&&Expenditure1)//������ڰ�����ˮ�в��ұ������ѽ�Ϊ0����Ҫ����
	//													{
	//														AnliangFlag1=0;
	//														Make104Data(S2TransBuf);PushZhenNum(S2TransBuf);
	//														memcpy(&S2TransBuf[15],CardCode1,4); S2TransBuf[25]=3;S2TransBuf[42]=CardType[0];S2TransBuf[29]=0x01;//��������ѽ��˵���ǽ����
	//														Int_Array(Expenditure1,&S2TransBuf[30]);Expenditure1=0;//���뱾�����ѽ��
	//														
	//													}
														memcpy(CardCode,&S2RecBuf[15],4);  //����������·��Ŀ���
														CardType=S2RecBuf[19];//�����̨�·���ˮ������
														Balance=int1;TotalMoney=Balance;  //�������ѽ��
														RemoteFlag=1;WaterPrice=EpromDataGu.DataNum.VirtualPrice;TotalWater=0;Confiscate=EpromDataGu.DataNum.Confiscate;SellState=1;//�������ˮ����
													}
													Audio_Play(8);//LightLockOn();
												}
												else Audio_Play(9);
											}
										}
										SoundFlag=1;
									}
								}
								if((S2RecBuf[46]&0x40)&&(S2RecBuf[41]==2))   //�ڿ�flag-�ڿ�
								{
									if(S2RecBuf[43]&0xf0)  //����п���
									{
										memcpy(BlackCard,&S2RecBuf[15],4);BlackCardFlag=1; //����ڿ�����
//										if(S2RecBuf[42]==0)BlackCard[0]+=0xa0;//A��
//										else if(S2RecBuf[42]>1&&S2RecBuf[42]<7)//
//										{
//											BlackCard[0]+=(S2RecBuf[42]%0x10+9)*0x10;
//										}
////										else if(S2RecBuf[42]==1)//�Ͽ�
////										{
////											int1=Array_Int(&S2RecBuf[15]);
////											long1=(ulong)int1*0x10000;long1+=(ulong)Array_Int(&S2RecBuf[17]);
////											BlackCard[0]=dec_bcd(long1/1000000);long1%=1000000;
////											BlackCard[1]=dec_bcd(long1/10000);int1=long1%10000;BlackCard[2]=dec_bcd(int1/100);BlackCard[3]=dec_bcd(int1%100);
////		//									MoneyArray(long1,&BlackCard[1]);//BlackCard[1]=dec_bcd(BlackCard[1]);BlackCard[2]=dec_bcd(BlackCard[2]);BlackCard[3]=dec_bcd(BlackCard[3]);
////										}
//										else{}
//										if(memcmp(CardCode,BlackCard,4)==0&&Balance){SellState=4;Audio_Play(6);Confiscate=EpromDataGu.DataNum.Confiscate;}//�����ǰ���źͺڿ����ֹ��ˮ
//										//���Ŀ������ڴ�ˮվ��ˮ��������ˢ���˿�
										BlackCardType=S2RecBuf[42];AutoRead();if(memcmp(CardCode,BlackCard,4)==0&&Balance){SellState=4;Audio_Play(6);Confiscate=5;}//�����ǰ���źͺڿ����ֹ��ˮ
									}
								}
								if(S2RecBuf[46]&0x04)
								{
									if(S2RecBuf[37]==0x01){if(ShutDown)PowerOn=1;}//����ָ��
									if(S2RecBuf[37]==0x02){ShutDown=1;}//�ػ�ָ��
								}	
								SoundFlag=1;
							break;
							case 5://�������·���ȡ�豸״ָ̬��	0305
								StateFlag=1;return;
							break;
							case 6://��ϴʱ������ָ��	0306
								if(S2RecBuf[15]>240||S2RecBuf[15]<5)EpromDataGu.DataNum.OnceCXTime=10;//����������ݲ��ڿɵ���Χ�ڣ��򽫳�ϴʱ������ΪĬ��ֵ
								else EpromDataGu.DataNum.OnceCXTime=S2RecBuf[15];
								if(S2RecBuf[16]>240||S2RecBuf[16]<30)EpromDataGu.DataNum.CXInterval=60;
								else EpromDataGu.DataNum.CXInterval=S2RecBuf[16];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 7://����ʱ������	0307
								if(S2RecBuf[15]>120||S2RecBuf[15]<1)EpromDataGu.DataNum.OnceOzoneTime=10;//����������ݲ��ڿɵ���Χ�ڣ��򽫳�ϴʱ������ΪĬ��ֵ
								else EpromDataGu.DataNum.OnceOzoneTime=S2RecBuf[15];
								if(S2RecBuf[16]>240||S2RecBuf[16]<30)EpromDataGu.DataNum.OzoneInterval=60;
								else EpromDataGu.DataNum.OzoneInterval=S2RecBuf[16];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 8://��������	0308
								if(S2RecBuf[15]>23)EpromDataGu.DataNum.StartLEDHour=19;//����������ݲ��ڿɵ���Χ�ڣ��򽫳�ϴʱ������ΪĬ��ֵ
								else EpromDataGu.DataNum.StartLEDHour=S2RecBuf[15];
								if(S2RecBuf[16]>59)EpromDataGu.DataNum.StartLEDMinute=30;
								else EpromDataGu.DataNum.StartLEDMinute=S2RecBuf[16];
								if(S2RecBuf[18]>23)EpromDataGu.DataNum.StopLEDHour=23;//����������ݲ��ڿɵ���Χ�ڣ��򽫳�ϴʱ������ΪĬ��ֵ
								else EpromDataGu.DataNum.StopLEDHour=S2RecBuf[18];
								if(S2RecBuf[19]>59)EpromDataGu.DataNum.StopLEDMinute=30;
								else EpromDataGu.DataNum.StopLEDMinute=S2RecBuf[19];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 9://�豸������	0309
//								EpromDataGu.DataNum.MaxCoin=S2RecBuf[15];SaveSetData=1;//�ر��豸������,��ˮ�ٶ������豸������
							break;	
							case 0x10://��������ˮ��ָ��	0310
									if(S2RecBuf[15]>100||S2RecBuf[15]==0)EpromDataGu.DataNum.MaxCoin=5;
									else EpromDataGu.DataNum.MaxCoin=S2RecBuf[15];
								SaveSetData=1;SoundFlag=1;
							break;
							case 0x11://����������ָ��	0311
								int1=S2RecBuf[15]*0x100+S2RecBuf[16];
								if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//����ǵ�������1�ų�ˮ�ڻ��߹㲥ָ��
								{
									if(S2RecBuf[19]==0x01||S2RecBuf[19]==0x03)//����һ��Ԥ�۽��
									{
										if(int1<10||int1>1000)EpromDataGu.DataNum.YuKou=150;//ʱ��Ʒ����豸���ֽڱ�ʾ�������
										else EpromDataGu.DataNum.YuKou=int1;
									}
									EpromDataGu.DataNum.CoinPluse=50;//Ͷ�������岻�ɵ�
								}
								SaveSetData=1;////
								SoundFlag=1;
							break;
//ע�⣬�˴���Ҫ�����޸�
							case 0x12://��ֵָ��	0312		�ظ�������
								S2RecBuf[2]=28;
								memcpy(RecCardCode,&S2RecBuf[15],4);  //����������·��Ŀ���
								RechargeType=S2RecBuf[20];RecCardType=S2RecBuf[19];
								RechargeMonney=ArrayMoney(&S2RecBuf[21]);//S2RecBuf[21]*0x10000+S2RecBuf[22]*0x100+S2RecBuf[23]+S2RecBuf[24]*0x10000+S2RecBuf[25]*0x100+S2RecBuf[26];//�ܳ�ֵ���
								RechargeMonney+=ArrayMoney(&S2RecBuf[24]);
								RechargeFlag=1;//CardType=S2RecBuf[19];
								if(S2RecBuf[7]&0x10){AutoRead(); }   //���������ȷ���ҿ�������ͬ//1��ˢ����ִ�г�ֵָ��SendData(0x55);
//								SendData(0xaa);
								RechargeFlag=0;
								if(RechargeOk)//�����ֵ�ɹ�
								{
									RechargeOk=0;Audio_Play(4);SellState=4;Confiscate=5;Balance=0;ShowMoney=RechargeFinishMoney;//SendData(RechargeFinishMoney/0x1000000);SendData(RechargeFinishMoney/0x10000%0x100);SendData(RechargeFinishMoney/0x100%0x100);SendData(RechargeFinishMoney%0x100);
									MoneyArray(RechargeMonney,&S2RecBuf[21]);//���γ�ֵ���
									MoneyArray(RechargeFinishMoney,&S2RecBuf[24]);//��ֵ����
								}
								else {Audio_Play(5);return;}//��ֵʧ�ܣ������²���	,��ֵʧ�ܲ��ظ���������
								
							break;
							case 0x13://��ֹ��ˮָ��	0313
									if(S2RecBuf[20]>7||S2RecBuf[20]==1)Audio_Play(6);
									switch(S2RecBuf[20])
									{
										case 2:
											Audio_Play(10);
										break;
										case 3:
											Audio_Play(11);
										break;
										case 4:
											Audio_Play(7);
										break;
										case 5:
											Audio_Play(12);
										break;
										case 6:
											Audio_Play(13);
										break;
										case 7:
											Audio_Play(14);
										break;
										default:break;
									}
									if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//1�ų�ˮ�ڽ�ֹ��ˮ
									{
										if((memcmp(CardCode,&S2RecBuf[15],4)==0)&&Balance){SellState=4;Confiscate=EpromDataGu.DataNum.Confiscate;}//�����ǰ���źͺڿ����ֹ��ˮ
//										if(CardFlag1&&Vendibility&&Balance1){ForbidGZ1=1;PauseSellWater1();}//���һ�ſ�����ˢ����ˮ�����ֹ��ˮ������ˢ����ˮ״̬�²������ֹ��ˮָ�������û�ռ�ʱ����
									}
							break;
							case 0x14://������ʾָ��	0314������оƬ��֧��
								
							break;
							case 0x15://��������ָ��	0315
		//						int1=S2RecBuf[15]*0x100+S2RecBuf[16];int2=S2RecBuf[17]*0x100+S2RecBuf[18];int1=S2RecBuf[19]*0x100+S2RecBuf[20];
								if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//����һ�ų�ˮ�ڵ���
								{
									if(S2RecBuf[15]||S2RecBuf[16])EpromDataGu.DataNum.CardPrice=Array_Int(&S2RecBuf[15])*10;
									if(S2RecBuf[17]||S2RecBuf[18])EpromDataGu.DataNum.VirtualPrice=Array_Int(&S2RecBuf[17])*10;
									if(S2RecBuf[19]||S2RecBuf[20])EpromDataGu.DataNum.CoinPrice=Array_Int(&S2RecBuf[19])*10;
								}
								SaveSetData=1;SoundFlag=1;//for(ch1=0;ch1<S2RecBuf[2]+1;ch1++)SendData(S2RecBuf[ch1]);
							break;
							case 0x16://ˮ����̨����ˮָ��	0316
								long1=ArrayMoney(&S2RecBuf[21]);//S2RecBuf[11]*0x10000+S2RecBuf[12]*0x100+S2RecBuf[13];
								if(long1)//���͵�ָ���н��������Ϊ0�򲻴���
								{
									if(S2RecBuf[7]&0x10)//
									{
										if(Vendibility)
										{
											if((CoinFlag==0)&&(AnliangFlag==0)&&(RemoteFlag==0)||(Balance==0))//һ�ų�ˮ�ڲ���ˢ����ˮ��Ͷ�Ҵ�ˮ������
											{
												CardFlag=0;SellState=1;//����ȴ���ˮ����
												memcpy(CardCode,&S2RecBuf[15],4);  //����������·��Ŀ���
												CardType=S2RecBuf[19];//�����̨�·���ˮ������
												AnliangFlag=1;TotalMoney=long1;
												if(TotalMoney>EpromDataGu.DataNum.MaxCoin*100)Balance=EpromDataGu.DataNum.MaxCoin*100;//�������ˮ���ڽ����ڵ���������ѽ��ѵ�����������ˮ���
												else Balance=(uint)TotalMoney;
												WaterPrice=EpromDataGu.DataNum.VirtualPrice;TotalWater=0;Confiscate=EpromDataGu.DataNum.Confiscate;//����û�ս���ʱ
												Audio_Play(8);
											}
										}
//										else Audio_Play(9);
									}
								}
								SoundFlag=1;
							break;
							default:break;
						}
						if(SaveSetData){SaveSetData=0;WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); }//����
//						if(S2RecBuf[10]==0x12)//
//						{
//							memcpy(S2TransBuf,S2RecBuf,29);     
//							S2TransBuf[2]=28;     //��
//							//��Ҫ�����ֵ����
//						}
//						else //if(S2RecBuf[10]<0x17&&S2RecBuf[10]!=0)
//						{
//							memcpy(S2TransBuf,S2RecBuf,17);     
//							S2TransBuf[2]=16;     //��
//						}
						memcpy(S2TransBuf,S2RecBuf,S2RecBuf[2]+1);
						S2TransBuf[9]=4;//S2TransBuf[10]=2;    //�ظ�0402
						PushCRC(S2TransBuf);        //����CRC	
		//				for(ch1=0;ch1<S2TransBuf[2]+2;ch1++)Uart2SendData(S2TransBuf[ch1]);//���ݴ������֮��ظ����ݸ�������
						SendFixedLength(S2TransBuf);SendDataInterval=DataInterval;
						if(S2TransBuf[9]==3&&S2TransBuf[10]==1)Uart2SendString("+++");//�յ�ת��ָ���,�ظ����ȷ���+++�˳�͸��
					break;
					
					default:break;
				}
		//		for(ch1=0;ch1<S2RecBuf[2]+1;ch1++)SendData(S2RecBuf[ch1]);
				
			}
			else//�������ظ���02��ͷ�����ݰ������˵�½ָ�����豸�ţ�����ָ��û���豸�ţ�Ҳ���ж��豸��
			{
				if(S2RecBuf[9]==2||S2RecBuf[9]==0x12)HeartCnt=0;
				switch(S2RecBuf[9])
				{
					case 2://�������ظ���02
						switch(S2RecBuf[10])
						{
							case 1://�������ظ���	0201
							
							break;
							case 3://ͬ��ʱ��ָ��	0203
								Year=S2RecBuf[12];Month=S2RecBuf[13];Day=S2RecBuf[14];Hour=S2RecBuf[15];Minute=S2RecBuf[16];Second=S2RecBuf[17];
								//iic_send_add_byte(0x00,0x02);Delay_k(1000);  //�ر�ʱ��
								time_init(Second,Minute,Hour,Day,Month,Year);	SoundFlag=1;//SoundFlag=5;//pcf_init();
				//					SaveSetData=1;
							break;
							case 4://�豸����ָ��Ļظ�����ˢ�����ݰ����������ݰ��ȵĻظ���0204
								//��Ҫ�жϽ��յ���֡��ͷ���֡���Ƿ���ͬ����ͬ���ط����Σ����ζ����ɹ�����Ϊ��������
							//104����----------------------------
							if(ZhenBakH==S2RecBuf[5]&&ZhenBakL==S2RecBuf[6])
							{
								SaveLiData=0;SaveLiData1s=0;//SoundFlag=3;
							}
//									for(ch1=0;ch1<5;++ch1)
//									{
//										if(JiluTemp[ch1].Timer)        //������ϴ������Ѽ�¼
//										{ if((S2RecBuf[5]==JiluTemp[ch1].ZhenNo[0])
//											 &&(S2RecBuf[6]==JiluTemp[ch1].ZhenNo[1]))     //���֡�����,˵���������յ�
//											{ JiluTemp[ch1].Timer=0;    //�������Ѽ�¼,�رռ�ʱ��
//		//										SoundFlag=7;
//											}
//										}
//									}
//							SaveDataFlag=0;JiluWaitTime=0;
							break;
							case 5://״̬���Ļظ���	0205
								//�Ƿ���Ҫ���ط�
							break;
							case 7://��ˮʱ��ͬ��ָ��	0207
								//���յ�֡����ͬ���ݻظ��������ˮʱ������
		//						if((S2RecBuf[5]==FrameH)&&(S2RecBuf[6]==FrameL))
		//						{
		//								//������յ���֡����ͬ
		//						}
							//����յ�0207�Ļظ����������ˮʱ�����㣬�����ۼ�
							ZSTime1h=0;SaveZStime();//IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,0);IapProgramByte(ShuiSector+1,0);//����ˮʱ������
							break;
							case 8://�豸����ָ��ظ���	0208
								//��Ҫ����֡���ж��Ƿ�Ҫ�ط�
							break;
							default:break;
						}
					break;
					case 0x12://�������ظ�������ָ��ظ���1204
						if(S2RecBuf[10]==0x04)
						{
							S2ingTimer=0;S2ReTime=0;  //Ϊδ�յ��ظ��ط�����
							SendLiWait=0;     //����ȴ���־��������һ����¼
		//�������޸�
							SendLuNum++;//���ͳɹ��������ݵ�ַ����
							WriSecNum();//��������ָ��
		//					EpromData[0]=0x55;EpromData[1]=ShanNum;EpromData[2]=LuNum;EpromData[3]=SendShanNum;EpromData[4]=SendLuNum;EpromData[6]=0xaa;
		//					WriteEeprom(SysShan,sizeof(EpromData),EpromData);    
//							if(ShanNum<MaxShan) ShanOverFlag=0;                  //������ʷ���ݺ󣬽������
						}
					break;
					default:break;
				}
			}
		}
	}
//	RestData(S2RecBuf);//������ɰѽ��ջ�������
}
void Show()
{
	uchar ch1=0,i=0;
	uint int1=0;
	if(TestFlag)memset(DisBuf,0xff,16);//{for(i=0;i<16;i++)DisBuf[i]=0xff;}//��ʼ����ʾ����
	else
	{
		ShowMoneyWay();
	//	memset(DisBuf,0,16);//��ʼ����ʾ����
		for(i=0;i<16;i++)DisBuf[i]=0;
		if(ShowMoney==0&&DelayShowTime==0)//��һ������ܴ���ʱ��ʾ�·ݺ�ʱ��
		{
			if(ShowEquipmentTime)ShowEquipment();
			else
			{
				if(FlickerFlag)
				{
					DisBuf[9]=DispData[0];DisBuf[5]=DispData[0];
				}
			}
		}
		else//�н��ʱ��ʾ���޽��ʱ��ʾʱ��
		{
			if(ShowMoney<=9999)//������С��100Ԫ
			{
				ch1=ShowMoney/100;
				if(ch1/10)DisBuf[6]=DispData[ch1/10];
				else DisBuf[6]=DispData[Disp_SP];
				DisBuf[7]=DispData[ch1%10]|0x80;//
				ch1=ShowMoney%100;
				DisBuf[8]=DispData[ch1/10];
				DisBuf[9]=DispData[ch1%10];
			}
			else
			{
				if(ShowMoney<=99999)//�����С��100Ԫʱ
				{
					int1=ShowMoney/10;DisBuf[8]|=0x80;
				}
				else
				{
					int1=ShowMoney/100;DisBuf[9]|=0x80;
				}
				ch1=int1/100;
				DisBuf[6]=DispData[ch1/10];
				DisBuf[7]=DispData[ch1%10];
				ch1=int1%100;
				DisBuf[8]|=DispData[ch1/10];
				DisBuf[9]|=DispData[ch1%10];
			}
			if(TotalWater>200000)ShowWater=2000;
			ShowWater=TotalWater/100;DisBuf[4]|=0x80;
			if(ShowWater>999)DisBuf[2]|=DispData[ShowWater/1000];
			if(ShowWater>99)DisBuf[3]|=DispData[ShowWater/100%10];//�����������ʾ�¶�
			DisBuf[4]|=DispData[ShowWater/10%10];
			DisBuf[5]|=DispData[ShowWater%10];
		}
		if(SZFlag)MenuShow();//�����������״̬
		else
		{
			if(ErrorCode)
			{
				if(ErrorCode>1)//�����Һλ���ϣ�ǰ�������Ҳ��ʾ���ϱ���
				{
					DisBuf[2]=DispData[Disp_E];
					DisBuf[3]=DispData[20];
					DisBuf[4]=DispData[8];
					DisBuf[5]=DispData[ErrorCode];
				}
				DisBuf[10]=DispData[Disp_E];
				DisBuf[11]=DispData[8];
				DisBuf[12]=DispData[ErrorCode];
			}
			else
			{
				DisBuf[10]=DispData[Disp_P];
				DisBuf[11]=DispData[WaterLevel];
			}
			DisBuf[15]=DispData[NetProce/0x10];
		}
		StateLedShow();
		DisBuf[1]=StateLed;//DispData[StateLed]StateLed
	}
	
	Update_Display();
}
void ShowMoneyWay()
{
	switch(SellState)
	{
		case 0:
			if(DelayShowTime==0)ShowMoney=0;//ShowMoney=Balance;&&Balance==0
//			else //������ˮ״̬������ʱ��ʾʱ�䵽ʱ��������ʾΪ0
		break;
		case 1:
			ShowMoney=TotalMoney;//��ˮ����Ϊ1ʱ��ʾ���=����ǰ���
		break;
		case 2:
			if(AnliangFlag&&(TotalMoney>EpromDataGu.DataNum.MaxCoin*100))ShowMoney=TotalMoney-(EpromDataGu.DataNum.MaxCoin*100)+Balance;//����ǰ���ȡˮ
			else ShowMoney=Balance;
		break;
//		case 3:
//		
//		break;
		default:break;
	}
}
void ReadEquipment()
{
	EquipmentNumber=Cdatas[0];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[1];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[2];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[3];
	
}
void CloseOutput()
{
	GZPump=0;Heat1=0;Heat2=0;LED_BL=0;Ozone=0;
	ZSPump=0;CXValve=0;LightLock=0;Buzzer=0;TBControl=1;//M800Power=0;
}
void OpenOutput()
{
	GZPump=1;Heat1=1;Heat2=1;LED_BL=1;Ozone=1;
	ZSPump=1;CXValve=1;LightLock=1;TBControl=0;//Buzzer=1;
}
void ShowEquipment()
{
	uchar ch1;
	uint int1;
	int1=EquipmentNumber/10000; 
  ch1=int1/100; DisBuf[6]=DispData[ch1/10];DisBuf[7]=DispData[ch1%10];
	ch1=int1%100; DisBuf[8]=DispData[ch1/10];DisBuf[9]=DispData[ch1%10];
	int1=EquipmentNumber%10000; 
	ch1=int1/100; DisBuf[2]=DispData[ch1/10];DisBuf[3]=DispData[ch1%10];
	ch1=int1%100; DisBuf[4]=DispData[ch1/10];DisBuf[5]=DispData[ch1%10];
}