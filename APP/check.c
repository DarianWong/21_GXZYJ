void SaveZStime()
{
	IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,ZSTime1h/0x100);IapProgramByte(ShuiSector+1,ZSTime1h%0x100);//保存制水时间
}
void SecDispose()//秒处理函数
{
	uchar ch1=0,ch2=0;
	if(SetTimeFlag==0)
	{
		Readtime();
		if(NetProce>=0x50)          //如果在联网状态
		{ ch1=Minute%10;ch2=EquipmentNumber%10;    //当前时间和序列号最后一位
			if((Hour>1)&&(Hour<5)&&(ch1==ch2))     //凌晨2-4点，0-9分钟时分开发送
			{ 							
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
				if(ShanNum||LuNum||SaveFull)  //如果有历史数据
				{ //ShanOverFlag=1;           //不工作标志
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
		}
	}
//	if(Hour==12&&Minute==0&&Second==0)Init_Display();//每天12点整的时候初始化一次TM1640防止黑屏
//	Init_Display();
	if(Confiscate)//如果没收金额时间没到
	{
		Confiscate--;
		if(Confiscate==0)//没收金额时间到,发送结算数据包给主板
		{
			ConfiscateFlag=1;Order0104=1;LedLockTime=EpromDataGu.DataNum.LedLock;GZPump=0;if(Balance)Audio_Play(3);
			if(CardFlag)Expenditure=Balance;//ConsumePackageDispose();//如果不在充值状态时，表示没收金额//需要发送结算数据包，启动灯锁延时,播放谢谢使用
			else if(CoinFlag||RemoteFlag)Expenditure=TotalMoney;//投币，固定金额扫码打水，没收金额时，本次消费金额等于消费前金额，没收金额等于消费前金额
			else{}
		}
	}
	if(SellState==0)
	{
		if(LedLockTime)
		{
			LedLockTime--;
		}
		else LightLock=0;
		if(DelayShowTime)DelayShowTime--;
		else {TotalWater=0;}//ShowMoney=0;打水结束延时时间到则清掉总水量,和显示金额
	}
	else LightLock=1;
//	SendData(ShowWater/0x100);SendData(ShowWater%0x100);
	if(ShowEquipmentTime)ShowEquipmentTime--;
	MakeWater();//制水控制逻辑
	AdvertisingLamp();//广告灯控制逻辑
	if(TemperatureOk)HeatingControl();//加热控制逻辑
	OzoneControl();//臭氧控制逻辑
	if(SaveLiData)//如果有离线数据需要保存，则开始计时5秒之内没收到服务器回复则存为离线数据
	{
		if(SaveLiData1s)//如果保存数据标志
		{
			SaveLiData1s--;
			if(SaveLiData1s==0){SaveLiData=0;JiTimerDelay();}//SoundFlag=5;
		}
	}
	else SaveLiData1s=0;
}
uchar bsearchWithoutRecursion(unsigned int array[],uchar low,uchar high,uint target)		//二分法求温度
{	 uchar mid;
	 if(target<array[high]) mid=high;
	 else if(target>array[low]) mid=low;
	 else
	 {
		while(low<=high)
		{	
			mid=(low+high)/2;
			if(high-low<=1){mid=high;return mid;}
			if(array[mid]<target)
				high=mid-1;
			else if(array[mid]>target)
				low=mid+1;
			else//find the target
				return mid;
		}
	}
	return mid;
}
void CheckLevel()//液位检测
{
	LowLevel_present=LowLevel;
	if(LowLevel_last==LowLevel_present)
	{
		LowLevel_num++;
//		Waterlow_flag=1;
		if(LowLevel_num>10)//连续10次状态都相同则有效
		{
			LowLevel_num=0;
			LowLevel_ok=LowLevel_present;
		}
	}
	else
	{
		LowLevel_num=0;//Waterlow_flag=0;
		LowLevel_last=LowLevel_present;
	}
	HighLevel_present=HighLevel;
	if(HighLevel_last==HighLevel_present)
	{
		HighLevel_num++;
//		Waterlow_flag=1;
		if(HighLevel_num>10)//连续10次状态都相同则有效
		{
			HighLevel_num=0;
			HighLevel_ok=HighLevel_present;
		}
	}
	else
	{
		HighLevel_num=0;//Waterlow_flag=0;
		HighLevel_last=HighLevel_present;
	}
	LowTension_present=LowTension;
	if(LowTension_last==LowTension_present)
	{
		LowTension_num++;
//		Waterlow_flag=1;
		if(LowTension_num>10)//连续10次状态都相同则有效
		{
			LowTension_num=0;
			LowTension_ok=~LowTension_present;
		}
	}
	else
	{
		LowTension_num=0;//Waterlow_flag=0;
		LowTension_last=LowTension_present;
	}
}
void MakeWater()//制水逻辑，每秒处理一次
{
	ErrorCode&=0x05;//清除故障代码其他故障位
	if(LowTension_ok)//如果低压开关闭合
	{
		ErrorCode&=0xfe;//清除E81显示
		if(QSFlag){QSFlag=0;WashTime=EpromDataGu.DataNum.OnceCXTime;}
		if(LowLevel_ok)//如果到达低液位
		{
			ErrorCode&=0xfb;LevelError=0;//如果检测到低液位有水，则取消液位故障报警
			if(HighLevel_ok)//P5状态冲洗结束，进入水满状态
			{
				WaterLevel=5;OutWater1s=0;Vendibility=1;//P5,水满状态将出水计时器清零
				if(ZSFlag)
				{
					ZSFlag=0;WashTime=EpromDataGu.DataNum.OnceCXTime;//如果是从制水到水满，则冲洗15秒
				}
				if(WashTime==0){ZSPump=0;MSFlag=1;}
			}
			else
			{
				if(MSFlag)//P6状态,从水满到水不满，累计出水20L重新开启制水
				{
					WaterLevel=6;//出水计时器应该放在罐装计时部分。
					if(OutWater1s>=(EpromDataGu.DataNum.PumpDelay*60))
					{
						OutWater1s=0;
						MSFlag=0;WaterLevel=4;Vendibility1s=0;
					}
					Vendibility=1;
				}
				else//P3或P4状态
				{
					if(Vendibility1s){Vendibility1s--;WaterLevel=3;}//到达低液位180秒后状态变为可售，否则不可售P3状态
					else {Vendibility=1;WaterLevel=4;}//可售标志置位P4状态
					ZSPump=1;ZSFlag=1;
				}
			}
		}
		else	//未到低液位
		{
			Vendibility=0;MSFlag=0;//可售标志清零
			Vendibility1s=180;
			if(HighLevel_ok)//E-84状态
			{
				ZSPump=0;ZSFlag=0;ErrorCode|=0x04;LevelError=1;Vendibility=0;
			}
			else//P2状态
			{
				ErrorCode&=0xfb;ZSPump=1;ZSFlag=1;WaterLevel=2;//ErrorCode=0;//低液位和高液位都没有水，开启制水泵，制水标志置位 点亮制水指示灯
			}
		}
	}
	else//E81
	{
		ZSPump=0;WashTime=0;ZSFlag=0;QSFlag=1;ErrorCode|=0x01;//低压开关断开，关闭制水泵冲洗阀
		if(LowLevel_ok)//如果到达低液位
		{
			ErrorCode&=0xfb;if(Vendibility1s==0)Vendibility=1;
			if(HighLevel_ok)MSFlag=1;
		}
		else	//未到低液位
		{
			Vendibility=0;//可售标志清零
			if(HighLevel_ok)
			{
				ErrorCode|=0x04;LevelError=1;//Vendibility=0;//液位故障
			}
			else
			{
				ErrorCode&=0xfb;
			}
		}
	}
	if(ZSFlag)
	{
		ZSTime1s++;
		if(ZSTime1s>=60)
		{
			ZSTime1s=0;ZSTime1m++;CXInterval1m++;
			if(ZSTime1m>=60)
			{
				ZSTime1m=0;ZSTime1h++;ZSTimeFlag=1;//发送同步制水时间指令，收到回复则清零
//				IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,ZSTime1h);//保存制水时间
				SaveZStime();
			}
			if(CXInterval1m>=EpromDataGu.DataNum.CXInterval)//冲洗间隔时间到进行冲洗
			{
				CXInterval1m=0;WashTime=EpromDataGu.DataNum.OnceCXTime;//SoundFlag=9;
			}
		}
	}
	if(Vendibility)WorkStateByte|=0x02;
	else WorkStateByte&=0xfd;
	if(MSFlag){WorkStateByte|=0x08;}//StateLed|=0x10;
	else {WorkStateByte&=0xf7;}//StateLed&=0xef;
	if(ZSFlag){WorkStateByte|=0x10;}//StateLed|=0x04;
	else {WorkStateByte&=0xef;}//StateLed&=0xfb;
	if(ErrorCode&0x04)WorkStateByte|=0x20;
	else WorkStateByte&=0xdf;
//	if(LowLevel_ok)StateLed&=0xfe;
//	else StateLed|=0x01;
	if(LowTension_ok){WorkStateByte&=0xfb;}//StateLed&=0xfd;
	else {WorkStateByte|=0x04;}//StateLed|=0x02;
//	if(CXFlag)StateLed|=0x08;
//	else StateLed&=0xf7;

}
void StateLedShow()
{
	if(MSFlag)StateLed|=0x10;
	else StateLed&=0xef;
	if(ZSFlag)StateLed|=0x04;
	else StateLed&=0xfb;
	if(LowLevel_ok)StateLed&=0xfe;
	else StateLed|=0x01;
	if(LowTension_ok)StateLed&=0xfd;
	else StateLed|=0x02;
	if(CXFlag)StateLed|=0x08;
	else StateLed&=0xf7;
}

void CheckKey()//按键检测
{
	uchar KeyValuetemp=0;
	uint int1=0;
//	KeyValue&=0x7f;//清除最高位备用位键值
	if(GZKey)KeyValuetemp&=0xfe;//
	else KeyValuetemp|=0x01;//检测到罐装1按钮
	if(ZTKey)KeyValuetemp&=0xfd;
	else KeyValuetemp|=0x02;
	if(ZJKey)KeyValuetemp&=0xef;
	else KeyValuetemp|=0x10;
	if(JSKey)KeyValuetemp&=0xdf;
	else KeyValuetemp|=0x20;
	if(SZKey)KeyValuetemp&=0xbf;
	else KeyValuetemp|=0x40;
	if(KeyValue==KeyValuetemp)//键值无变化
	{	
		if(KeyValue||KeyChange){KeyFlag=1;KeyChange=0;}//如果键值不为0，按键标志置位，10ms之后检测键值
		if(KeyValue>0x0f){AutoExit=0;}//如果设置按键有效则清除自动退出计时器
		if(AutoExit>10)//自动退出时间到，保存设置参数
		{
			SZFlag=0;SetCnt=0;
//注意！！此处需要修改
			if(SetCnt==32)AreaIntToChar();//说明是设置完区域码
			if(SetPriceFlag){SetPriceFlag=0;ParameterFlag=1;}//设置完单价需要同步下设备参数
			if(SetTimeFlag)
			{
				SetTimeFlag=0;
				iic_send_add_byte(0x00,0x02);Delay_k(1000);  //关闭时钟
				time_init(Second,Minute,Hour,Day,Month,Year);	
				pcf_init();Delay_k(1000);//启动时钟
			}
			WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //保存
//		DataB_A();WriteEeprom(SetDataAdd,38,SetParameter);
		}
		if(DebouncingOk)//10ms消抖完成
		{
			if(KeyValue&0x01){GZKeyFlag=1;}//如果是罐装1键按下
			else 
			{
				if(GZKeyFlag)//按键松手
				{
					GZKeyFlag=0;SoundFlag=1;
					if(Balance&&SellState<4)//如果消费余额不为0，并且售水进程处于待出水，出水，暂停时可以打开罐装泵
					{
						GZPump=1;SellState=2;//开启罐装泵，进入正在出水中的售水进程
						if(CardFlag)WaterFlowRate=EpromDataGu.DataNum.FlowRate1;//刷卡出水速度
						else if(CoinFlag)WaterFlowRate=EpromDataGu.DataNum.FlowRate2;//投币出水速度
						else WaterFlowRate=EpromDataGu.DataNum.FlowRate3;//扫码出水速度
					}
				}
			}
			if(KeyValue&0x02)ZTKeyFlag=1;//如果是暂停1键按下
			else
			{
				if(ZTKeyFlag)//按键松手
				{
					ZTKeyFlag=0;SoundFlag=1;
//					Audio_Play(TestAn);SendData(TestAn);
//					if(TestAn>14)TestAn=0;
//					else {TestAn++;}
					if(Balance&&CardFlag==0)//有余额并且不处于刷卡打水状态
					{
						if(AnliangFlag)//如果是按量取水，则进行结算
						{
							Order0104=1;SellState=4;Audio_Play(3);Balance=0;LedLockTime=EpromDataGu.DataNum.LedLock;//SoundFlag=7;//准备发送结算数据包，并报谢谢使用语音
						}
						else
						{
							if(SellState==2)//如果是从出水状态下按暂停键
							{
								SellState=3;Audio_Play(1);Confiscate=EpromDataGu.DataNum.Confiscate;//进入暂停出水状态，播放罐装暂停语音，启动没收金额计时器
							}
						}
						GZPump=0;
					}
				}
			}
			if(KeyValue&0x10)//如果是减少键按下
			{
				JSKeyFlag=1;
				if(AutoChange)
				{AutoChange=0;
					if(EpromDataGu.DataNum.ParameterLock==0)//设备参数锁，如果为0则设备和服务器均可调节参数
					{
							if(SetCnt>3&&SetCnt<9)SetPriceFlag=1;//如果是调节单价，调节完成之后需要发送0108设备状态同步指令
							else if(SetCnt>25&&SetCnt<32)SetTimeFlag=1;//如果调节时间
							else{}
		//					else SetTimeFlag=0;
						switch(SetCnt)
						{
							case 0:
								
							
							break;//不在设置状态时按减号键显示设备号6秒钟
							case 1://1A
								EpromDataGu.DataNum.YuKou-=10;
								if(EpromDataGu.DataNum.YuKou<10||EpromDataGu.DataNum.YuKou>990)EpromDataGu.DataNum.YuKou=990;
							break;
							case 2://脉冲数2A
							break;
							case 3://投币器脉冲对应金额数2AA
							break;
							case 4://刷卡单价3AA
								--EpromDataGu.DataNum.CardPrice;
								if(EpromDataGu.DataNum.CardPrice<10||EpromDataGu.DataNum.CardPrice>2500)EpromDataGu.DataNum.CardPrice=2500;
							break;
							case 5://扫码单价3AA
								--EpromDataGu.DataNum.VirtualPrice;
								if(EpromDataGu.DataNum.VirtualPrice<10||EpromDataGu.DataNum.VirtualPrice>2500)EpromDataGu.DataNum.VirtualPrice=2500;
							break;
							case 6://投币单价3AA
								--EpromDataGu.DataNum.CoinPrice;
								if(EpromDataGu.DataNum.CoinPrice<10||EpromDataGu.DataNum.CoinPrice>2500)EpromDataGu.DataNum.CoinPrice=2500;
							break;
							case 7:
							break;
							case 8://单次最大消费金额，只限制投币和按量取水5A
								--EpromDataGu.DataNum.MaxCoin;
							if(EpromDataGu.DataNum.MaxCoin==0||EpromDataGu.DataNum.MaxCoin>100)EpromDataGu.DataNum.MaxCoin=100;
							break;
							case 9://刷卡出水速度
								--EpromDataGu.DataNum.FlowRate1;
							if(EpromDataGu.DataNum.FlowRate1==0||EpromDataGu.DataNum.FlowRate1>200)EpromDataGu.DataNum.FlowRate1=200;
							break;
							case 10://扫码出水速度
								--EpromDataGu.DataNum.FlowRate2;
								if(EpromDataGu.DataNum.FlowRate2==0||EpromDataGu.DataNum.FlowRate2>200)EpromDataGu.DataNum.FlowRate2=200;
							break;
							case 11://投币出水速度
								--EpromDataGu.DataNum.FlowRate3;
								if(EpromDataGu.DataNum.FlowRate3==0||EpromDataGu.DataNum.FlowRate3>200)EpromDataGu.DataNum.FlowRate3=200;
							break;
							case 12:
								--EpromDataGu.DataNum.Confiscate;
								if(EpromDataGu.DataNum.Confiscate<15||EpromDataGu.DataNum.Confiscate>240)EpromDataGu.DataNum.Confiscate=240;
							break;
							case 13:
								--EpromDataGu.DataNum.LedLock;
								if(EpromDataGu.DataNum.LedLock<5||EpromDataGu.DataNum.LedLock>240)EpromDataGu.DataNum.LedLock=240;
							break;
							case 14://制水延时单位（分钟）
								--EpromDataGu.DataNum.PumpDelay;
								if(EpromDataGu.DataNum.PumpDelay==0||EpromDataGu.DataNum.PumpDelay>10)EpromDataGu.DataNum.PumpDelay=10;
							break;
							case 15:
								--EpromDataGu.DataNum.OnceCXTime;
								if(EpromDataGu.DataNum.OnceCXTime<5||EpromDataGu.DataNum.OnceCXTime>240)EpromDataGu.DataNum.OnceCXTime=240;
							break;
							case 16://自动冲洗间隔时间单位分
								--EpromDataGu.DataNum.CXInterval;
								if(EpromDataGu.DataNum.CXInterval<30||EpromDataGu.DataNum.CXInterval>240)EpromDataGu.DataNum.CXInterval=240;
							break;
							case 17:
								--EpromDataGu.DataNum.OnceOzoneTime;
								if(EpromDataGu.DataNum.OnceOzoneTime==0||EpromDataGu.DataNum.OnceOzoneTime>50)EpromDataGu.DataNum.OnceOzoneTime=50;
							break;
							case 18:
								--EpromDataGu.DataNum.OzoneInterval;
								if(EpromDataGu.DataNum.OzoneInterval<30||EpromDataGu.DataNum.OzoneInterval>240)EpromDataGu.DataNum.OzoneInterval=240;
							break;
							case 19://减减溢出之后就会变为255，所以此处判断>35
								--EpromDataGu.DataNum.StartHeat;
								if(EpromDataGu.DataNum.StartHeat>35)EpromDataGu.DataNum.StartHeat=35;if(EpromDataGu.DataNum.StopHeat-5<EpromDataGu.DataNum.StartHeat)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 20:
								--EpromDataGu.DataNum.StopHeat;
								if((EpromDataGu.DataNum.StopHeat-5<EpromDataGu.DataNum.StartHeat)||EpromDataGu.DataNum.StopHeat>40)EpromDataGu.DataNum.StopHeat=40;
							break;
							case 21://加热控制时间(分钟)，如果开启一个浴霸灯加热这么长时间未达到设置的停止加热温度，则打开两个浴霸灯进行加热
								--EpromDataGu.DataNum.HeatTime;
								if(EpromDataGu.DataNum.HeatTime>250)EpromDataGu.DataNum.HeatTime=250;
							break;
							case 22:
								--EpromDataGu.DataNum.StartLEDHour;
							if(EpromDataGu.DataNum.StartLEDHour>23)EpromDataGu.DataNum.StartLEDHour=23;
							break;
							case 23:
								--EpromDataGu.DataNum.StartLEDMinute;
								if(EpromDataGu.DataNum.StartLEDMinute>59)EpromDataGu.DataNum.StartLEDMinute=59;
							break;
							case 24:
								--EpromDataGu.DataNum.StopLEDHour;
								if(EpromDataGu.DataNum.StopLEDHour>23)EpromDataGu.DataNum.StopLEDHour=23;
							break;
							case 25:
								--EpromDataGu.DataNum.StopLEDMinute;
								if(EpromDataGu.DataNum.StopLEDMinute>59)EpromDataGu.DataNum.StopLEDMinute=59;
							break;
							case 26:
								--Year;
								if(Year<20||Year>99)Year=99;
							break;
							case 27:
								--Month;
								if(Month>12)Month=12;
							break;
							case 28:
								--Day;
								if(Day>31)Day=31;
							break;
							case 29:
								--Hour;
								if(Hour>23)Hour=23;
							break;
							case 30:
								--Minute;
								if(Minute>59)Minute=59;
							break;
							case 31:
								--Second;
								if(Second>59)Second=59;
							break;
							default:break;
						}
					}
				}
				if(SetCnt==0)
				{
					if(KeyValue&0x20)//如果增加键和减少键同时按下时
					{
						AreaCodeKey=1;
					}
					else ShowEquipmentTime=6;
				}
			}
			else
			{
				if(JSKeyFlag)//按键松手
				{
					JSKeyFlag=0;SoundFlag=1;
					if(SetCnt==33)
					{
						AreaCnt++;
						if(AreaCnt>3)AreaCnt=0;
					}
				}AreaCodeKey=0;
			}
			if(KeyValue&0x20)//如果是增加键按下
			{
				ZJKeyFlag=1;
				if(AutoChange)
				{AutoChange=0;
					if(EpromDataGu.DataNum.ParameterLock==0)//二号口最大单次出水量作设备参数锁，如果为0则设备和服务器均可调节参数
					{
						if(SetCnt>3&&SetCnt<9)SetPriceFlag=1;//如果是调节单价，调节完成之后需要发送0108设备状态同步指令
						else if(SetCnt>25&&SetCnt<32)SetTimeFlag=1;//如果调节时间
						else {}
						switch(SetCnt)
						{
							case 0:break;
							case 1:
								EpromDataGu.DataNum.YuKou+=10;
								if(EpromDataGu.DataNum.YuKou>990||EpromDataGu.DataNum.YuKou<10)EpromDataGu.DataNum.YuKou=10;
							break;
							case 2://脉冲数2A
							break;
							case 3://投币器脉冲对应金额数2AA//投币器1一个脉冲对应金额数
//								++EpromDataGu.DataNum.CoinPluse1;
//								if(EpromDataGu.DataNum.CoinPluse1>1000)EpromDataGu.DataNum.CoinPluse1=1;
							break;
							case 4:
								++EpromDataGu.DataNum.CardPrice;
								if(EpromDataGu.DataNum.CardPrice>2500||EpromDataGu.DataNum.CardPrice<10)EpromDataGu.DataNum.CardPrice=10;
							break;
							case 5:
								++EpromDataGu.DataNum.VirtualPrice;
							if(EpromDataGu.DataNum.VirtualPrice>2500||EpromDataGu.DataNum.VirtualPrice<10)EpromDataGu.DataNum.VirtualPrice=10;
							break;
							case 6:
								++EpromDataGu.DataNum.CoinPrice;
							if(EpromDataGu.DataNum.CoinPrice>2500||EpromDataGu.DataNum.CoinPrice<10)EpromDataGu.DataNum.CoinPrice=10;
							break;
							case 7:
							break;
							case 8:
								++EpromDataGu.DataNum.MaxCoin;
								if(EpromDataGu.DataNum.MaxCoin>100)EpromDataGu.DataNum.MaxCoin=1;
							break;
							case 9:
								++EpromDataGu.DataNum.FlowRate1;
								if(EpromDataGu.DataNum.FlowRate1>200)EpromDataGu.DataNum.FlowRate1=1;
							break;
							case 10:
								++EpromDataGu.DataNum.FlowRate2;
								if(EpromDataGu.DataNum.FlowRate2>200)EpromDataGu.DataNum.FlowRate2=1;
							break;
							case 11:
								++EpromDataGu.DataNum.FlowRate3;
								if(EpromDataGu.DataNum.FlowRate3>200)EpromDataGu.DataNum.FlowRate3=1;
							break;
							case 12:
								++EpromDataGu.DataNum.Confiscate;
							if(EpromDataGu.DataNum.Confiscate>240||EpromDataGu.DataNum.Confiscate<15)EpromDataGu.DataNum.Confiscate=15;
							break;
							case 13:
								++EpromDataGu.DataNum.LedLock;
							if(EpromDataGu.DataNum.LedLock>240||EpromDataGu.DataNum.LedLock<5)EpromDataGu.DataNum.LedLock=5;
							break;
							case 14://制水延时单位（分钟）
								++EpromDataGu.DataNum.PumpDelay;
							if(EpromDataGu.DataNum.PumpDelay>10)EpromDataGu.DataNum.PumpDelay=1;
							break;
							case 15:
								++EpromDataGu.DataNum.OnceCXTime;
							if(EpromDataGu.DataNum.OnceCXTime>240||EpromDataGu.DataNum.OnceCXTime<5)EpromDataGu.DataNum.OnceCXTime=5;
							break;
							case 16://自动冲洗间隔时间单位分
								++EpromDataGu.DataNum.CXInterval;
								if(EpromDataGu.DataNum.CXInterval>240||EpromDataGu.DataNum.CXInterval<30)EpromDataGu.DataNum.CXInterval=30;
							break;
							case 17:
								++EpromDataGu.DataNum.OnceOzoneTime;
							if(EpromDataGu.DataNum.OnceOzoneTime>50)EpromDataGu.DataNum.OnceOzoneTime=1;
							break;
							case 18:
								++EpromDataGu.DataNum.OzoneInterval;
							if(EpromDataGu.DataNum.OzoneInterval>240||EpromDataGu.DataNum.OzoneInterval<30)EpromDataGu.DataNum.OzoneInterval=30;
							break;
							case 19://设置开始加热温度时，停止加热温度自动变化为开始加热温度+5℃
								++EpromDataGu.DataNum.StartHeat;
							if(EpromDataGu.DataNum.StartHeat>35)EpromDataGu.DataNum.StartHeat=0;if((EpromDataGu.DataNum.StopHeat-5)<EpromDataGu.DataNum.StartHeat)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 20:
								++EpromDataGu.DataNum.StopHeat;
							if(EpromDataGu.DataNum.StopHeat>40)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 21://加热控制时间(分钟)，如果开启一个浴霸灯加热这么长时间未达到设置的停止加热温度，则打开两个浴霸灯进行加热
								++EpromDataGu.DataNum.HeatTime;
							if(EpromDataGu.DataNum.HeatTime>250)EpromDataGu.DataNum.HeatTime=0;
							break;
							case 22:
								++EpromDataGu.DataNum.StartLEDHour;
							if(EpromDataGu.DataNum.StartLEDHour>23)EpromDataGu.DataNum.StartLEDHour=0;
							break;
							case 23:
								++EpromDataGu.DataNum.StartLEDMinute;
							if(EpromDataGu.DataNum.StartLEDMinute>59)EpromDataGu.DataNum.StartLEDMinute=0;
							break;
							case 24:
								++EpromDataGu.DataNum.StopLEDHour;
							if(EpromDataGu.DataNum.StopLEDHour>23)EpromDataGu.DataNum.StopLEDHour=0;
							break;
							case 25:
								++EpromDataGu.DataNum.StopLEDMinute;
							if(EpromDataGu.DataNum.StopLEDMinute>59)EpromDataGu.DataNum.StopLEDMinute=0;
							break;
							case 26:
								++Year;
							if(Year>99||Year<20)Year=20;
							break;
							case 27:
								++Month;
							if(Month>12)Month=1;
							break;
							case 28:
								++Day;
							if(Day>31)Day=1;
							break;
							case 29:
								++Hour;
							if(Hour>23)Hour=0;
							break;
							case 30:
								++Minute;
							if(Minute>59)Minute=0;
							break;
							case 31:
								++Second;
							if(Second>59)Second=0;
							break;
							case 33://EpromDataGu.DataNum.AreaCode
								FlickerFlag=1;
								switch(AreaCnt)
								{
									case 0://区域码最高位调节
										if(EpromDataGu.DataNum.AreaCode<9000)EpromDataGu.DataNum.AreaCode+=1000;
										else EpromDataGu.DataNum.AreaCode-=9000;
										
									break;
									case 1://
										int1=EpromDataGu.DataNum.AreaCode%1000;
										if(int1<900) EpromDataGu.DataNum.AreaCode+=100;
										else EpromDataGu.DataNum.AreaCode-=900;
									break;
									case 2:
										int1=EpromDataGu.DataNum.AreaCode%100;
										if(int1<90) EpromDataGu.DataNum.AreaCode+=10;
										else EpromDataGu.DataNum.AreaCode-=90;
									break;
									case 3:
										int1=EpromDataGu.DataNum.AreaCode%10;
										if(int1<9) EpromDataGu.DataNum.AreaCode+=1;
										else EpromDataGu.DataNum.AreaCode-=9;
									break;
									default:break;
								}
							break;
							default:break;
						}
					
						
					}
					
				}
				
			}
			else
			{
				if(ZJKeyFlag)//按键松手
				{
					ZJKeyFlag=0;SoundFlag=1;
				}AreaCodeKey=0;
			}
			if(KeyValue&0x40)SZKeyFlag=1;//如果是设置键按下
			else
			{
				if(SZKeyFlag)//按键松手
				{
					SZKeyFlag=0;SoundFlag=1;
					if(TestFlag)while(1);
					else
					{
						if(SZFlag)
						{
							if(SetCnt==33){AreaIntToChar();SendData(CardAreaCode[0]);SendData(CardAreaCode[1]);}//说明是设置完区域码
							SetCnt++;
							if(SetCnt>32)
							{
								SetCnt=0;SZFlag=0;//DataB_A();WriteEeprom(SetDataAdd,38,SetParameter);
								WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //保存
								if(SetPriceFlag){SetPriceFlag=0;ParameterFlag=1;}
								if(SetTimeFlag)
								{
									SetTimeFlag=0;
									iic_send_add_byte(0x00,0x02);Delay_k(1000);  //关闭时钟
									time_init(Second,Minute,Hour,Day,Month,Year);
									pcf_init();Delay_k(1000);//启动时钟
					
								}
							}
						}//保存并退出
						else {SZFlag=1;SetCnt=1;}
					}
				}
			}
//20201102增加测试模式		
			if((KeyValue&0x40)&&(KeyValue&0x10))TestKeyFlag=1;
			else
			{
				if(TestKeyFlag)
				{
					TestKeyFlag=0;SoundFlag=1;
					if(TestKey100ms>30)
					{
						TestKey100ms=0;TestFlag=1;//SZFlag=1;SetCnt=39;
					}
				}
			}
//20201102增加测试模式
			Debouncing1ms=0;KeyFlag=0;
		}
	}
	else
	{
		KeyFlag=0;Debouncing1ms=0;KeyChange=1;DebouncingOk=0;//如果有新按键按下if(KeyValuetemp){}
		KeyValue=KeyValuetemp;//将变化后的键值	
	}
}
void MenuShow()//菜单显示
{
	switch(SetCnt)
	{
		case 0:
			
		break;
		case 1://1号出水口预扣1A1	1号出水口刷卡时使用
			DisBuf[10]=DispData[1];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.YuKou);
		break;
		case 2://1号出水口1升水对应脉冲数2A1		
//			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
//			DispSetData(EpromDataGu.DataNum.Pluse);
			SetCnt=3;
		break;
		case 3://1号出水口一个脉冲对应金额数2AA
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.CoinPluse);
		break;
		case 4://1号出水口刷卡单价3A
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.CardPrice<1000)DisBuf[12]=DispData[Disp_A];else DisBuf[12]=DispData[EpromDataGu.DataNum.CardPrice/1000];
			DispSetData(EpromDataGu.DataNum.CardPrice%1000);
		break;
		case 5://1号出水口扫码单价3A
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.VirtualPrice<1000)DisBuf[12]=DispData[Disp_B];else DisBuf[12]=DispData[EpromDataGu.DataNum.VirtualPrice/1000];
			DispSetData(EpromDataGu.DataNum.VirtualPrice%1000);
		break;
		case 6://1号出水口投币单价3Ac
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.CoinPrice<1000)DisBuf[12]=DispData[Disp_C];else DisBuf[12]=DispData[EpromDataGu.DataNum.CoinPrice/1000];
			DispSetData(EpromDataGu.DataNum.CoinPrice%1000);
		break;
		case 7://设备参数锁4A
//			DisBuf[10]=DispData[4];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
//			DispSetData(EpromDataGu.DataNum.ParameterLock);
				SetCnt=8;
		break;
		case 8://1号出水口单次最大消费金额5A
			DisBuf[10]=DispData[5];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.MaxCoin);
		break;
		case 9://刷卡出水速度6A1
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.FlowRate1);
		break;
		case 10://扫码出水速度6A2
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.FlowRate2);
		break;
		case 11://投币出水速度6A3
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[3];
			DispSetData(EpromDataGu.DataNum.FlowRate3);
		break;
		case 12://金额回收时间Confiscate
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.Confiscate);
		break;
		case 13://灯锁延时8a
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.LedLock);
		break;
		case 14://制水延时1B
			DisBuf[10]=DispData[1];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.PumpDelay);
		break;
		case 15://冲洗时间
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OnceCXTime);
		break;
		case 16://冲洗间隔
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.CXInterval);
		break;
		case 17://臭氧时间
			DisBuf[10]=DispData[4];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OnceOzoneTime);
		break;
		case 18://臭氧间隔
			DisBuf[10]=DispData[5];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OzoneInterval);
		break;
		case 19://开始加热温度
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.StartHeat);
		break;
		case 20://停止加热温度
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.StopHeat);
		break;
		case 21://加热控制时间(分钟)，如果开启一个浴霸灯加热这么长时间未达到设置的停止加热温度，则打开两个浴霸灯进行加热
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.HeatTime);
		break;
		case 22://广告灯开启时设置
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.StartLEDHour);
		break;
		case 23://广告灯开启分设置
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.StartLEDMinute);
		break;
		case 24://广告灯关闭时设置
			DisBuf[10]=DispData[9];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.StopLEDHour);
		break;
		case 25://广告灯关闭分设置
			DisBuf[10]=DispData[9];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.StopLEDMinute);
		break;
		case 26://年
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[1];
			DispSetData(Year);
		break;
		case 27://月
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[2];
			DispSetData(Month);
		break;
		case 28://日
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[3];
			DispSetData(Day);
		break;
		case 29://时
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[4];
			DispSetData(Hour);
		break;
		case 30://分
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[5];
			DispSetData(Minute);
		break;
		case 31://秒
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[6];
			DispSetData(Second);
		break;
		case 32://版本号
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[0];DisBuf[12]=DispData[Disp_u];//DisBuf[13]=DispData[5];DisBuf[14]=DispData[0]|0x80;DisBuf[15]=DispData[1];
		DispSetData(Version);
		break;
		case 33://区域码
			DisBuf[10]=DispData[Disp_A];//DisBuf[11]=DispData[Disp_sp];
			DisBuf[12]=DispData[EpromDataGu.DataNum.AreaCode/1000];
			DisBuf[13]=DispData[EpromDataGu.DataNum.AreaCode%1000/100];
			DisBuf[14]=DispData[EpromDataGu.DataNum.AreaCode%100/10];
			DisBuf[15]=DispData[EpromDataGu.DataNum.AreaCode%10];
			if(FlickerFlag==0)DisBuf[12+AreaCnt]=DispData[Disp_SP];
		break;
		default:break;
	}

}
void  DispSetData(uint int1)
{
	uchar ch1,ch2;
	ch1=int1/100;ch2=int1%100;
	if(ch1)DisBuf[13]=DispData[ch1];
	if(ch1||(ch2/10))DisBuf[14]=DispData[ch2/10];
	DisBuf[15]=DispData[ch2%10];
	if(SetCnt==1)//预扣有小数点
	{
		DisBuf[13]=DispData[ch1];DisBuf[13]|=0x80;//预扣金额
		DisBuf[14]=DispData[ch2/10];
	}
	if(SetCnt==4||SetCnt==5||SetCnt==6)DisBuf[14]|=0x80;
	if(SetCnt==9||SetCnt==10||SetCnt==11||SetCnt==32)//出水速度，版本号
	{
		DisBuf[14]|=0x80;//出水速度
		DisBuf[14]|=DispData[ch2/10];
	}
}
void AdvertisingLamp()//广告灯判断//根据需求可以适当优化代码量,放置在秒执行函数里
{
	if((EpromDataGu.DataNum.StartLEDHour*60+EpromDataGu.DataNum.StartLEDMinute)>(EpromDataGu.DataNum.StopLEDHour*60+EpromDataGu.DataNum.StopLEDMinute))//如果开启时间大于关闭时间，则说明关闭是在第二天关闭的
	{
		if(((Hour*60+Minute)>(EpromDataGu.DataNum.StartLEDHour*60+EpromDataGu.DataNum.StartLEDMinute))||((Hour*60+Minute)<(EpromDataGu.DataNum.StopLEDHour*60+EpromDataGu.DataNum.StopLEDMinute)))//||(GetADCResult(7)<0x40)先按必须跨天处理，如果客户有需求再加
		{
			LED_BL=1;NightFlag=1;
		}
		else
		{
			LED_BL=0;NightFlag=0;
		}
	}
	else
	{
		if(((Hour*60+Minute)>(EpromDataGu.DataNum.StartLEDHour*60+EpromDataGu.DataNum.StartLEDMinute))&&((Hour*60+Minute)<(EpromDataGu.DataNum.StopLEDHour*60+EpromDataGu.DataNum.StopLEDMinute)))//||(GetADCResult(7)<0x40)
		{
			LED_BL=1;NightFlag=1;
		}
		else
		{
			LED_BL=0;NightFlag=0;
		}
	}
	
}
void HeatingControl()//加热控制，放置在秒执行函数里
{
	if(Temperature>EpromDataGu.DataNum.StopHeat)//当温度高于停止温度
	{
		Heat1=0;Heat2=0;HeatFlag=0;HeatTime1s=0;//SpeedHeat=0;
	}
	else
	{
		if(Temperature<=EpromDataGu.DataNum.StartHeat)//当温度低于开始加热温度时，开启浴霸1进行加热
		{
			if(HeatFlag==0){HeatTime1s=EpromDataGu.DataNum.HeatTime*60;}HeatFlag=1;
			Heat1=1;
		}
		if(HeatTime1s)
		{
			--HeatTime1s;if(HeatTime1s==0)Heat2=1;
		}
//		if(SpeedHeat)//如果
//		{
//			Heat1=1;
//		}
	}
}
/*
*pData存放卡内数据
Value 充值的金额值
Type	充值类型		0x00 正充值，0x01负充值，0x02清零，0x03重置
*/
void RechargeCard(uchar *pData,ulong Value,uchar Type)//在线充值uchar *pUID,
{
	ulong ulong1;//,ulong2
		ulong1=ReadMoney(pData);//读出卡内金额
//		ulong2=ulong1;
		switch(Type)//判断充值类型
		{
			case 0://正充值
				ulong1+=Value;ulong1+=Balance;
				if(ulong1>999999)ulong1=999999;
			break;
			case 1://负充值
				if(ulong1>Value){ulong1-=Value;ulong1+=Balance;}
				else ulong1=0;
			break;
			case 2://清零
				ulong1=0;
			break;
			case 3://重置
				ulong1=Value;
			break;
			default:break;
		}
		SaveMoney(ulong1,pData);
		if(WriteCard4_6(pData)==MI_OK)//充值成功
		{
			RechargeOk=1;RechargeFinishMoney=ulong1;//充值完毕，注意取卡//充值后金额
//			SendData(RechargeFinishMoney/0x1000000);SendData(RechargeFinishMoney/0x10000%0x100);SendData(RechargeFinishMoney/0x100%0x100);SendData(RechargeFinishMoney%0x100);SendData(Balance/0x100);SendData(Balance%0x100);SendData(RechargeOk);
			WriteCardOk=1;
		}
//		else RechargeFinishMoney=ulong2;//充值失败，充值后金额等于充值前金额
	
}


void SellWater()
{
	if(Balance&&SellState==2)//有余额并且在出水状态中
	{
		SellWater1ms++;Confiscate=0;//出水过程中讲金额回收时间清零
//		SellWater10ms++;
		if(WaterLevel==6)OutWater10ms++;//P6状态出水计时器自增,因为单价精度调整了，所以10ms改为1ms了
		if(OutWater10ms>=1000){OutWater10ms=0;OutWater1s++;}
//		if(SellWater10ms>=WaterPrice)
		if(SellWater1ms>=WaterPrice)
		{
			SellWater1ms=0;--Balance;Expenditure++;TotalWater+=WaterFlowRate;DelayShowTime=3;//EpromDataGu.DataNum.FlowRate;if(CardFlag){TotalWater+=EpromDataGu.DataNum.FlowRate1;DelayShowTime=3;}//如果是刷卡打水显示出水量
			if(Balance==0)//金额消费完，需要上传消费清单
			{
				GZPump=0;Order0104=1;SellState=4;Audio_Play(3);LedLockTime=EpromDataGu.DataNum.LedLock;//
				if(AnliangFlag==0)ShowMoney=0;//如果不是按量取水消费完毕时把显示金额清零
			}
		}
	}
//	else GZPump=0;
}
//*********************臭氧****************************//
void OzoneControl()
{
//臭氧控制逻辑
	if(OzoneTime){--OzoneTime;Ozone=1;}//开启臭氧杀菌
	else {++OzoneTime1s;Ozone=0;}
	if(OzoneTime1s>=60)
	{
		OzoneTime1s=0;OzoneTime1m++;
		if(OzoneTime1m>EpromDataGu.DataNum.OzoneInterval)//臭氧间隔时间到，需要开启臭氧
		{
			OzoneTime1m=0;OzoneTime=EpromDataGu.DataNum.OnceOzoneTime;
		}
	}
//臭氧控制逻辑
}
//*********************臭氧****************************//
//读数据区或备份数据区:0=成功,1=失败
uchar ReadCard4_6(uchar *CardBuff)
{	uchar ch1=0,MainStatus=0;

  MainStatus=PcdRead(4,CardBuff);        //读第4块
  if(MainStatus) return 1;  //读卡失败返回错误
  //0块数据判断
  if((*(CardBuff+9)==0xff)&&(*(CardBuff+10)==0xff)
				&&(*(CardBuff+11)==0xff)&&(*(CardBuff+12)==0xff))	 ch1=0;     //如果卡号全为ff
	else ch1=*(CardBuff+9)|*(CardBuff+10)|*(CardBuff+10)|*(CardBuff+10);  //检查卡号是不是全0
  if(ch1==0)											//如果第一块数据不正确
	{	MainStatus=PcdRead(6,CardBuff);       //读第一扇区第3块数据
    if(MainStatus) return 1;  //备份区读卡失败返回错误
		if((*(CardBuff+9)==0xff)&&(*(CardBuff+10)==0xff)
  	  &&(*(CardBuff+11)==0xff)&&(*(CardBuff+12)==0xff))	ch1=0;  //如果卡号全为ff
    else ch1=*(CardBuff+9)|*(CardBuff+10)|*(CardBuff+11)|*(CardBuff+12);
    if(ch1==0)    return 1;              //备份区数据错返回错误
//    KuaiNumOk=2;   //第二块好
  }
//	else	KuaiNumOk=1;	 //第一块好
	 return 0;
}
//将LMoneyNum的金额分不同的卡(新老卡),写入卡内第4第6块
uchar WriteCard4_6(uchar *CardBuff)
{ uchar MainStatus=0;	
 	
	MainStatus=PcdWrite(4,CardBuff);
//	if(MainStatus)PcdWrite(4,CardBuff);
	PcdWrite(6,CardBuff);
//  KuaiNumOk=0;
  return MainStatus;
}
/*
从传入的数据里面获取金额
*/
ulong	ReadMoney(uchar *pData)
{
	bit bit1;
	ulong ulong1;
	ulong1=*(pData+1)%0x10;//取金额最高位
	if(*(pData+9)>=0x9f)bit1=0;//新卡
	else bit1=1;
	if(bit1)
	{
		ulong1*=10000;
		ulong1+=bcd_dec(*(pData+2))*100+bcd_dec(*(pData+3));
//							if(CardMoney>60000) {SoundFlag=5;break;}
		
	}
	else//老卡
	{//
//		ulong1=ulong1*0x10000+*(pData+2)*0x100+*(pData+3);
		ulong1*=0x10000;
		ulong1+=*(pData+2)*0x100+*(pData+3);
//		ulong1+=*(pData+3);
//			ulong1=ArrayMoney(pData+1);
//		MoneyArray(ulong1,pData+1);
//							if(CardMoney>999999){SoundFlag=5;} //增加问题卡语音提示
		
	}
	return ulong1;
}
/*
将金额转换为卡内存放数据格式
Money为需要转换的金额
pData为转换之后存放的地址
*/
void SaveMoney(ulong Money,uchar *pData)
{
	uchar ch1;
	uint int1;
	if(*(pData+9)<0x9f)//如果是老卡
	{
		if(Money>60000) Money=60000;
		ch1=Money/10000; //最高位;           //余额百位
		*(pData+1)&=0xf0;               //区域码低位
		ch1&=0x0f;                      //纠错:屏蔽掉区域码位
		*(pData+1)|=ch1;																//区域码和百位
		int1=Money%10000;
		ch1=int1/100;
		*(pData+2)=dec_bcd(ch1); //余额十个位
		ch1=int1%100;
		*(pData+3)=dec_bcd(ch1); //余额角分位
	}
	else
	{
		ch1=(uchar)(Money/0x10000); //最高位;           //余额百位
		*(pData+1)&=0xf0;               //区域码低位
		ch1&=0x0f;                      //纠错:屏蔽掉区域码位
		*(pData+1)|=ch1;																//区域码和百位
		int1=Money%0x10000;
		*(pData+2)=int1/0x100; //余额十个位
		*(pData+3)=int1%0x100; //余额角分位
	}
}
/*
//-------------------------------
//计算区域码发送给服务器
//  1、区域码<99时,区域码在卡数据第一字节(01)的BCD码
//  2、区域码大于99小于200时,区域码在卡数据前1.5字节(012)，高字节从0xA6开始计数,低位从0开始计数:
//                        区域码100=0xA60n(n代表金额百位),101=0xA61n,....115=0xA6fn,116=0xA70n,
//  3、区域码大于199时，区域码用卡数据第一和第五字节(0189)，第一字节为int的高位，第五字节为int的低位，高字节从0xC8开始，低字节从0开始
//                          区域码200=0xc800,201=0xc801........一直到9999=0xee47.
*/
void CalAreaVal(uchar *pData)
{ 
	*pData=CardAreaCode[0];
	if(EpromDataGu.DataNum.AreaCode>199)
	{
		*(pData+4)=CardAreaCode[1];
	}
	else
	{
		if(EpromDataGu.DataNum.AreaCode>99)
		{
			*(pData+1)|=CardAreaCode[1]&0xf0;
		}
	}
}
//整形变量区域码变为分三段（0-99,100-199,200-9999）的两个字节的
void AreaIntToChar()
{ uint int1;
  if(EpromDataGu.DataNum.AreaCode>199)
		{ int1=0xc738;int1+=EpromDataGu.DataNum.AreaCode;
    CardAreaCode[0]=(uchar)(int1/0x100);         //系统数据改为修改后的数据
    CardAreaCode[1]=(uchar)(int1%0x100);         //系统数据改为修改后的数据
  }
  else
  { if(EpromDataGu.DataNum.AreaCode>99)               //区域码>99时:(区域码值-100)+0xa60或者(区域码值+0x9fc)
    { int1=0x9fc;int1+=EpromDataGu.DataNum.AreaCode;
      CardAreaCode[0]=(uchar)(int1/0x10);         //系统数据改为修改后的数据
      CardAreaCode[1]=(uchar)(int1%0x10);         //系统数据改为修改后的数据
			CardAreaCode[1]*=0x10; 
    }
    else                         //区域码<100时直接变为BCD码
    { 
			CardAreaCode[0]=dec_bcd((uchar)EpromDataGu.DataNum.AreaCode);
      CardAreaCode[1]=0;
    }
	}
}
/*
IC卡内存储的卡号转为通讯卡号进行存储char存储
*/
void CardCode_Char(uchar *pCode,uchar *pICcode)
{
//	uchar	ch1;
	uint int1;
	ulong long1=0;
	CardType=Code_CardType(pICcode);
	if((*pICcode)>0x9f)//新卡
	{
		*pCode=*pICcode&0x0f;*(pCode+1)=*(pICcode+1);*(pCode+2)=*(pICcode+2);*(pCode+3)=*(pICcode+3);
	}
	else//老卡
	{
		long1=(ulong)bcd_dec(*pICcode)*1000000+(ulong)bcd_dec(*(pICcode+1))*10000+bcd_dec(*(pICcode+2))*100+bcd_dec(*(pICcode+3));//此处必须加强制类型转换，否则运算结果会溢出出错
		int1=long1/0x10000;Int_Array(int1,pCode);int1=long1%0x10000;Int_Array(int1,pCode+2);
	}
}
