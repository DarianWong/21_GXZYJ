void SaveZStime()
{
	IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,ZSTime1h/0x100);IapProgramByte(ShuiSector+1,ZSTime1h%0x100);//������ˮʱ��
}
void SecDispose()//�봦����
{
	uchar ch1=0,ch2=0;
	if(SetTimeFlag==0)
	{
		Readtime();
		if(NetProce>=0x50)          //���������״̬
		{ ch1=Minute%10;ch2=EquipmentNumber%10;    //��ǰʱ������к����һλ
			if((Hour>1)&&(Hour<5)&&(ch1==ch2))     //�賿2-4�㣬0-9����ʱ�ֿ�����
			{ 							
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
				if(ShanNum||LuNum||SaveFull)  //�������ʷ����
				{ //ShanOverFlag=1;           //��������־
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
		}
	}
//	if(Hour==12&&Minute==0&&Second==0)Init_Display();//ÿ��12������ʱ���ʼ��һ��TM1640��ֹ����
//	Init_Display();
	if(Confiscate)//���û�ս��ʱ��û��
	{
		Confiscate--;
		if(Confiscate==0)//û�ս��ʱ�䵽,���ͽ������ݰ�������
		{
			ConfiscateFlag=1;Order0104=1;LedLockTime=EpromDataGu.DataNum.LedLock;GZPump=0;if(Balance)Audio_Play(3);
			if(CardFlag)Expenditure=Balance;//ConsumePackageDispose();//������ڳ�ֵ״̬ʱ����ʾû�ս��//��Ҫ���ͽ������ݰ�������������ʱ,����ллʹ��
			else if(CoinFlag||RemoteFlag)Expenditure=TotalMoney;//Ͷ�ң��̶����ɨ���ˮ��û�ս��ʱ���������ѽ���������ǰ��û�ս���������ǰ���
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
		else {TotalWater=0;}//ShowMoney=0;��ˮ������ʱʱ�䵽�������ˮ��,����ʾ���
	}
	else LightLock=1;
//	SendData(ShowWater/0x100);SendData(ShowWater%0x100);
	if(ShowEquipmentTime)ShowEquipmentTime--;
	MakeWater();//��ˮ�����߼�
	AdvertisingLamp();//���ƿ����߼�
	if(TemperatureOk)HeatingControl();//���ȿ����߼�
	OzoneControl();//���������߼�
	if(SaveLiData)//���������������Ҫ���棬��ʼ��ʱ5��֮��û�յ��������ظ����Ϊ��������
	{
		if(SaveLiData1s)//����������ݱ�־
		{
			SaveLiData1s--;
			if(SaveLiData1s==0){SaveLiData=0;JiTimerDelay();}//SoundFlag=5;
		}
	}
	else SaveLiData1s=0;
}
uchar bsearchWithoutRecursion(unsigned int array[],uchar low,uchar high,uint target)		//���ַ����¶�
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
void CheckLevel()//Һλ���
{
	LowLevel_present=LowLevel;
	if(LowLevel_last==LowLevel_present)
	{
		LowLevel_num++;
//		Waterlow_flag=1;
		if(LowLevel_num>10)//����10��״̬����ͬ����Ч
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
		if(HighLevel_num>10)//����10��״̬����ͬ����Ч
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
		if(LowTension_num>10)//����10��״̬����ͬ����Ч
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
void MakeWater()//��ˮ�߼���ÿ�봦��һ��
{
	ErrorCode&=0x05;//������ϴ�����������λ
	if(LowTension_ok)//�����ѹ���رպ�
	{
		ErrorCode&=0xfe;//���E81��ʾ
		if(QSFlag){QSFlag=0;WashTime=EpromDataGu.DataNum.OnceCXTime;}
		if(LowLevel_ok)//��������Һλ
		{
			ErrorCode&=0xfb;LevelError=0;//�����⵽��Һλ��ˮ����ȡ��Һλ���ϱ���
			if(HighLevel_ok)//P5״̬��ϴ����������ˮ��״̬
			{
				WaterLevel=5;OutWater1s=0;Vendibility=1;//P5,ˮ��״̬����ˮ��ʱ������
				if(ZSFlag)
				{
					ZSFlag=0;WashTime=EpromDataGu.DataNum.OnceCXTime;//����Ǵ���ˮ��ˮ�������ϴ15��
				}
				if(WashTime==0){ZSPump=0;MSFlag=1;}
			}
			else
			{
				if(MSFlag)//P6״̬,��ˮ����ˮ�������ۼƳ�ˮ20L���¿�����ˮ
				{
					WaterLevel=6;//��ˮ��ʱ��Ӧ�÷��ڹ�װ��ʱ���֡�
					if(OutWater1s>=(EpromDataGu.DataNum.PumpDelay*60))
					{
						OutWater1s=0;
						MSFlag=0;WaterLevel=4;Vendibility1s=0;
					}
					Vendibility=1;
				}
				else//P3��P4״̬
				{
					if(Vendibility1s){Vendibility1s--;WaterLevel=3;}//�����Һλ180���״̬��Ϊ���ۣ����򲻿���P3״̬
					else {Vendibility=1;WaterLevel=4;}//���۱�־��λP4״̬
					ZSPump=1;ZSFlag=1;
				}
			}
		}
		else	//δ����Һλ
		{
			Vendibility=0;MSFlag=0;//���۱�־����
			Vendibility1s=180;
			if(HighLevel_ok)//E-84״̬
			{
				ZSPump=0;ZSFlag=0;ErrorCode|=0x04;LevelError=1;Vendibility=0;
			}
			else//P2״̬
			{
				ErrorCode&=0xfb;ZSPump=1;ZSFlag=1;WaterLevel=2;//ErrorCode=0;//��Һλ�͸�Һλ��û��ˮ��������ˮ�ã���ˮ��־��λ ������ˮָʾ��
			}
		}
	}
	else//E81
	{
		ZSPump=0;WashTime=0;ZSFlag=0;QSFlag=1;ErrorCode|=0x01;//��ѹ���ضϿ����ر���ˮ�ó�ϴ��
		if(LowLevel_ok)//��������Һλ
		{
			ErrorCode&=0xfb;if(Vendibility1s==0)Vendibility=1;
			if(HighLevel_ok)MSFlag=1;
		}
		else	//δ����Һλ
		{
			Vendibility=0;//���۱�־����
			if(HighLevel_ok)
			{
				ErrorCode|=0x04;LevelError=1;//Vendibility=0;//Һλ����
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
				ZSTime1m=0;ZSTime1h++;ZSTimeFlag=1;//����ͬ����ˮʱ��ָ��յ��ظ�������
//				IapEraseSector(ShuiSector);IapProgramByte(ShuiSector,ZSTime1h);//������ˮʱ��
				SaveZStime();
			}
			if(CXInterval1m>=EpromDataGu.DataNum.CXInterval)//��ϴ���ʱ�䵽���г�ϴ
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

void CheckKey()//�������
{
	uchar KeyValuetemp=0;
	uint int1=0;
//	KeyValue&=0x7f;//������λ����λ��ֵ
	if(GZKey)KeyValuetemp&=0xfe;//
	else KeyValuetemp|=0x01;//��⵽��װ1��ť
	if(ZTKey)KeyValuetemp&=0xfd;
	else KeyValuetemp|=0x02;
	if(ZJKey)KeyValuetemp&=0xef;
	else KeyValuetemp|=0x10;
	if(JSKey)KeyValuetemp&=0xdf;
	else KeyValuetemp|=0x20;
	if(SZKey)KeyValuetemp&=0xbf;
	else KeyValuetemp|=0x40;
	if(KeyValue==KeyValuetemp)//��ֵ�ޱ仯
	{	
		if(KeyValue||KeyChange){KeyFlag=1;KeyChange=0;}//�����ֵ��Ϊ0��������־��λ��10ms֮�����ֵ
		if(KeyValue>0x0f){AutoExit=0;}//������ð�����Ч������Զ��˳���ʱ��
		if(AutoExit>10)//�Զ��˳�ʱ�䵽���������ò���
		{
			SZFlag=0;SetCnt=0;
//ע�⣡���˴���Ҫ�޸�
			if(SetCnt==32)AreaIntToChar();//˵����������������
			if(SetPriceFlag){SetPriceFlag=0;ParameterFlag=1;}//�����굥����Ҫͬ�����豸����
			if(SetTimeFlag)
			{
				SetTimeFlag=0;
				iic_send_add_byte(0x00,0x02);Delay_k(1000);  //�ر�ʱ��
				time_init(Second,Minute,Hour,Day,Month,Year);	
				pcf_init();Delay_k(1000);//����ʱ��
			}
			WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //����
//		DataB_A();WriteEeprom(SetDataAdd,38,SetParameter);
		}
		if(DebouncingOk)//10ms�������
		{
			if(KeyValue&0x01){GZKeyFlag=1;}//����ǹ�װ1������
			else 
			{
				if(GZKeyFlag)//��������
				{
					GZKeyFlag=0;SoundFlag=1;
					if(Balance&&SellState<4)//���������Ϊ0��������ˮ���̴��ڴ���ˮ����ˮ����ͣʱ���Դ򿪹�װ��
					{
						GZPump=1;SellState=2;//������װ�ã��������ڳ�ˮ�е���ˮ����
						if(CardFlag)WaterFlowRate=EpromDataGu.DataNum.FlowRate1;//ˢ����ˮ�ٶ�
						else if(CoinFlag)WaterFlowRate=EpromDataGu.DataNum.FlowRate2;//Ͷ�ҳ�ˮ�ٶ�
						else WaterFlowRate=EpromDataGu.DataNum.FlowRate3;//ɨ���ˮ�ٶ�
					}
				}
			}
			if(KeyValue&0x02)ZTKeyFlag=1;//�������ͣ1������
			else
			{
				if(ZTKeyFlag)//��������
				{
					ZTKeyFlag=0;SoundFlag=1;
//					Audio_Play(TestAn);SendData(TestAn);
//					if(TestAn>14)TestAn=0;
//					else {TestAn++;}
					if(Balance&&CardFlag==0)//�����Ҳ�����ˢ����ˮ״̬
					{
						if(AnliangFlag)//����ǰ���ȡˮ������н���
						{
							Order0104=1;SellState=4;Audio_Play(3);Balance=0;LedLockTime=EpromDataGu.DataNum.LedLock;//SoundFlag=7;//׼�����ͽ������ݰ�������ллʹ������
						}
						else
						{
							if(SellState==2)//����Ǵӳ�ˮ״̬�°���ͣ��
							{
								SellState=3;Audio_Play(1);Confiscate=EpromDataGu.DataNum.Confiscate;//������ͣ��ˮ״̬�����Ź�װ��ͣ����������û�ս���ʱ��
							}
						}
						GZPump=0;
					}
				}
			}
			if(KeyValue&0x10)//����Ǽ��ټ�����
			{
				JSKeyFlag=1;
				if(AutoChange)
				{AutoChange=0;
					if(EpromDataGu.DataNum.ParameterLock==0)//�豸�����������Ϊ0���豸�ͷ��������ɵ��ڲ���
					{
							if(SetCnt>3&&SetCnt<9)SetPriceFlag=1;//����ǵ��ڵ��ۣ��������֮����Ҫ����0108�豸״̬ͬ��ָ��
							else if(SetCnt>25&&SetCnt<32)SetTimeFlag=1;//�������ʱ��
							else{}
		//					else SetTimeFlag=0;
						switch(SetCnt)
						{
							case 0:
								
							
							break;//��������״̬ʱ�����ż���ʾ�豸��6����
							case 1://1A
								EpromDataGu.DataNum.YuKou-=10;
								if(EpromDataGu.DataNum.YuKou<10||EpromDataGu.DataNum.YuKou>990)EpromDataGu.DataNum.YuKou=990;
							break;
							case 2://������2A
							break;
							case 3://Ͷ���������Ӧ�����2AA
							break;
							case 4://ˢ������3AA
								--EpromDataGu.DataNum.CardPrice;
								if(EpromDataGu.DataNum.CardPrice<10||EpromDataGu.DataNum.CardPrice>2500)EpromDataGu.DataNum.CardPrice=2500;
							break;
							case 5://ɨ�뵥��3AA
								--EpromDataGu.DataNum.VirtualPrice;
								if(EpromDataGu.DataNum.VirtualPrice<10||EpromDataGu.DataNum.VirtualPrice>2500)EpromDataGu.DataNum.VirtualPrice=2500;
							break;
							case 6://Ͷ�ҵ���3AA
								--EpromDataGu.DataNum.CoinPrice;
								if(EpromDataGu.DataNum.CoinPrice<10||EpromDataGu.DataNum.CoinPrice>2500)EpromDataGu.DataNum.CoinPrice=2500;
							break;
							case 7:
							break;
							case 8://����������ѽ�ֻ����Ͷ�ҺͰ���ȡˮ5A
								--EpromDataGu.DataNum.MaxCoin;
							if(EpromDataGu.DataNum.MaxCoin==0||EpromDataGu.DataNum.MaxCoin>100)EpromDataGu.DataNum.MaxCoin=100;
							break;
							case 9://ˢ����ˮ�ٶ�
								--EpromDataGu.DataNum.FlowRate1;
							if(EpromDataGu.DataNum.FlowRate1==0||EpromDataGu.DataNum.FlowRate1>200)EpromDataGu.DataNum.FlowRate1=200;
							break;
							case 10://ɨ���ˮ�ٶ�
								--EpromDataGu.DataNum.FlowRate2;
								if(EpromDataGu.DataNum.FlowRate2==0||EpromDataGu.DataNum.FlowRate2>200)EpromDataGu.DataNum.FlowRate2=200;
							break;
							case 11://Ͷ�ҳ�ˮ�ٶ�
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
							case 14://��ˮ��ʱ��λ�����ӣ�
								--EpromDataGu.DataNum.PumpDelay;
								if(EpromDataGu.DataNum.PumpDelay==0||EpromDataGu.DataNum.PumpDelay>10)EpromDataGu.DataNum.PumpDelay=10;
							break;
							case 15:
								--EpromDataGu.DataNum.OnceCXTime;
								if(EpromDataGu.DataNum.OnceCXTime<5||EpromDataGu.DataNum.OnceCXTime>240)EpromDataGu.DataNum.OnceCXTime=240;
							break;
							case 16://�Զ���ϴ���ʱ�䵥λ��
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
							case 19://�������֮��ͻ��Ϊ255�����Դ˴��ж�>35
								--EpromDataGu.DataNum.StartHeat;
								if(EpromDataGu.DataNum.StartHeat>35)EpromDataGu.DataNum.StartHeat=35;if(EpromDataGu.DataNum.StopHeat-5<EpromDataGu.DataNum.StartHeat)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 20:
								--EpromDataGu.DataNum.StopHeat;
								if((EpromDataGu.DataNum.StopHeat-5<EpromDataGu.DataNum.StartHeat)||EpromDataGu.DataNum.StopHeat>40)EpromDataGu.DataNum.StopHeat=40;
							break;
							case 21://���ȿ���ʱ��(����)���������һ��ԡ�ԵƼ�����ô��ʱ��δ�ﵽ���õ�ֹͣ�����¶ȣ��������ԡ�Եƽ��м���
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
					if(KeyValue&0x20)//������Ӽ��ͼ��ټ�ͬʱ����ʱ
					{
						AreaCodeKey=1;
					}
					else ShowEquipmentTime=6;
				}
			}
			else
			{
				if(JSKeyFlag)//��������
				{
					JSKeyFlag=0;SoundFlag=1;
					if(SetCnt==33)
					{
						AreaCnt++;
						if(AreaCnt>3)AreaCnt=0;
					}
				}AreaCodeKey=0;
			}
			if(KeyValue&0x20)//��������Ӽ�����
			{
				ZJKeyFlag=1;
				if(AutoChange)
				{AutoChange=0;
					if(EpromDataGu.DataNum.ParameterLock==0)//���ſ���󵥴γ�ˮ�����豸�����������Ϊ0���豸�ͷ��������ɵ��ڲ���
					{
						if(SetCnt>3&&SetCnt<9)SetPriceFlag=1;//����ǵ��ڵ��ۣ��������֮����Ҫ����0108�豸״̬ͬ��ָ��
						else if(SetCnt>25&&SetCnt<32)SetTimeFlag=1;//�������ʱ��
						else {}
						switch(SetCnt)
						{
							case 0:break;
							case 1:
								EpromDataGu.DataNum.YuKou+=10;
								if(EpromDataGu.DataNum.YuKou>990||EpromDataGu.DataNum.YuKou<10)EpromDataGu.DataNum.YuKou=10;
							break;
							case 2://������2A
							break;
							case 3://Ͷ���������Ӧ�����2AA//Ͷ����1һ�������Ӧ�����
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
							case 14://��ˮ��ʱ��λ�����ӣ�
								++EpromDataGu.DataNum.PumpDelay;
							if(EpromDataGu.DataNum.PumpDelay>10)EpromDataGu.DataNum.PumpDelay=1;
							break;
							case 15:
								++EpromDataGu.DataNum.OnceCXTime;
							if(EpromDataGu.DataNum.OnceCXTime>240||EpromDataGu.DataNum.OnceCXTime<5)EpromDataGu.DataNum.OnceCXTime=5;
							break;
							case 16://�Զ���ϴ���ʱ�䵥λ��
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
							case 19://���ÿ�ʼ�����¶�ʱ��ֹͣ�����¶��Զ��仯Ϊ��ʼ�����¶�+5��
								++EpromDataGu.DataNum.StartHeat;
							if(EpromDataGu.DataNum.StartHeat>35)EpromDataGu.DataNum.StartHeat=0;if((EpromDataGu.DataNum.StopHeat-5)<EpromDataGu.DataNum.StartHeat)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 20:
								++EpromDataGu.DataNum.StopHeat;
							if(EpromDataGu.DataNum.StopHeat>40)EpromDataGu.DataNum.StopHeat=EpromDataGu.DataNum.StartHeat+5;
							break;
							case 21://���ȿ���ʱ��(����)���������һ��ԡ�ԵƼ�����ô��ʱ��δ�ﵽ���õ�ֹͣ�����¶ȣ��������ԡ�Եƽ��м���
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
									case 0://���������λ����
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
				if(ZJKeyFlag)//��������
				{
					ZJKeyFlag=0;SoundFlag=1;
				}AreaCodeKey=0;
			}
			if(KeyValue&0x40)SZKeyFlag=1;//��������ü�����
			else
			{
				if(SZKeyFlag)//��������
				{
					SZKeyFlag=0;SoundFlag=1;
					if(TestFlag)while(1);
					else
					{
						if(SZFlag)
						{
							if(SetCnt==33){AreaIntToChar();SendData(CardAreaCode[0]);SendData(CardAreaCode[1]);}//˵����������������
							SetCnt++;
							if(SetCnt>32)
							{
								SetCnt=0;SZFlag=0;//DataB_A();WriteEeprom(SetDataAdd,38,SetParameter);
								WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //����
								if(SetPriceFlag){SetPriceFlag=0;ParameterFlag=1;}
								if(SetTimeFlag)
								{
									SetTimeFlag=0;
									iic_send_add_byte(0x00,0x02);Delay_k(1000);  //�ر�ʱ��
									time_init(Second,Minute,Hour,Day,Month,Year);
									pcf_init();Delay_k(1000);//����ʱ��
					
								}
							}
						}//���沢�˳�
						else {SZFlag=1;SetCnt=1;}
					}
				}
			}
//20201102���Ӳ���ģʽ		
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
//20201102���Ӳ���ģʽ
			Debouncing1ms=0;KeyFlag=0;
		}
	}
	else
	{
		KeyFlag=0;Debouncing1ms=0;KeyChange=1;DebouncingOk=0;//������°�������if(KeyValuetemp){}
		KeyValue=KeyValuetemp;//���仯��ļ�ֵ	
	}
}
void MenuShow()//�˵���ʾ
{
	switch(SetCnt)
	{
		case 0:
			
		break;
		case 1://1�ų�ˮ��Ԥ��1A1	1�ų�ˮ��ˢ��ʱʹ��
			DisBuf[10]=DispData[1];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.YuKou);
		break;
		case 2://1�ų�ˮ��1��ˮ��Ӧ������2A1		
//			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
//			DispSetData(EpromDataGu.DataNum.Pluse);
			SetCnt=3;
		break;
		case 3://1�ų�ˮ��һ�������Ӧ�����2AA
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.CoinPluse);
		break;
		case 4://1�ų�ˮ��ˢ������3A
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.CardPrice<1000)DisBuf[12]=DispData[Disp_A];else DisBuf[12]=DispData[EpromDataGu.DataNum.CardPrice/1000];
			DispSetData(EpromDataGu.DataNum.CardPrice%1000);
		break;
		case 5://1�ų�ˮ��ɨ�뵥��3A
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.VirtualPrice<1000)DisBuf[12]=DispData[Disp_B];else DisBuf[12]=DispData[EpromDataGu.DataNum.VirtualPrice/1000];
			DispSetData(EpromDataGu.DataNum.VirtualPrice%1000);
		break;
		case 6://1�ų�ˮ��Ͷ�ҵ���3Ac
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_A];if(EpromDataGu.DataNum.CoinPrice<1000)DisBuf[12]=DispData[Disp_C];else DisBuf[12]=DispData[EpromDataGu.DataNum.CoinPrice/1000];
			DispSetData(EpromDataGu.DataNum.CoinPrice%1000);
		break;
		case 7://�豸������4A
//			DisBuf[10]=DispData[4];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
//			DispSetData(EpromDataGu.DataNum.ParameterLock);
				SetCnt=8;
		break;
		case 8://1�ų�ˮ�ڵ���������ѽ��5A
			DisBuf[10]=DispData[5];DisBuf[11]=DispData[Disp_A];//DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.MaxCoin);
		break;
		case 9://ˢ����ˮ�ٶ�6A1
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.FlowRate1);
		break;
		case 10://ɨ���ˮ�ٶ�6A2
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.FlowRate2);
		break;
		case 11://Ͷ�ҳ�ˮ�ٶ�6A3
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_A];DisBuf[12]=DispData[3];
			DispSetData(EpromDataGu.DataNum.FlowRate3);
		break;
		case 12://������ʱ��Confiscate
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.Confiscate);
		break;
		case 13://������ʱ8a
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_A];
			DispSetData(EpromDataGu.DataNum.LedLock);
		break;
		case 14://��ˮ��ʱ1B
			DisBuf[10]=DispData[1];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.PumpDelay);
		break;
		case 15://��ϴʱ��
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OnceCXTime);
		break;
		case 16://��ϴ���
			DisBuf[10]=DispData[3];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.CXInterval);
		break;
		case 17://����ʱ��
			DisBuf[10]=DispData[4];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OnceOzoneTime);
		break;
		case 18://�������
			DisBuf[10]=DispData[5];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.OzoneInterval);
		break;
		case 19://��ʼ�����¶�
			DisBuf[10]=DispData[6];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.StartHeat);
		break;
		case 20://ֹͣ�����¶�
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_B];
			DispSetData(EpromDataGu.DataNum.StopHeat);
		break;
		case 21://���ȿ���ʱ��(����)���������һ��ԡ�ԵƼ�����ô��ʱ��δ�ﵽ���õ�ֹͣ�����¶ȣ��������ԡ�Եƽ��м���
			DisBuf[10]=DispData[7];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.HeatTime);
		break;
		case 22://���ƿ���ʱ����
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.StartLEDHour);
		break;
		case 23://���ƿ���������
			DisBuf[10]=DispData[8];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.StartLEDMinute);
		break;
		case 24://���ƹر�ʱ����
			DisBuf[10]=DispData[9];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[1];
			DispSetData(EpromDataGu.DataNum.StopLEDHour);
		break;
		case 25://���ƹرշ�����
			DisBuf[10]=DispData[9];DisBuf[11]=DispData[Disp_B];DisBuf[12]=DispData[2];
			DispSetData(EpromDataGu.DataNum.StopLEDMinute);
		break;
		case 26://��
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[1];
			DispSetData(Year);
		break;
		case 27://��
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[2];
			DispSetData(Month);
		break;
		case 28://��
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[3];
			DispSetData(Day);
		break;
		case 29://ʱ
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[4];
			DispSetData(Hour);
		break;
		case 30://��
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[5];
			DispSetData(Minute);
		break;
		case 31://��
			DisBuf[10]=DispData[Disp_F];DisBuf[11]=DispData[6];
			DispSetData(Second);
		break;
		case 32://�汾��
			DisBuf[10]=DispData[2];DisBuf[11]=DispData[0];DisBuf[12]=DispData[Disp_u];//DisBuf[13]=DispData[5];DisBuf[14]=DispData[0]|0x80;DisBuf[15]=DispData[1];
		DispSetData(Version);
		break;
		case 33://������
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
	if(SetCnt==1)//Ԥ����С����
	{
		DisBuf[13]=DispData[ch1];DisBuf[13]|=0x80;//Ԥ�۽��
		DisBuf[14]=DispData[ch2/10];
	}
	if(SetCnt==4||SetCnt==5||SetCnt==6)DisBuf[14]|=0x80;
	if(SetCnt==9||SetCnt==10||SetCnt==11||SetCnt==32)//��ˮ�ٶȣ��汾��
	{
		DisBuf[14]|=0x80;//��ˮ�ٶ�
		DisBuf[14]|=DispData[ch2/10];
	}
}
void AdvertisingLamp()//�����ж�//������������ʵ��Ż�������,��������ִ�к�����
{
	if((EpromDataGu.DataNum.StartLEDHour*60+EpromDataGu.DataNum.StartLEDMinute)>(EpromDataGu.DataNum.StopLEDHour*60+EpromDataGu.DataNum.StopLEDMinute))//�������ʱ����ڹر�ʱ�䣬��˵���ر����ڵڶ���رյ�
	{
		if(((Hour*60+Minute)>(EpromDataGu.DataNum.StartLEDHour*60+EpromDataGu.DataNum.StartLEDMinute))||((Hour*60+Minute)<(EpromDataGu.DataNum.StopLEDHour*60+EpromDataGu.DataNum.StopLEDMinute)))//||(GetADCResult(7)<0x40)�Ȱ�������촦������ͻ��������ټ�
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
void HeatingControl()//���ȿ��ƣ���������ִ�к�����
{
	if(Temperature>EpromDataGu.DataNum.StopHeat)//���¶ȸ���ֹͣ�¶�
	{
		Heat1=0;Heat2=0;HeatFlag=0;HeatTime1s=0;//SpeedHeat=0;
	}
	else
	{
		if(Temperature<=EpromDataGu.DataNum.StartHeat)//���¶ȵ��ڿ�ʼ�����¶�ʱ������ԡ��1���м���
		{
			if(HeatFlag==0){HeatTime1s=EpromDataGu.DataNum.HeatTime*60;}HeatFlag=1;
			Heat1=1;
		}
		if(HeatTime1s)
		{
			--HeatTime1s;if(HeatTime1s==0)Heat2=1;
		}
//		if(SpeedHeat)//���
//		{
//			Heat1=1;
//		}
	}
}
/*
*pData��ſ�������
Value ��ֵ�Ľ��ֵ
Type	��ֵ����		0x00 ����ֵ��0x01����ֵ��0x02���㣬0x03����
*/
void RechargeCard(uchar *pData,ulong Value,uchar Type)//���߳�ֵuchar *pUID,
{
	ulong ulong1;//,ulong2
		ulong1=ReadMoney(pData);//�������ڽ��
//		ulong2=ulong1;
		switch(Type)//�жϳ�ֵ����
		{
			case 0://����ֵ
				ulong1+=Value;ulong1+=Balance;
				if(ulong1>999999)ulong1=999999;
			break;
			case 1://����ֵ
				if(ulong1>Value){ulong1-=Value;ulong1+=Balance;}
				else ulong1=0;
			break;
			case 2://����
				ulong1=0;
			break;
			case 3://����
				ulong1=Value;
			break;
			default:break;
		}
		SaveMoney(ulong1,pData);
		if(WriteCard4_6(pData)==MI_OK)//��ֵ�ɹ�
		{
			RechargeOk=1;RechargeFinishMoney=ulong1;//��ֵ��ϣ�ע��ȡ��//��ֵ����
//			SendData(RechargeFinishMoney/0x1000000);SendData(RechargeFinishMoney/0x10000%0x100);SendData(RechargeFinishMoney/0x100%0x100);SendData(RechargeFinishMoney%0x100);SendData(Balance/0x100);SendData(Balance%0x100);SendData(RechargeOk);
			WriteCardOk=1;
		}
//		else RechargeFinishMoney=ulong2;//��ֵʧ�ܣ���ֵ������ڳ�ֵǰ���
	
}


void SellWater()
{
	if(Balance&&SellState==2)//�������ڳ�ˮ״̬��
	{
		SellWater1ms++;Confiscate=0;//��ˮ�����н�������ʱ������
//		SellWater10ms++;
		if(WaterLevel==6)OutWater10ms++;//P6״̬��ˮ��ʱ������,��Ϊ���۾��ȵ����ˣ�����10ms��Ϊ1ms��
		if(OutWater10ms>=1000){OutWater10ms=0;OutWater1s++;}
//		if(SellWater10ms>=WaterPrice)
		if(SellWater1ms>=WaterPrice)
		{
			SellWater1ms=0;--Balance;Expenditure++;TotalWater+=WaterFlowRate;DelayShowTime=3;//EpromDataGu.DataNum.FlowRate;if(CardFlag){TotalWater+=EpromDataGu.DataNum.FlowRate1;DelayShowTime=3;}//�����ˢ����ˮ��ʾ��ˮ��
			if(Balance==0)//��������꣬��Ҫ�ϴ������嵥
			{
				GZPump=0;Order0104=1;SellState=4;Audio_Play(3);LedLockTime=EpromDataGu.DataNum.LedLock;//
				if(AnliangFlag==0)ShowMoney=0;//������ǰ���ȡˮ�������ʱ����ʾ�������
			}
		}
	}
//	else GZPump=0;
}
//*********************����****************************//
void OzoneControl()
{
//���������߼�
	if(OzoneTime){--OzoneTime;Ozone=1;}//��������ɱ��
	else {++OzoneTime1s;Ozone=0;}
	if(OzoneTime1s>=60)
	{
		OzoneTime1s=0;OzoneTime1m++;
		if(OzoneTime1m>EpromDataGu.DataNum.OzoneInterval)//�������ʱ�䵽����Ҫ��������
		{
			OzoneTime1m=0;OzoneTime=EpromDataGu.DataNum.OnceOzoneTime;
		}
	}
//���������߼�
}
//*********************����****************************//
//���������򱸷�������:0=�ɹ�,1=ʧ��
uchar ReadCard4_6(uchar *CardBuff)
{	uchar ch1=0,MainStatus=0;

  MainStatus=PcdRead(4,CardBuff);        //����4��
  if(MainStatus) return 1;  //����ʧ�ܷ��ش���
  //0�������ж�
  if((*(CardBuff+9)==0xff)&&(*(CardBuff+10)==0xff)
				&&(*(CardBuff+11)==0xff)&&(*(CardBuff+12)==0xff))	 ch1=0;     //�������ȫΪff
	else ch1=*(CardBuff+9)|*(CardBuff+10)|*(CardBuff+10)|*(CardBuff+10);  //��鿨���ǲ���ȫ0
  if(ch1==0)											//�����һ�����ݲ���ȷ
	{	MainStatus=PcdRead(6,CardBuff);       //����һ������3������
    if(MainStatus) return 1;  //����������ʧ�ܷ��ش���
		if((*(CardBuff+9)==0xff)&&(*(CardBuff+10)==0xff)
  	  &&(*(CardBuff+11)==0xff)&&(*(CardBuff+12)==0xff))	ch1=0;  //�������ȫΪff
    else ch1=*(CardBuff+9)|*(CardBuff+10)|*(CardBuff+11)|*(CardBuff+12);
    if(ch1==0)    return 1;              //���������ݴ��ش���
//    KuaiNumOk=2;   //�ڶ����
  }
//	else	KuaiNumOk=1;	 //��һ���
	 return 0;
}
//��LMoneyNum�Ľ��ֲ�ͬ�Ŀ�(���Ͽ�),д�뿨�ڵ�4��6��
uchar WriteCard4_6(uchar *CardBuff)
{ uchar MainStatus=0;	
 	
	MainStatus=PcdWrite(4,CardBuff);
//	if(MainStatus)PcdWrite(4,CardBuff);
	PcdWrite(6,CardBuff);
//  KuaiNumOk=0;
  return MainStatus;
}
/*
�Ӵ�������������ȡ���
*/
ulong	ReadMoney(uchar *pData)
{
	bit bit1;
	ulong ulong1;
	ulong1=*(pData+1)%0x10;//ȡ������λ
	if(*(pData+9)>=0x9f)bit1=0;//�¿�
	else bit1=1;
	if(bit1)
	{
		ulong1*=10000;
		ulong1+=bcd_dec(*(pData+2))*100+bcd_dec(*(pData+3));
//							if(CardMoney>60000) {SoundFlag=5;break;}
		
	}
	else//�Ͽ�
	{//
//		ulong1=ulong1*0x10000+*(pData+2)*0x100+*(pData+3);
		ulong1*=0x10000;
		ulong1+=*(pData+2)*0x100+*(pData+3);
//		ulong1+=*(pData+3);
//			ulong1=ArrayMoney(pData+1);
//		MoneyArray(ulong1,pData+1);
//							if(CardMoney>999999){SoundFlag=5;} //�������⿨������ʾ
		
	}
	return ulong1;
}
/*
�����ת��Ϊ���ڴ�����ݸ�ʽ
MoneyΪ��Ҫת���Ľ��
pDataΪת��֮���ŵĵ�ַ
*/
void SaveMoney(ulong Money,uchar *pData)
{
	uchar ch1;
	uint int1;
	if(*(pData+9)<0x9f)//������Ͽ�
	{
		if(Money>60000) Money=60000;
		ch1=Money/10000; //���λ;           //����λ
		*(pData+1)&=0xf0;               //�������λ
		ch1&=0x0f;                      //����:���ε�������λ
		*(pData+1)|=ch1;																//������Ͱ�λ
		int1=Money%10000;
		ch1=int1/100;
		*(pData+2)=dec_bcd(ch1); //���ʮ��λ
		ch1=int1%100;
		*(pData+3)=dec_bcd(ch1); //���Ƿ�λ
	}
	else
	{
		ch1=(uchar)(Money/0x10000); //���λ;           //����λ
		*(pData+1)&=0xf0;               //�������λ
		ch1&=0x0f;                      //����:���ε�������λ
		*(pData+1)|=ch1;																//������Ͱ�λ
		int1=Money%0x10000;
		*(pData+2)=int1/0x100; //���ʮ��λ
		*(pData+3)=int1%0x100; //���Ƿ�λ
	}
}
/*
//-------------------------------
//���������뷢�͸�������
//  1��������<99ʱ,�������ڿ����ݵ�һ�ֽ�(01)��BCD��
//  2�����������99С��200ʱ,�������ڿ�����ǰ1.5�ֽ�(012)�����ֽڴ�0xA6��ʼ����,��λ��0��ʼ����:
//                        ������100=0xA60n(n�������λ),101=0xA61n,....115=0xA6fn,116=0xA70n,
//  3�����������199ʱ���������ÿ����ݵ�һ�͵����ֽ�(0189)����һ�ֽ�Ϊint�ĸ�λ�������ֽ�Ϊint�ĵ�λ�����ֽڴ�0xC8��ʼ�����ֽڴ�0��ʼ
//                          ������200=0xc800,201=0xc801........һֱ��9999=0xee47.
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
//���α����������Ϊ�����Σ�0-99,100-199,200-9999���������ֽڵ�
void AreaIntToChar()
{ uint int1;
  if(EpromDataGu.DataNum.AreaCode>199)
		{ int1=0xc738;int1+=EpromDataGu.DataNum.AreaCode;
    CardAreaCode[0]=(uchar)(int1/0x100);         //ϵͳ���ݸ�Ϊ�޸ĺ������
    CardAreaCode[1]=(uchar)(int1%0x100);         //ϵͳ���ݸ�Ϊ�޸ĺ������
  }
  else
  { if(EpromDataGu.DataNum.AreaCode>99)               //������>99ʱ:(������ֵ-100)+0xa60����(������ֵ+0x9fc)
    { int1=0x9fc;int1+=EpromDataGu.DataNum.AreaCode;
      CardAreaCode[0]=(uchar)(int1/0x10);         //ϵͳ���ݸ�Ϊ�޸ĺ������
      CardAreaCode[1]=(uchar)(int1%0x10);         //ϵͳ���ݸ�Ϊ�޸ĺ������
			CardAreaCode[1]*=0x10; 
    }
    else                         //������<100ʱֱ�ӱ�ΪBCD��
    { 
			CardAreaCode[0]=dec_bcd((uchar)EpromDataGu.DataNum.AreaCode);
      CardAreaCode[1]=0;
    }
	}
}
/*
IC���ڴ洢�Ŀ���תΪͨѶ���Ž��д洢char�洢
*/
void CardCode_Char(uchar *pCode,uchar *pICcode)
{
//	uchar	ch1;
	uint int1;
	ulong long1=0;
	CardType=Code_CardType(pICcode);
	if((*pICcode)>0x9f)//�¿�
	{
		*pCode=*pICcode&0x0f;*(pCode+1)=*(pICcode+1);*(pCode+2)=*(pICcode+2);*(pCode+3)=*(pICcode+3);
	}
	else//�Ͽ�
	{
		long1=(ulong)bcd_dec(*pICcode)*1000000+(ulong)bcd_dec(*(pICcode+1))*10000+bcd_dec(*(pICcode+2))*100+bcd_dec(*(pICcode+3));//�˴������ǿ������ת�����������������������
		int1=long1/0x10000;Int_Array(int1,pCode);int1=long1%0x10000;Int_Array(int1,pCode+2);
	}
}
