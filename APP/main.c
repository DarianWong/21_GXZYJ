
#include "head.h"
//#define TestServer
#ifdef	TestServer
uchar code Domain[]={"\x015testmanage.wateree.cn"};
uchar code Port[]={"09999"};
#else
uchar code Domain[]={"\x00cs.waterer.cn"};  //域名,第一字节为域名长度http://testmanage.waterer.cn/
uchar code Port[]={"09999"};
//uchar code Domain[]={"\x013manage.waterchip.cn"};  //域名,第一字节为域名长度http://testmanage.waterer.cn/
//uchar code Port[]={"09999"};
#endif
uchar SoundNum=0;SoundFlag=0;
//-------------TDS------------
struct
{ uint  TDSVal;      //TDS值
  uint  TDSPlus;     //TDS值对应的脉冲值
  uchar  TDSper;      //每个TDS对应脉冲数
}
//code TdsData[6]={{0,0,0},{16,355,22},{50,1051,20},{100,1795,15},{200,3215,14},{300,4236,10}};//TdsData[6]={{0,0,0},{16,933,58},{50,2550,48},{100,4414,37},{200,7046,26},{300,8640,16}};  
//code TdsData[6]={{0,0,0},{26,854,33},{50,1487,26},{100,2761,25},{200,4531,18},{300,5928,14}};
code TdsData[6]={{0,0,0},{16,537,34},{50,1487,28},{100,2761,25},{200,4531,18},{300,5928,14}};
uint TDS1=0,TDS2=0;//TDS计数纯水 TDS计数原水 ,,TDS3=0,TDS4=0	   TDS_formerly=0
uint TDS_array[2]={0};//={0,0}; //原水TDS 纯水TDS	 
uchar TDStime1=0,TDStime2=0;//采样tds计数器 	time100_ms2=0,,TDStime3=0,TDStime4=0
bit TDS1flag=0,TDS2flag=0;			// TDS_CS_flag=0;//,,TDS3flag=0,TDS4flag=0
uint CalTDS(uint PlusNum)//根据脉冲数PlusNum，计算TDS值并返回 
{ uchar ch1,ch2;
  uint int1;
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
//-------------TDS------------

xdata union ActDateGE //--------------系统固定数据结构
 { uchar DataStr[35];    //读Eprom  24B
  struct
  { uchar FirstChar;   //字头      1B
		uint AreaCode;  //区域码   2B
//--------------------------------------------  
		uint YuKou;  		//预扣1								2B		1A1
		uint Pluse;      //脉冲     						2B     1A2  1-250   100   个 10ml对应的脉冲数,或时间当量
    uint CoinPluse;//硬币器脉冲对应金额数		2B
		uint CardPrice;		//一号口刷卡单价			2B
		uint VirtualPrice;//扫码单价						2B
		uint CoinPrice;		//投币单价						2B
		
		uchar MaxCoin;			//单次最大消费金额		1B
		uchar ParameterLock;//参数锁
		uchar FlowRate1;		//刷卡出水速度						1B
		uchar FlowRate2;		//扫码出水速度						1B
		uchar FlowRate3;		//投币出水速度						1B
		uchar	Confiscate;		//没收时间				1B
		uchar LedLock;		//灯锁延时							1B
		uchar	PumpDelay;	//制水泵水满高液位断开	1B
		uchar OnceCXTime;	//单次冲洗时间					1B
		uchar	CXInterval;	//冲洗间隔							1B
		uchar OnceOzoneTime;//臭氧工作时间				1B
		uchar	OzoneInterval;//臭氧工作间隔时间		1B
		uchar	StartHeat;//开始加热温度						1B
		uchar StopHeat;//停止加热温度						1B
		uchar HeatTime;//多长时间未达到停止温度开启两个浴霸灯	1B
		uchar	StartLEDHour;//广告灯开启时设置		1B
		uchar	StartLEDMinute;//广告灯开启分设置	1B
		uchar	StopLEDHour;		//广告灯关闭时设置		1B
		uchar	StopLEDMinute;//广告灯关闭分设置		1B
    uchar LastChar;    //字尾      1B
  }DataNum;
}xdata EpromDataGu;

ulong xdata EquipmentNumber=0;//设备号
uint Sys1ms=0;
bit Sys10msflag=0,Sys100msflag=0,Sys1sflag=0,Sys1mflag=0,SecFlag=0,Sys200msFlag=0,NetTimeFlag=0;
uchar Parameter1s=0,SellWater10ms=0;//售水计时器，当该计时器和售水单价相等时正好扣一分钱
uint	SellWater1ms=0;
//输入检测部分
uint code Thermomete[101]=
{ 782, 774, 765,755,746,737,727,717,707,696,686,675,664,653,642,631,619,608,596,584,		//0~19
	572,560,548,536,524,512,500,489,478,467,456,446,435,425,414,404,394,384,374,365,		//20~39
	355,346,337,328,319,310,302,294,286,278,270,263,256,248,241,235,228,221,215,209,		//40~59
	203,197,192,186,181,176,171,166,161,156,152,148,143,139,135,131,128,124,120,117,		//60~79
	114,111,107,104,101,99,96,93,90,88,85,83,81,79,76,74,72,70,69,67,		//80~99
65};					//100	
uchar xdata Temperature=0;
bit TemperatureOk=0;
uchar xdata KeyValue=0;//键值，bit7：备用，bit6:设置键，bit5:加键，bit4:减键，bit3:停止2键，bit2:开始2键，bit1:停止1键，bit0:开始1键
bit LowLevel_present=0,LowLevel_last=0,LowLevel_ok=0,HighLevel_present=0,HighLevel_last=0,HighLevel_ok=0,LowTension_present=0,LowTension_last=0,LowTension_ok=0;
uchar xdata LowLevel_num=0,HighLevel_num=0,LowTension_num=0;
bit SZFlag=0,KeyFlag=0,KeyChange=0,GZKeyFlag=0,ZTKeyFlag=0,ZJKeyFlag=0,JSKeyFlag=0,SZKeyFlag=0,AreaCodeKey=0,TestKeyFlag=0;
bit DebouncingOk=0;
uchar Debouncing1ms=0;
bit ZSFlag,MSFlag=0,QSFlag=0,CXFlag=0;
//bit	ZSTimeFlag=0;//同步制水时间指令
uchar ZSTime1s=0,ZSTime1m=0,CXInterval1m=0;//制水秒计时器，分计时器，冲洗间隔计时器
uint ZSTime1h=0;//制水时计时器
uchar WashTime=0;//冲洗时间
uchar OzoneTime=0,OzoneTime1s=0,OzoneTime1m=0;
uint OutWater1s=0,OutWater10ms=0;//出水计时器，出水时间用来设置低于高液位后出水多长时间开始制水
bit Vendibility=0;//可售标志
uchar Vendibility1s=0;//可售计时器，水位到达低液位后继续制水180秒变为可售状态
uchar StateLed=0;//状态灯
uchar ErrorCode=0;//故障代码
bit LevelError=0;//液位故障标志
uchar WaterLevel=0;//水位状态

bit ShutDown=0,PowerOn=0;//关机标志
bit SaveSetData=0;//保存设置参数标志
uchar SetCnt=0;
uchar AreaCodeKey100ms=0,AreaCnt=0,ShowEquipmentTime=0,TestKey100ms=0;//区域码按键计时器,区域码位数选择
uchar Flicker100ms=0,Flicker10ms=0;//闪烁时间计时器
bit FlickerFlag=0;//闪烁标志
bit AutoChange=0,SetTimeFlag=0,NightFlag=0;
uchar AutoChange1ms=0,AutoExit=0;//自动变化计时器
bit HeatFlag=0;//快速加热标志,SpeedHeat=0
uchar T100ms=0;//加热计时器
uint HeatTime1s=0;
uint TempertureTemp=0;//温度

uchar SellState=0;//售水进程
bit TBFlag=0;//投币器脉冲，投币打水标志	
bit CoinFlag=0,CardFlag=0,AnliangFlag=0,RemoteFlag=0;//按量取水标志
bit MaxMoneyFlag=0,ConfiscateFlag=0;//最大消费金额标志
uchar Confiscate=0,LedLockTime=0;//没收时间，灯锁延时
uint Balance=0,KouMoney=0,Expenditure=0;//余额,扣款金额,本次消费金额
uint WaterPrice=0;//售水单价
ulong ShowMoney=0;//显示金额
uchar CardAreaCode[2]={0};//默认区域码为9999
uchar CardBuf[16]={0};//卡数据
uchar UID[4]={0};//卡ID
uchar CardSer[4]={0};//保存的卡UID
uchar CardCode[4]={0};//卡号
uchar RecCardCode[4]={0};//接收卡号
uchar BlackCard[4]={0};//黑卡卡号
uchar CardKey[6]={0x03,0x67,0x04,0x01,0x05,0x09};//卡密码
//uchar CardKey[6]={0x03,0x67,0x04,0x67,0x05,0x09};//卡密码
//uchar code CardKey[6]={0x20,0x15,0x05,0x04,0x08,0x30};//太阳雨密码
uchar CardType=0,RechargeType=0,BlackCardType=0,RecCardType=0;//卡类型,接收卡类型
bit RechargeOk=0,RechargeFlag=0,BlackCardFlag=0,OnceRead=0,WriteCardOk=0,YuKouOk=0,ReadCardData=0;//充值成功标志,黑卡标志，单次读卡标志，卡不拿走只读一次卡
ulong CardMoney=0,RechargeMonney=0,RechargeFinishMoney=0,TotalMoney=0;//卡内金额,充值金额,充值完毕金额


//串口部分
//离线数据
uchar xdata EpromData[7];     //Eprom中放的工作参数

bit ReadSendflag=0;//读要发送离线数据扇区标志，此标志置位后需要读取需要发送的离线数据地址扇区数据，否则读取需要写入的离线数据地址扇区数据
////---------------------------
//bit SendLishiFlag;       //需要发送历史数据
//bit SendLiWait;       //发送历史标志时的等待回复标志
//uint LiTimerUnit;     //最大发送时间
////#define LishiTimer  30*60   //最大传输时间30分钟
//bit ShanOverFlag=0;
//uchar JiluPoint=0;
uchar ShanBakNum;          //保存的历史记录扇区位置备份
uchar LuBakNum;            //保存的历史记录在扇区中的位置备份
//uchar S2ReTime;      //同一个记录连续发送次数
//uint xdata ZhenNum=0;//帧序
uchar S2ingTimer;    //S2限时发送一个记录的等待时间
uchar JiluTemp[17]; //发完的消费记录控制数据包有用的数据:帧序4+卡号5+时间6+消费类别1+消费前金额2+消费金额2=20
uchar ZhenBakH=0,ZhenBakL=0;//

xdata union ActDateLS //保存的历史记录数据结构
{ uchar DataByte[512];    //读Eprom  11B
  struct
  { uchar CardCode[4]; //卡号              4B
    uchar TimeByte[6]; //时间:年月日时分秒 6B
    uchar TypeFlag;    //类别:H为消费类别,L为金额类别 1B
    uchar OldMoney[3]; //消费前金额        3B
		uchar	Cardtype;		//卡类型							1B
    uchar YMoney[2];   //本次消费金额      2B
  }DataNum[30];
}xdata RamDataLS;
//卡号4+时间6+消费类别1+消费前金额3++卡类型1+消费金额2=17
//离线数据
bit YuanipFlag=0;
uchar NewIPTime=0;
uchar IportBuf[30]={0};
bit SaveFull=0,SendLishiFlag=0,SendLiWait=0,SaveLiData=0;//历史数据存满标志
uint LiTimerUnit;     //最大发送时间
bit ZhenNumFlag=0;
uint ZhenNum=0;
uchar SendShanNum=0,SendLuNum=0,ShanNum=0,LuNum=0;//历史数据发送到的扇区号//历史数据发送的条数//从之前的倒着发，保存的历史记录扇区位置,保存的历史记录在扇区中的位置
uchar EpromData[7];     //Eprom中放的工作参数
uchar CommVal=0;      //信号强度
uchar NetOverTime=0;     //重新开始心跳计时
uchar S2TransBuf[110];//发送缓冲区
uchar S2RecBuf[110];//数据接收缓存
uchar S2RecDataBuf[8][50];//数据接收缓存
uchar S2RecNum=0,S2DisposeNum=0;//串口接收数据条数,串口数据处理条数
uchar S2RecCount=0,S2DataLen=0,S2TransLen=0;
uchar S2Begin=0;
uchar S2ProceFlag=0;//数据接收完成标志
uchar S2ingTimer,S2ReTime;    //S2限时发送一个记录的等待时间//同一个记录连续发送次数
uchar NetProce=0,AutoReconnection=0;//网络链接状态，1-5,自动重连
uchar S2RecFlag=0,S2CharSpace=0;//串口接收标志，字符间隔，用于接收超时
bit PowerDown=0;//模块断电标志
uchar PowerDown1s=0,SaveLiData1s=0;//断电计时器
bit RorS_Flag=0,NetWork=0;//
bit busy=0,busy2=0;
bit TimingFlag=0,StateFlag=0,HeartFlag=0,Order0104=0;//时间同步指令标志，状态包标志，心跳包标志,
bit ParameterFlag=0,SetPriceFlag=0;//参数同步指令标志,设置单价标志，用于设置完成之后发送参数同步指令
bit ZSTimeFlag=0;//制水时间同步标志
uchar ParameterTime=0,StateTime=0,HeartTime=0;//设备参数发送间隔计时器，状态包计时器，心跳包计时器
uchar HeartCnt=0;//心跳计数器，发送6个数据包没有收到回复则认为断网，需要重新配网
uchar WorkStateByte=0,WorkStateLast=0;//设备工做状态字节
uchar SendDataInterval=0;
uchar TestAn=0;
bit TestFlag=0;
ulong xdata TotalWater=0;//总水量
uchar WaterFlowRate=0,DelayShowTime=0;
uint ShowWater=0;//显示水量
//串口部分

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
//	Init_Display();//初始化TM1640
	PReadEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //读系统数据
	if(EpromDataGu.DataNum.FirstChar!=0x55||EpromDataGu.DataNum.LastChar!=0xaa)//数据头和尾不正确则把初始数据保存起来
	{
		EpromDataGu.DataNum.FirstChar=0x55;               //字头
    EpromDataGu.DataNum.LastChar=0xaa;                //字尾
		EpromDataGu.DataNum.AreaCode=9999;
		EpromDataGu.DataNum.YuKou=150;
		EpromDataGu.DataNum.Pluse=540;
		EpromDataGu.DataNum.CoinPluse=50;
		EpromDataGu.DataNum.CardPrice=400;
		EpromDataGu.DataNum.VirtualPrice=400;
		EpromDataGu.DataNum.CoinPrice=400;
		EpromDataGu.DataNum.MaxCoin=5;
		EpromDataGu.DataNum.ParameterLock=0;//用做参数锁，0为可以在设备或者服务器端修改设备参数，1为仅服务器可设置
		EpromDataGu.DataNum.FlowRate1=50;//一号出水口水流速度
		EpromDataGu.DataNum.FlowRate2=50;
		EpromDataGu.DataNum.FlowRate3=50;
		EpromDataGu.DataNum.Confiscate=180;//没收时间临时调为20秒
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
		WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //保存
//20201124修改制水时间上传问题滤芯寿命问题		
		ZSTime1h=0;SaveZStime();//保存制水时间
//		DataB_A();
//		WriteEeprom(SetDataAdd,38,SetParameter);
		time_init(0,0,8,29,4,20);
		memcpy(IportBuf,Domain,Domain[0]+1);     //域名
    memcpy(&IportBuf[22],Port,5);
    WriteEeprom(SysIpPort,27,IportBuf); 
		SoundFlag=3;
	}
	else
	{
		PReadEeprom(SysIpPort,27,IportBuf);
		ZSTime1h=IapReadByte(ShuiSector)*0x100+IapReadByte(ShuiSector+1);//读取制水时间
		SoundFlag=1;
	}
	AreaIntToChar();
	ReadEquipment();WDT_CONTR=0x35;
	if(LowTension==0&&HighLevel==0)WashTime=EpromDataGu.DataNum.OnceCXTime;//上电检测低压开关闭合，高水位无水，则打开冲洗阀
//	ZSTime1h=2;
	while(1)
	{
//		si=1;
		if(ShutDown)//如果处于关机状态
		{
			memset(DisBuf,0,16);Update_Display();WorkStateByte&=0xfe;if(PowerOn==0)WDT_CONTR=0x35;//关闭显示
			CloseOutput();//关闭所有输出
		}
		else
		{
			WorkStateByte|=0x01;PowerOn=0;WDT_CONTR=0x35;
				CheckKey();
//20201102增加测试模式
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
	//计算温度转换
					T100ms++;
					TempertureTemp+=bsearchWithoutRecursion(Thermomete,0,100,GetADCResult(7)*4);
					if(T100ms>=20)
					{
						T100ms=0;
						Temperature=TempertureTemp/20;TempertureTemp=0;TemperatureOk=1;
					}
				}
				if(Sys1sflag){Sys1sflag=0;SecDispose();}//SendData(ZSTime1h);秒处理包含，主板计时，没收计时，制水检测，广告灯控制，加热控制，臭氧控制，离线数据处理
				if(Sys200msFlag)
				{
					Sys200msFlag=0;if((CoinFlag==0)&&(AnliangFlag==0)&&(RemoteFlag==0)&&(LevelError==0))AutoRead();
				}
			}
			CoinCheck();
			Show();
		}
		if(NetWork)//联网状态
		{
			if(SendDataInterval==0){SendDataJoint();}//如果发送数据间隔为0则检查是否有数据需要发送
			if(S2ProceFlag){S2ProceFlag=0;UartDataTranslate();}//串口通讯
			if(SellState==0)SendLishiPrg();
		}
		else
		{
			if(NetTimeFlag)//配网时间到
			{
				NetTimeFlag=0;if(PowerDown)InitModel();//
			}
			LostConnection();//掉线结算数据处理//离线状态
		}
		
	}
}
void tm0_isr() interrupt 1
{
	Sys1ms++;
	SellWater();//售水进程
	if(Sys1ms%5==0)
	{
//		CoinCheck();//投币检测放此处如果检测不到则需要放在主函数里持续检测
	}
	if(Sys1ms%10==0)
	{
		Sys10msflag=1;
		Flicker10ms++;
		if(Flicker10ms>100)Flicker10ms=0;//1秒
// -------------------------------------串口2结束鉴别
    if(S2RecFlag)       //如果正在接收
    { ++S2CharSpace;
      if(S2CharSpace>8)   
      { S2ProceFlag=1;S2RecNum=0;/*S2RecCounBak=S2RecCount;*/ S2RecFlag=0;S2RecCount=0;S2Begin=0;}//TwoPackage=0; S2RecInterval=1;
    }
		else S2CharSpace=0;
	}
	if(Sys1ms%100==0)
	{
		Sys100msflag=1;//100ms标志NetTimeFlag=1;
		if(SendDataInterval)SendDataInterval--;//数据包间隔
		if(AreaCodeKey)
		{
			AreaCodeKey100ms++;
			if(AreaCodeKey100ms>30){SZFlag=1;SetCnt=33;AreaCnt=3;}//进入区域码设置页面
			
		}
		else AreaCodeKey100ms=0;
//20201102增加测试模式
		if(TestKeyFlag)TestKey100ms++;//如果是测试按键按下
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
		if(PowerDown1s==2){M800Power=1;}//断电2秒钟
		if(PowerDown1s>=10){PowerDown1s=0;PowerDown=1;}
		if(ZSFlag)
		{
			if(TDS1flag)	//开始采样原水TDS
			{
				TDStime1++;if(TDStime1>=5){TDStime1=0;if(CalTDS(TDS1)>=999)TDS_array[0]=999;else TDS_array[0]=CalTDS(TDS1);TDS1=0;TDS1flag=0;}//5S采样一次	//计算原水tds	//TDS清空
			}
			if(TDS2flag)	//开始采样原水TDS
			{
				TDStime2++;if(TDStime2>=5){TDStime2=0;if(CalTDS(TDS2)>=999)TDS_array[1]=999;else TDS_array[1]=CalTDS(TDS2);TDS2=0;TDS2flag=0;}//5S采样一次	//计算原水tds	//TDS清空
			}
		}
		else {TDS1=0;TDStime1=0;TDS1flag=0;TDS2=0;TDStime2=0;TDS2flag=0;}
		if(SZFlag)AutoExit++;//设置状态下无按键操作10秒自动保存退出
		else AutoExit=0;
		if(HeartTime)HeartTime--;//心跳包计时器
		else HeartFlag=1;
		if(WorkStateLast!=WorkStateByte)
		{
			WorkStateLast=WorkStateByte;StateFlag=1;
		}
//		SendData(WorkStateByte);
	}
	if(Sys1ms>=60000)//1分钟计时器
	{
		Sys1ms=0;Sys1mflag=1;
		if(StateTime)StateTime--;
		else StateFlag=1;
		Parameter1s++;
		if(Parameter1s>=60)//1小时进入一次
		{
			Parameter1s=0;if(ParameterTime)ParameterTime--;else {ParameterFlag=1;TimingFlag=1;}//每小时发送一次设备参数指令,并发送一次校时指令
		}
	}
 //--------------------------------声音
   if(SoundFlag)                     //sound_flug的奇数个数为声音数
   { ++SoundNum;                     //计时单元+1
     if((SoundFlag%2)==1)            //多次发声的发声阶段
     { if(SoundNum<150)  Buzzer=1;
       else                          //如果时间到
       { --SoundFlag;   Buzzer=0;
         if(SoundFlag==0)
           {SoundNum=0;}             //关闭标志，关闭发声位电流
       }
     }
     else                              //多次发声的不发声阶段
     { if(SoundNum>200)                //不发声的时间间隔
        {--SoundFlag;SoundNum=0;Buzzer=0;}
     }
   }
	 else Buzzer=0;   
	 if(KeyFlag)Debouncing1ms++;
	 if(Debouncing1ms>=8){Debouncing1ms=0;DebouncingOk=1;}//消抖10毫秒计时成功
	 if(ZJKeyFlag||JSKeyFlag)AutoChange1ms++;
	 else AutoChange1ms=0;
	 if(AutoChange1ms>=100){AutoChange1ms=0;AutoChange=1;} 
}
/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4
{
    if (RI)
    {
        RI = 0;                 //清除RI位
//			Tests=SBUF;
//			RecOk=1;
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(uchar dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}
void AutoRead()
{
	uchar status=0,ch1=0,ch2=0;
//	if(Sys200msFlag)
//	{
//		Sys200msFlag=0;
		RFIDinit();
		PcdHalt();
		PcdAntennaOff();Delay1ms(6);//关闭载波6ms
		PcdAntennaOn();Delay1ms(1);//开启载波，延时一毫秒进行读卡
//		PcdHalt();
		status=PcdRequest(0x52,UID);//寻卡
//		SendData(status);
		if(status==MI_OK)
		{
//			SendData(1);
			status=MI_ERR;status=PcdAnticoll(UID);//防冲突
			if(status==MI_OK)//都序列号成功
			{
//				SendData(2);
				status=MI_ERR;status=PcdSelect(UID);//选择卡
				if(status==MI_OK)//选择卡成功
				{
//					SendData(3);
					status=MI_ERR;status=PcdAuthState(0x60,4,CardKey,UID);//验证卡密码
					if(status==MI_OK)//验证密码成功
					{
//						SendData(4);
						status=MI_ERR;status=ReadCard4_6(CardBuf);//读卡
						if(status==MI_OK)//读卡成功
						{
//							SendData(5);
							CardCode_Char(CardCode,&CardBuf[9]);//取卡号
							ch1=CardBuf[0];
							if(ch1!=0xf5)//正常卡
							{
								if(ch1>0xc7) ch2=CardBuf[4];    //区域码大于199后的区域码低位
								else if(ch1>0x99)ch2=CardBuf[1]&0xf0;      //0-199区域码低位
								else ch2=0;
								if((ch1==CardAreaCode[0]&&ch2==CardAreaCode[1])||((ch1==0)&&(ch2==0)))//区域码正确
								{
									CardMoney=ReadMoney(CardBuf);//SendData(CardMoney/0x10000%0x100);SendData(CardMoney%0x10000/0x100);SendData(CardMoney%0x10000%0x100);
									if(RechargeFlag)
									{
//										RechargeFlag=0;SendData(0xaa);
										if((memcmp(CardCode,RecCardCode,4)==0)&&CardType==RecCardType)
										{
											if(ch1==0)CalAreaVal(CardBuf);//写入区域码
											RechargeCard(CardBuf,RechargeMonney,RechargeType);//如果是充值指令，则进行充值
										}
									}
									else
									{
										if(BlackCardFlag&&(memcmp(CardCode,BlackCard,4)==0)&&(BlackCardType==CardType))//如果是设黑卡&CardBuf[9]
										{
											CardBuf[0]=0xf5;Confiscate=5;
											if(WriteCard4_6(CardBuf)==MI_OK)
											{
												Audio_Play(13);BlackCardFlag=0;WriteCardOk=1;return;//写黑卡成功之后5秒钟没收
											}
										}
										else//正常卡
										{
											if(WriteCardOk==0)//如果没有写卡成功，需要继续写卡
											{
												if(YuKouOk)//返还预扣或余额
												{
													if(memcmp(CardSer,UID,4)==0)//如果是同一张卡，则返还余额
													{
														CardMoney+=Balance;//
														SaveMoney(CardMoney,CardBuf);//金额转换为卡内数据格式并保存if(CardError==0)
														status=MI_ERR;status=WriteCard4_6(CardBuf);
														if(status==MI_OK)//如果写卡成功
														{
														//	SendData(Balance/0x100);//SendData(Balance%0x100);SendData(CardMoney/0x10000%0x100);SendData(CardMoney%0x10000/0x100);SendData(CardMoney%0x10000%0x100);
//															SendData(6);
															YuKouOk=0;Balance=0;WriteCardOk=1;SellState=0;Order0104=1;LedLockTime=EpromDataGu.DataNum.LedLock;//需要发送结算数据包，启动灯锁延时
															GZPump=0;SoundFlag=1;Audio_Play(3);//关闭罐装泵，播放谢谢使用
														}
														else {SoundFlag=5;}//SendData(0xaa);写卡失败蜂鸣器响三声
													}
												}
												else//扣预扣
												{
													
													TotalMoney=CardMoney;
													if(Vendibility)//可售状态扣预扣
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
													else KouMoney=0;//不可售状态不扣款
													SaveMoney(CardMoney,CardBuf);//金额转换为卡内数据格式并保存if(CardError==0)
													status=MI_ERR;status=WriteCard4_6(CardBuf);
													if(status==MI_OK)//如果写卡成功
													{
//														SendData(7);
														if(KouMoney)//如果有扣款金额，进入扣款流程
														{
															memcpy(CardSer,UID,4);  //保存卡序列号
															Balance=KouMoney;YuKouOk=1;SellState=1;TotalWater=0;//SendData(SellState);
															WaterPrice=EpromDataGu.DataNum.CardPrice;Confiscate=EpromDataGu.DataNum.Confiscate;//启动没收计时//
														}
														else	LedLockTime=EpromDataGu.DataNum.LedLock;//卡内没钱启动灯锁延时关闭
														CardFlag=1;WriteCardOk=1;ReadCardData=1;LightLock=1;//开启灯锁
														ConsumeDataPackage(S2TransBuf);PushCRC(S2TransBuf);SendFixedLength(S2TransBuf);ReadCardData=0;////放入CRCWriteCardOk=0;
														if(Vendibility)Audio_Play(0);//播放欢迎语音
														else {Audio_Play(9);if(TotalMoney)Confiscate=5;SellState=4;ShowMoney=TotalMoney;}//播放正在制水语音，显示金额5秒钟
														SoundFlag=1;
													}
													else
													{
														SoundFlag=5;//SendData(0xff);//写卡失败蜂鸣器响三声
													}
												}
											}
										}
									}
								}
								else{if(OnceRead==0)SoundFlag=3;}//区域码不正确蜂鸣器响两声
							}
							else//黑卡，语音提醒
							{
								if(OnceRead==0){Audio_Play(13);SoundFlag=5;}//
							}
							OnceRead=1;//读卡成功，将读标志置位，
						}//读卡
					}//验密
				}//选择卡
			}//防碰撞	
		}//寻卡
		else//卡拿开了
		{
			OnceRead=0;WriteCardOk=0;//SendData(0x11);
		}
//	}
}
//********************投币**********************//
void CoinCheck()//投币检测
{
	if(CardFlag||AnliangFlag||RemoteFlag||(Vendibility==0)||MaxMoneyFlag||LevelError||ShutDown||SendLishiFlag)TBControl=1;//刷卡，扫码，不可售时关闭投币器
	else
	{
		TBControl=0;//投币
		if(Coin==0)TBFlag=1;   //投币器1SoundFlag=1;
		else
		{ //SoundFlag=1;
			if(TBFlag)              //如果刚才有负脉冲
			{ TBFlag=0; 
//				if(CardFlag==0&&AnliangFlag==0&&RemoteFlag==0)//防止刷卡时同时投币
				{
					if(CoinFlag){Balance+=EpromDataGu.DataNum.CoinPluse;TotalMoney+=EpromDataGu.DataNum.CoinPluse;}//TotalMoney
					else
					{
						Audio_Play(0);CoinFlag=1;SellState=1;LightLock=1;Balance=EpromDataGu.DataNum.CoinPluse;TotalMoney=Balance;//CardFlag=0;AnliangFlag=0;RemoteFlag=0;TotalMoney=Balance;
					}
					if(TotalMoney>=(EpromDataGu.DataNum.MaxCoin*100)){if(MaxMoneyFlag==0)Audio_Play(2);MaxMoneyFlag=1;}
					if(SellState==2){GZPump=0;SellState=3;}ShowMoney=Balance;
					Confiscate=EpromDataGu.DataNum.Confiscate;
					WaterPrice=EpromDataGu.DataNum.CoinPrice;TotalWater=0;//投币器售水并启动售水进程
				}
			}
		}
	}
}
//********************投币**********************//
//串口数据处理
void UartDataTranslate()//通讯数据解析,联网状态下才执行此处
{
	uchar ch1,ch2=0;
	uchar i=0;
	uint port1,int1;
	ulong long1;
//	ch1=RecBuffer[1];//把数据长度给CH1
	for(ch2=0;ch2<8;ch2++)
	{
		if(S2RecDataBuf[ch2][0]==0x7E)
		{
			memcpy(S2RecBuf,&S2RecDataBuf[ch2][0],50);//把数据拷贝到数据接收缓冲区进行处理
//			SendFixedLength2(S2RecBuf);SendData(ch2);SendData(S2RecNum);
			for(ch1=0;ch1<50;ch1++)S2RecDataBuf[ch2][ch1]=0;
			if(CompCRC(S2RecBuf))return;//如果CRC不对，不处理退出
			if((S2RecBuf[11]==Cdatas[0])&&(S2RecBuf[12]==Cdatas[1])&&(S2RecBuf[13]==Cdatas[2])&&(S2RecBuf[14]==Cdatas[3]))//设备号相同
			{
				if(S2RecBuf[9]==2||S2RecBuf[9]==0x03)HeartCnt=0;//收到服务器回复或者控制指令，丢包计数器清零
				switch(S2RecBuf[9])
				{
					case 3:
						S2RecBuf[2]=16;
						switch(S2RecBuf[10])
						{
							case 1://转网指令	0301
								if(S2RecBuf[21]>0)   //有域名
								{ ch1=S2RecBuf[21];	//域名长度
									memcpy(&IportBuf[1],&S2RecBuf[22],ch1);  //保存域名
									IportBuf[0]=ch1;    //域名长度
									YuanipFlag=1;       //更新域名标志
									NewIPTime=0;       //上网计数器
								}
								else                  //无域名
								{ 
		//							ch1=S2RecBuf[15];ch1|=S2RecBuf[16];ch1|=S2RecBuf[17];ch1|=S2RecBuf[18];
									ch1=S2RecBuf[15]|S2RecBuf[16]|S2RecBuf[17]|S2RecBuf[18];
									if(ch1>0)           //有IP
									{ 
				//                      012345678901234 5678901   2   34
				//uchar code IpYuan[]={"039.106.074.186\",09999\x0d\x0a1"};
									//ip1------------------------
										IportBuf[0]=15;
										IportBuf[1]=S2RecBuf[15]/100+'0';//百位
										ch1=S2RecBuf[15]%100;
										IportBuf[2]=ch1/10+'0';IportBuf[3]=ch1%10+'0';//十位，个位
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
										NewIPTime=0;       //上网计数器
									}
									else   return;      //无域名,无IP
								}
								//端口port------------------
								port1=S2RecBuf[19];port1*=0x100;port1+=S2RecBuf[20];
								IportBuf[22]=(uchar)(port1/10000)+'0';
								port1%=10000;
								ch1=port1/100;
								IportBuf[23]=ch1/10+'0';IportBuf[24]=ch1%10+'0';
								ch1=port1%100;
								IportBuf[25]=ch1/10+'0';IportBuf[26]=ch1%10+'0';
								
								NetProce=0;PowerDown=0;NetWork=0;M800Power=0;       //重新上网
		//						return;
							break;
							case 2://获取离线数据指令	0302
								if(NetProce>=0x50)          //如果在联网状态
								{ 
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
									if(ShanNum||LuNum||SaveFull)  //如果有历史数据
									{ 
										if(SaveFull)//如果扇区已经存满
										{
											if((SendShanNum*30+SendLuNum)<(ShanNum*30+LuNum))
											{
												SendShanNum=ShanNum;SendLuNum=LuNum;//如果保存指针大于发送指针，把保存指针赋给发送指针，遵循先进先出原则
											}
										}
										S2ReTime=0;
										SendLishiFlag=1;          //发送历史数据标志
										LiTimerUnit=LishiTimer;   //最多传输时间
									}
								}
							break;
							case 3://保温灯温度设置指令	0303	
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
							case 4://服务器下发控制指令	0304
								if(S2RecBuf[44]&0x40)
								{
									if((S2RecBuf[25]==3)&&(S2RecBuf[45]&0x18))   //固定金额扫码打水
									{ 
										int1=Array_Int(&S2RecBuf[30]);//取消费金额
										if(int1)
										{
											if(S2RecBuf[7]&0x10)//1号出水口
											{
												if(Vendibility)
												{
													if((CardFlag==0)&&(CoinFlag==0)&&(AnliangFlag==0))//一号出水口不在刷卡打水和投币打水过程中
													{
	//													if(AnliangFlag1&&Expenditure1)//如果正在按量出水中并且本次消费金额不为0，需要结算
	//													{
	//														AnliangFlag1=0;
	//														Make104Data(S2TransBuf);PushZhenNum(S2TransBuf);
	//														memcpy(&S2TransBuf[15],CardCode1,4); S2TransBuf[25]=3;S2TransBuf[42]=CardType[0];S2TransBuf[29]=0x01;//如果有消费金额说明是结算包
	//														Int_Array(Expenditure1,&S2TransBuf[30]);Expenditure1=0;//放入本次消费金额
	//														
	//													}
														memcpy(CardCode,&S2RecBuf[15],4);  //保存服务器下发的卡号
														CardType=S2RecBuf[19];//保存后台下发的水卡类型
														Balance=int1;TotalMoney=Balance;  //本次消费金额
														RemoteFlag=1;WaterPrice=EpromDataGu.DataNum.VirtualPrice;TotalWater=0;Confiscate=EpromDataGu.DataNum.Confiscate;SellState=1;//进入待售水进程
													}
													Audio_Play(8);//LightLockOn();
												}
												else Audio_Play(9);
											}
										}
										SoundFlag=1;
									}
								}
								if((S2RecBuf[46]&0x40)&&(S2RecBuf[41]==2))   //黑卡flag-黑卡
								{
									if(S2RecBuf[43]&0xf0)  //如果有卡号
									{
										memcpy(BlackCard,&S2RecBuf[15],4);BlackCardFlag=1; //保存黑卡卡号
//										if(S2RecBuf[42]==0)BlackCard[0]+=0xa0;//A卡
//										else if(S2RecBuf[42]>1&&S2RecBuf[42]<7)//
//										{
//											BlackCard[0]+=(S2RecBuf[42]%0x10+9)*0x10;
//										}
////										else if(S2RecBuf[42]==1)//老卡
////										{
////											int1=Array_Int(&S2RecBuf[15]);
////											long1=(ulong)int1*0x10000;long1+=(ulong)Array_Int(&S2RecBuf[17]);
////											BlackCard[0]=dec_bcd(long1/1000000);long1%=1000000;
////											BlackCard[1]=dec_bcd(long1/10000);int1=long1%10000;BlackCard[2]=dec_bcd(int1/100);BlackCard[3]=dec_bcd(int1%100);
////		//									MoneyArray(long1,&BlackCard[1]);//BlackCard[1]=dec_bcd(BlackCard[1]);BlackCard[2]=dec_bcd(BlackCard[2]);BlackCard[3]=dec_bcd(BlackCard[3]);
////										}
//										else{}
//										if(memcmp(CardCode,BlackCard,4)==0&&Balance){SellState=4;Audio_Play(6);Confiscate=EpromDataGu.DataNum.Confiscate;}//如果当前卡号和黑卡则禁止出水
//										//您的卡不能在此水站打水，请重新刷卡退款
										BlackCardType=S2RecBuf[42];AutoRead();if(memcmp(CardCode,BlackCard,4)==0&&Balance){SellState=4;Audio_Play(6);Confiscate=5;}//如果当前卡号和黑卡则禁止出水
									}
								}
								if(S2RecBuf[46]&0x04)
								{
									if(S2RecBuf[37]==0x01){if(ShutDown)PowerOn=1;}//开机指令
									if(S2RecBuf[37]==0x02){ShutDown=1;}//关机指令
								}	
								SoundFlag=1;
							break;
							case 5://服务器下发获取设备状态指令	0305
								StateFlag=1;return;
							break;
							case 6://冲洗时间设置指令	0306
								if(S2RecBuf[15]>240||S2RecBuf[15]<5)EpromDataGu.DataNum.OnceCXTime=10;//如果设置数据不在可调范围内，则将冲洗时间设置为默认值
								else EpromDataGu.DataNum.OnceCXTime=S2RecBuf[15];
								if(S2RecBuf[16]>240||S2RecBuf[16]<30)EpromDataGu.DataNum.CXInterval=60;
								else EpromDataGu.DataNum.CXInterval=S2RecBuf[16];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 7://臭氧时间设置	0307
								if(S2RecBuf[15]>120||S2RecBuf[15]<1)EpromDataGu.DataNum.OnceOzoneTime=10;//如果设置数据不在可调范围内，则将冲洗时间设置为默认值
								else EpromDataGu.DataNum.OnceOzoneTime=S2RecBuf[15];
								if(S2RecBuf[16]>240||S2RecBuf[16]<30)EpromDataGu.DataNum.OzoneInterval=60;
								else EpromDataGu.DataNum.OzoneInterval=S2RecBuf[16];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 8://广告灯设置	0308
								if(S2RecBuf[15]>23)EpromDataGu.DataNum.StartLEDHour=19;//如果设置数据不在可调范围内，则将冲洗时间设置为默认值
								else EpromDataGu.DataNum.StartLEDHour=S2RecBuf[15];
								if(S2RecBuf[16]>59)EpromDataGu.DataNum.StartLEDMinute=30;
								else EpromDataGu.DataNum.StartLEDMinute=S2RecBuf[16];
								if(S2RecBuf[18]>23)EpromDataGu.DataNum.StopLEDHour=23;//如果设置数据不在可调范围内，则将冲洗时间设置为默认值
								else EpromDataGu.DataNum.StopLEDHour=S2RecBuf[18];
								if(S2RecBuf[19]>59)EpromDataGu.DataNum.StopLEDMinute=30;
								else EpromDataGu.DataNum.StopLEDMinute=S2RecBuf[19];
								SaveSetData=1;
								SoundFlag=1;
							break;
							case 9://设备参数锁	0309
//								EpromDataGu.DataNum.MaxCoin=S2RecBuf[15];SaveSetData=1;//关闭设备参数锁,出水速度用做设备参数锁
							break;	
							case 0x10://单次最大出水量指令	0310
									if(S2RecBuf[15]>100||S2RecBuf[15]==0)EpromDataGu.DataNum.MaxCoin=5;
									else EpromDataGu.DataNum.MaxCoin=S2RecBuf[15];
								SaveSetData=1;SoundFlag=1;
							break;
							case 0x11://脉冲数设置指令	0311
								int1=S2RecBuf[15]*0x100+S2RecBuf[16];
								if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//如果是单独设置1号出水口或者广播指令
								{
									if(S2RecBuf[19]==0x01||S2RecBuf[19]==0x03)//设置一号预扣金额
									{
										if(int1<10||int1>1000)EpromDataGu.DataNum.YuKou=150;//时间计费型设备此字节表示余口设置
										else EpromDataGu.DataNum.YuKou=int1;
									}
									EpromDataGu.DataNum.CoinPluse=50;//投币器脉冲不可调
								}
								SaveSetData=1;////
								SoundFlag=1;
							break;
//注意，此处需要重新修改
							case 0x12://充值指令	0312		回复包特殊
								S2RecBuf[2]=28;
								memcpy(RecCardCode,&S2RecBuf[15],4);  //保存服务器下发的卡号
								RechargeType=S2RecBuf[20];RecCardType=S2RecBuf[19];
								RechargeMonney=ArrayMoney(&S2RecBuf[21]);//S2RecBuf[21]*0x10000+S2RecBuf[22]*0x100+S2RecBuf[23]+S2RecBuf[24]*0x10000+S2RecBuf[25]*0x100+S2RecBuf[26];//总充值金额
								RechargeMonney+=ArrayMoney(&S2RecBuf[24]);
								RechargeFlag=1;//CardType=S2RecBuf[19];
								if(S2RecBuf[7]&0x10){AutoRead(); }   //如果卡号正确并且卡类型相同//1号刷卡器执行充值指令SendData(0x55);
//								SendData(0xaa);
								RechargeFlag=0;
								if(RechargeOk)//如果充值成功
								{
									RechargeOk=0;Audio_Play(4);SellState=4;Confiscate=5;Balance=0;ShowMoney=RechargeFinishMoney;//SendData(RechargeFinishMoney/0x1000000);SendData(RechargeFinishMoney/0x10000%0x100);SendData(RechargeFinishMoney/0x100%0x100);SendData(RechargeFinishMoney%0x100);
									MoneyArray(RechargeMonney,&S2RecBuf[21]);//本次充值金额
									MoneyArray(RechargeFinishMoney,&S2RecBuf[24]);//充值后金额
								}
								else {Audio_Play(5);return;}//充值失败，请重新操作	,充值失败不回复给服务器
								
							break;
							case 0x13://禁止打水指令	0313
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
									if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//1号出水口禁止出水
									{
										if((memcmp(CardCode,&S2RecBuf[15],4)==0)&&Balance){SellState=4;Confiscate=EpromDataGu.DataNum.Confiscate;}//如果当前卡号和黑卡则禁止出水
//										if(CardFlag1&&Vendibility&&Balance1){ForbidGZ1=1;PauseSellWater1();}//如果一号口正在刷卡出水，则禁止打水，不在刷卡打水状态下不处理禁止打水指令，并进入没收计时处理
									}
							break;
							case 0x14://语音提示指令	0314，语音芯片不支持
								
							break;
							case 0x15://单价设置指令	0315
		//						int1=S2RecBuf[15]*0x100+S2RecBuf[16];int2=S2RecBuf[17]*0x100+S2RecBuf[18];int1=S2RecBuf[19]*0x100+S2RecBuf[20];
								if((S2RecBuf[7]&0x10)||(S2RecBuf[7]==0))//设置一号出水口单价
								{
									if(S2RecBuf[15]||S2RecBuf[16])EpromDataGu.DataNum.CardPrice=Array_Int(&S2RecBuf[15])*10;
									if(S2RecBuf[17]||S2RecBuf[18])EpromDataGu.DataNum.VirtualPrice=Array_Int(&S2RecBuf[17])*10;
									if(S2RecBuf[19]||S2RecBuf[20])EpromDataGu.DataNum.CoinPrice=Array_Int(&S2RecBuf[19])*10;
								}
								SaveSetData=1;SoundFlag=1;//for(ch1=0;ch1<S2RecBuf[2]+1;ch1++)SendData(S2RecBuf[ch1]);
							break;
							case 0x16://水卡后台余额打水指令	0316
								long1=ArrayMoney(&S2RecBuf[21]);//S2RecBuf[11]*0x10000+S2RecBuf[12]*0x100+S2RecBuf[13];
								if(long1)//发送的指令有金额，则处理，金额为0则不处理
								{
									if(S2RecBuf[7]&0x10)//
									{
										if(Vendibility)
										{
											if((CoinFlag==0)&&(AnliangFlag==0)&&(RemoteFlag==0)||(Balance==0))//一号出水口不在刷卡打水和投币打水过程中
											{
												CardFlag=0;SellState=1;//进入等待出水进程
												memcpy(CardCode,&S2RecBuf[15],4);  //保存服务器下发的卡号
												CardType=S2RecBuf[19];//保存后台下发的水卡类型
												AnliangFlag=1;TotalMoney=long1;
												if(TotalMoney>EpromDataGu.DataNum.MaxCoin*100)Balance=EpromDataGu.DataNum.MaxCoin*100;//如果电子水卡内金额大于单次最大消费金额，把单次最大金额给出水余额
												else Balance=(uint)TotalMoney;
												WaterPrice=EpromDataGu.DataNum.VirtualPrice;TotalWater=0;Confiscate=EpromDataGu.DataNum.Confiscate;//开启没收金额倒计时
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
						if(SaveSetData){SaveSetData=0;WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); }//保存
//						if(S2RecBuf[10]==0x12)//
//						{
//							memcpy(S2TransBuf,S2RecBuf,29);     
//							S2TransBuf[2]=28;     //长
//							//需要填入充值后金额
//						}
//						else //if(S2RecBuf[10]<0x17&&S2RecBuf[10]!=0)
//						{
//							memcpy(S2TransBuf,S2RecBuf,17);     
//							S2TransBuf[2]=16;     //长
//						}
						memcpy(S2TransBuf,S2RecBuf,S2RecBuf[2]+1);
						S2TransBuf[9]=4;//S2TransBuf[10]=2;    //回复0402
						PushCRC(S2TransBuf);        //放入CRC	
		//				for(ch1=0;ch1<S2TransBuf[2]+2;ch1++)Uart2SendData(S2TransBuf[ch1]);//数据处理完成之后回复数据给服务器
						SendFixedLength(S2TransBuf);SendDataInterval=DataInterval;
						if(S2TransBuf[9]==3&&S2TransBuf[10]==1)Uart2SendString("+++");//收到转网指令后,回复完先发送+++退出透传
					break;
					
					default:break;
				}
		//		for(ch1=0;ch1<S2RecBuf[2]+1;ch1++)SendData(S2RecBuf[ch1]);
				
			}
			else//服务器回复的02开头的数据包，除了登陆指令有设备号，其他指令没有设备号，也不判断设备号
			{
				if(S2RecBuf[9]==2||S2RecBuf[9]==0x12)HeartCnt=0;
				switch(S2RecBuf[9])
				{
					case 2://服务器回复包02
						switch(S2RecBuf[10])
						{
							case 1://心跳包回复包	0201
							
							break;
							case 3://同步时间指令	0203
								Year=S2RecBuf[12];Month=S2RecBuf[13];Day=S2RecBuf[14];Hour=S2RecBuf[15];Minute=S2RecBuf[16];Second=S2RecBuf[17];
								//iic_send_add_byte(0x00,0x02);Delay_k(1000);  //关闭时钟
								time_init(Second,Minute,Hour,Day,Month,Year);	SoundFlag=1;//SoundFlag=5;//pcf_init();
				//					SaveSetData=1;
							break;
							case 4://设备控制指令的回复包，刷卡数据包，结算数据包等的回复包0204
								//需要判断接收到的帧序和发送帧序是否相同，不同则重发三次，三次都不成功保存为离线数据
							//104核销----------------------------
							if(ZhenBakH==S2RecBuf[5]&&ZhenBakL==S2RecBuf[6])
							{
								SaveLiData=0;SaveLiData1s=0;//SoundFlag=3;
							}
//									for(ch1=0;ch1<5;++ch1)
//									{
//										if(JiluTemp[ch1].Timer)        //如果有上传的消费记录
//										{ if((S2RecBuf[5]==JiluTemp[ch1].ZhenNo[0])
//											 &&(S2RecBuf[6]==JiluTemp[ch1].ZhenNo[1]))     //如果帧序相等,说明服务器收到
//											{ JiluTemp[ch1].Timer=0;    //核销消费纪录,关闭计时器
//		//										SoundFlag=7;
//											}
//										}
//									}
//							SaveDataFlag=0;JiluWaitTime=0;
							break;
							case 5://状态包的回复包	0205
								//是否需要做重发
							break;
							case 7://制水时间同步指令	0207
								//接收到帧序相同数据回复包则把制水时间清零
		//						if((S2RecBuf[5]==FrameH)&&(S2RecBuf[6]==FrameL))
		//						{
		//								//如果接收到的帧序相同
		//						}
							//如果收到0207的回复包，则把制水时间清零，否则累加
							ZSTime1h=0;SaveZStime();//IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,0);IapProgramByte(ShuiSector+1,0);//将制水时间清零
							break;
							case 8://设备参数指令回复包	0208
								//需要根据帧序判断是否要重发
							break;
							default:break;
						}
					break;
					case 0x12://服务器回复的离线指令回复包1204
						if(S2RecBuf[10]==0x04)
						{
							S2ingTimer=0;S2ReTime=0;  //为未收到回复重发服务
							SendLiWait=0;     //清除等待标志，发送下一条记录
		//王雷雨修改
							SendLuNum++;//发送成功后发送数据地址自增
							WriSecNum();//保存扇区指针
		//					EpromData[0]=0x55;EpromData[1]=ShanNum;EpromData[2]=LuNum;EpromData[3]=SendShanNum;EpromData[4]=SendLuNum;EpromData[6]=0xaa;
		//					WriteEeprom(SysShan,sizeof(EpromData),EpromData);    
//							if(ShanNum<MaxShan) ShanOverFlag=0;                  //发送历史数据后，解除锁定
						}
					break;
					default:break;
				}
			}
		}
	}
//	RestData(S2RecBuf);//处理完成把接收缓存清零
}
void Show()
{
	uchar ch1=0,i=0;
	uint int1=0;
	if(TestFlag)memset(DisBuf,0xff,16);//{for(i=0;i<16;i++)DisBuf[i]=0xff;}//初始化显示数组
	else
	{
		ShowMoneyWay();
	//	memset(DisBuf,0,16);//初始化显示数组
		for(i=0;i<16;i++)DisBuf[i]=0;
		if(ShowMoney==0&&DelayShowTime==0)//第一列数码管待机时显示月份和时间
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
		else//有金额时显示金额，无金额时显示时间
		{
			if(ShowMoney<=9999)//如果金额小于100元
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
				if(ShowMoney<=99999)//当金额小于100元时
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
			if(ShowWater>99)DisBuf[3]|=DispData[ShowWater/100%10];//上面数码管显示温度
			DisBuf[4]|=DispData[ShowWater/10%10];
			DisBuf[5]|=DispData[ShowWater%10];
		}
		if(SZFlag)MenuShow();//如果处于设置状态
		else
		{
			if(ErrorCode)
			{
				if(ErrorCode>1)//如果是液位故障，前面数码管也显示故障报警
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
//			else //不在售水状态并且延时显示时间到时，金额部分显示为0
		break;
		case 1:
			ShowMoney=TotalMoney;//售水进程为1时显示金额=消费前金额
		break;
		case 2:
			if(AnliangFlag&&(TotalMoney>EpromDataGu.DataNum.MaxCoin*100))ShowMoney=TotalMoney-(EpromDataGu.DataNum.MaxCoin*100)+Balance;//如果是按量取水
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