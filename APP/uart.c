//生成固定的104数据包
void Make104Data(uchar Str[])
{
//	uchar ch1;
	memset(Str,0,50);//初始化发送缓冲区
//  for(ch1=0;ch1<50;++ch1) Str[ch1]=0;
	Str[0]=0x7e;Str[3]=0x20;  Str[4]=SheClass;        //版本
	Str[2]=48;Str[7]=0x10;Str[9]=1;Str[10]=4;//数据帧头
	memcpy(&Str[11],Cdatas,4);  //取设备序列号
//	PushZhenNum(Str);//放入帧序
	memcpy(&Str[15],CardCode,4);  //取卡号
	Str[19]=Year;Str[20]=Month;Str[21]=Day;Str[22]=Hour;Str[23]=Minute;Str[24]=Second;//加入触发时间
	Str[43]=0xff;Str[44]=0xff;
	if(ReadCardData)Str[45]=0x03;
	else Str[45]=0x1f;
	PushZhenNum(Str);//0104数据包放入帧序，用于核销结算数据包
}
void FrameHead()//固定帧头
{
	S2TransBuf[0]=0x7e;S2TransBuf[3]=0x20;S2TransBuf[4]=SheClass;S2TransBuf[7]=0x10;S2TransBuf[9]=1;
	//头									协议版本						设备类型								出水口
}
void ConsumeDataPackage(uchar Str[])//发送消费数据包
{
	Make104Data(Str);
	if(CardFlag)Str[25]=0x01;//如果是刷卡消费
	else if(CoinFlag)Str[25]=0x02;
	else if(AnliangFlag||RemoteFlag)Str[25]=0x03;
	else {}
	if(TotalMoney==0){CardFlag=0;}//如果消费前金额为0，则说明是卡上无金额的水卡刷卡数据，需要清除刷卡打水标志
	MoneyArray(TotalMoney,&Str[26]);//放入消费前金额
	if(Expenditure)//如果本次消费金额不为0
	{
		if(ConfiscateFlag){Str[29]=0x02;}//如果是没收
		else Str[29]=0x01;//正常消费
		Int_Array(Expenditure,&Str[30]);//放入本次消费金额
		SaveLiData=1;ZhenBakH=Str[5];ZhenBakL=Str[6];//保存历史数据标志置位，收到0204回复包之后清除，如果固定时间内或者下次结算数据时，没有收到回复，则直接保存为离线数据。
		
	}
//	if(ReadCardData)Str[29]=0;//如果是读卡数据
//	else//结算数据
//	{
//		
//	}
	if(CardType)Str[42]=CardType;
//	SendDataPackage(Str);
}
void ClearTemp()//清除参数表，用于结算是清除消费过程中的变量参数以及标志位
{
	CardFlag=0;CoinFlag=0;RemoteFlag=0;AnliangFlag=0;ConfiscateFlag=0;Confiscate=0;YuKouOk=0;
	SellState=0;TotalMoney=0;CardType=0;memset(CardCode,0,4);Expenditure=0;Balance=0;//SoundFlag=5;//清除售水状态，消费前金额，本次消费金额，卡类型，卡号Expenditure=0;
	MaxMoneyFlag=0;
}
void SendDataJoint()//发送数据拼包
{
	SendDataInterval=DataInterval;//主动发送的数据都需要有2s的间隔时间
	memset(S2TransBuf,0,110);//初始化发送缓冲区
	if(Order0104)//如果是发送0104数据指令
	{
		Order0104=0;
		if(Expenditure&&SaveLiData&&NetWork){SaveLiData=0;JiTimerDelay();}//SoundFlag=3;如果本次有消费金额并且保存数据标志被置位则保存上次的消费记录,在线状态下才这样处理，不在线时直接就存为离线数据了
		ConsumeDataPackage(S2TransBuf);ZhenNumFlag=1;//已经放入帧序了，后面不用重新放入帧序
		if(Expenditure)//如果本次有消费金额
		{
			SaveExpenditureData();//将消费数据转换为离线数据格式存放，暂不保存,
			if(NetWork)
			{
				SaveLiData=1;SaveLiData1s=5;//将保存数据标志置位,等待回复时间设为5秒，到时间未收到回复直接保存
			}
			else JiTimerDelay();//将消费数据转换为离线数据格式，并保存离线数据
		}
		ClearTemp();
	}//注意：本次消费金额为0时不发送结算包给服务器
	else if(TimingFlag)//校时指令拼包0103,上电联网之后校时一次
	{
//		SoundFlag=1;
		TimingFlag=0;
		S2TransBuf[2]=14;S2TransBuf[10]=3;S2TransBuf[11]=1;//请求北京时间
	}
	else if(ParameterFlag)//设备参数指令0108,每小时发一次，每个出水口发送一次
	{
//		SoundFlag=3;
		ParameterFlag=0;
		S2TransBuf[2]=44;S2TransBuf[10]=8;S2TransBuf[15]=EpromDataGu.DataNum.FlowRate1;S2TransBuf[17]=EpromDataGu.DataNum.OnceCXTime;S2TransBuf[18]=EpromDataGu.DataNum.CXInterval;//冲洗间隔时间
		S2TransBuf[19]=EpromDataGu.DataNum.OnceOzoneTime;S2TransBuf[20]=EpromDataGu.DataNum.OzoneInterval;//臭氧工作时间
		S2TransBuf[21]=EpromDataGu.DataNum.StartHeat;S2TransBuf[22]=EpromDataGu.DataNum.StopHeat;//温控
		S2TransBuf[23]=EpromDataGu.DataNum.StartLEDHour;S2TransBuf[24]=EpromDataGu.DataNum.StartLEDMinute;//广告灯开启时间
		S2TransBuf[26]=EpromDataGu.DataNum.StopLEDHour;S2TransBuf[27]=EpromDataGu.DataNum.StopLEDMinute;//广告灯关闭时间
		S2TransBuf[31]=EpromDataGu.DataNum.MaxCoin;
		S2TransBuf[7]=0x10;
		Int_Array(EpromDataGu.DataNum.YuKou,&S2TransBuf[29]);//流量计脉冲数
		Int_Array(EpromDataGu.DataNum.CoinPluse,&S2TransBuf[32]);//硬币器脉冲对应金额数
		Int_Array(EpromDataGu.DataNum.CardPrice/10,&S2TransBuf[34]);//刷卡单价
		Int_Array(EpromDataGu.DataNum.VirtualPrice/10,&S2TransBuf[36]);//扫码单价
		Int_Array(EpromDataGu.DataNum.CoinPrice/10,&S2TransBuf[38]);//投币单价
		ParameterTime=ParameterInterval;
	}
	else if(ZSTimeFlag)
	{
		ZSTimeFlag=0;PushZhenNum(S2TransBuf);ZhenNumFlag=1;S2TransBuf[2]=18;S2TransBuf[10]=7;Int_Array(ZSTime1h,&S2TransBuf[15]);//放入制水时间
//		FrameH=S2TransBuf[5];FrameL=S2TransBuf[6];
	}
	else if(StateFlag)//状态包0105，10分钟发送一次，每个出水口发送一次有状态改变立即发送一次
	{
//		SoundFlag=5;
		StateFlag=0;
		S2TransBuf[2]=30;S2TransBuf[10]=5;
		S2TransBuf[17]=EpromDataGu.DataNum.OnceCXTime;S2TransBuf[20]=Temperature;//臭氧工作时间，机箱温度
		if(TDS_array[0]>TDS_array[1])//TDS防反插
		{
			Int_Array(TDS_array[0],&S2TransBuf[21]);//原水TDS
			Int_Array(TDS_array[1],&S2TransBuf[23]);//净水TDS
		}
		else
		{
			Int_Array(TDS_array[1],&S2TransBuf[21]);//原水TDS
			Int_Array(TDS_array[0],&S2TransBuf[23]);//净水TDS
		}
		S2TransBuf[25]=WorkStateByte;//工作状态字节
//		Int_Array(ZSTime1h,&S2TransBuf[26]);//累计制水时间
		S2TransBuf[28]=CommVal;//信号强度
		S2TransBuf[7]=0x10;
		Int_Array(EpromDataGu.DataNum.YuKou,&S2TransBuf[15]);//预扣
		Int_Array(EpromDataGu.DataNum.CardPrice/10,&S2TransBuf[18]);//刷卡单价
		StateTime=StateInterval;
	}
	else if(HeartFlag) //心跳包优先级最低0101	25秒发送一次
	{
		S2TransBuf[2]=16;S2TransBuf[10]=1;HeartFlag=0;HeartTime=HeartInterval;//长度，数据类型
	}
	else return;//没有数据要发送，则跳出
	HeartCnt++;//发送一个数据包计数器增加一次
	if(HeartCnt>6){HeartCnt=0;NetProce=0;PowerDown=0;M800Power=0;NetWork=0;}//6次数据包未收到回复则断电，重新配网
	FrameHead();
	memcpy(&S2TransBuf[11],Cdatas,4);//放入设备号
	if(ZhenNumFlag==0)PushZhenNum(S2TransBuf);//放入帧序
	ZhenNumFlag=0;
	PushCRC(S2TransBuf);//放入CRC
	if(S2TransBuf[9]==1&&S2TransBuf[10]==4)//如果发送的是0104指令，并且本次消费金额不为0，则保存为离线数据格式
	{
		if(S2TransBuf[30]==0&&S2TransBuf[31]==0)return;//如果本次消费金额为0,则不发送给服务器
	}
	SendFixedLength(S2TransBuf);
}
void Uart2() interrupt 8
{
  uchar DATA_uart;
  if (S2CON & S2RI)     //RI
  { S2CON &= ~S2RI;         //清除S2RI位
    DATA_uart=S2BUF;
    if(NetProce<0x50) {S2RecBuf[S2RecCount++]=DATA_uart;if(S2RecCount>=100)S2RecCount=0;}  //如果断网
    else                 //如果在接受数据状态
    { if(S2Begin==3)              //4、接收
      { S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
        --S2DataLen;
				if(S2DataLen==0)
        { S2Begin=0;S2RecCount=0;//S2ProceFlag=1;S2RecFlag=0;
					S2RecNum++;if(S2RecNum>7)S2RecNum=0;//接收完一条数据，下一条存放在接收缓存的下一行
          return;
        }
      }
      if(S2Begin==2)               //3、找长度
      { ++S2Begin; S2DataLen=DATA_uart-2; 
        S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
      }
      if(S2Begin==1)               //2、找0
      { if(DATA_uart==0)
        { ++S2Begin;
          S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
        }
        else 
        { S2Begin=0;S2RecCount=0;}
      }
      if(S2Begin==0)                //1、先找7e
      { if(DATA_uart==0x7e) 
        { S2Begin=1;S2RecCount=0;
					S2RecDataBuf[S2RecNum][S2RecCount]=DATA_uart; ++S2RecCount;
//					if(S2ProceFlag){S2RecBuf[S2RecCount++]=DATA_uart;TwoPackage=1;}
//					else {S2RecBuf[S2RecCount]=DATA_uart; ++S2RecCount;}//如果上次数据处理结束了，则接收计数器清零，否则不清零
          
        }
      }
      S2RecFlag=1;S2CharSpace=0;      //接收标志和字符中间的间隔清零,为无传输字符退出做准备
			if(S2RecCount>=49) S2RecCount=0;//如果接收的是通讯数据，则每条最多接收50个字节
    }
    /*缓冲区满,所有接受到的数据就保存到最后一个字节，保证粘包时第一条数据可以处理正确*/
//王雷雨修改
//		 if(S2RecCount>100) S2RecCount=0;
  }
  if (S2CON & S2TI)
  {
    S2CON &= ~S2TI;         //清除S2TI位
    busy2 = 0;               //清忙标志
  } 
}

/*----------------------------
发送串口数据
----------------------------*/
void Uart2SendData(uchar dat)
{
    while (busy2);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    busy2 = 1;
    S2BUF = ACC;                //写数据到UART2数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void Uart2SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        Uart2SendData(*s++);         //发送当前字符
    }
}
uchar Code_CardType(uchar *pCode)
{
	uchar ch1;
	ch1=*pCode/0x10;
	if(ch1<=9)return 1;//老卡返回1
//	else if(ch1>100&&ch1<200)return ch1;
	else
	{
		switch(ch1)
		{
			case 0x0a:
				return 0;
			break;
			case 0x0b:
				return 2;
			break;
			case 0x0c:
				return 3;
			break;
			case 0x0d:
				return 4;
			break;
			case 0x0e:
				return 5;
			break;
			case 0x0f:
				return 6;
			break;
			default:break;
		}
	}
	return 0;
}
/*
传入一个long型数据，转换为三个char型数据
用于把金额转换成通讯发送的格式
*/
void MoneyArray(ulong money,uchar *pData)
{
	uint int1;
	*pData=money/0x10000%0x100;
	int1=money%0x10000;
	*(pData+1)=int1/0x100;
	*(pData+2)=int1%0x100;
}
ulong ArrayMoney(uchar *pData)
{
	ulong long1=0;
	long1=(ulong)*pData*0x10000+*(pData+1)*0x100+*(pData+2);
	return long1;
}
void Int_Array(uint int1,uchar *pArray)
{
	*pArray=int1/0x100;
	*(pArray+1)=int1%0x100;
}
uint Array_Int(uchar *pArray)
{
	uint int1=0;
	int1=*pArray*0x100+*(pArray+1);
	return int1;
}
uchar SimStr[21];
uchar SearchID()     //查找卡ID号
{ uchar ch1,ch2,Error;

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
            Error=0;//return 0;
          }
        }
      }
    }
  } 
  if(Error)
	{
		NetProce=0;
	}
	return Error;
//	strcpy(SimStr,"ERROR");
}
//                     头    长   版本 设类  帧序号    保   留  数据类型
uchar code Str102[]={0x7e,0,12+32,0x20,SheClass,0x00,0x00,0x10,0x00,0x01,0x02};
void InitModel()//放在秒处理函数里
{ 
	uchar ch1,ch2;
	uint int1;
	ch1=NetProce&0xf0;       //高位=1寻网,=2设置透传,=3设置IP,=4成功
  switch(ch1)
	{
		case 0:          //断电送电进程
      ch2=NetProce&0x0f;
			if(ch2==2||ch2==3)
			{
				Uart2SendString("AT\x0d\x0a");
			}
			switch(ch2)
			{
				case 1:
					Uart2SendString("+++"); 
				break;
				case 4:
					AutoReconnection++;
				if(AutoReconnection>=6)//6次没有联网成功，则给模块断电重启
				{
					AutoReconnection=0;
//暂时屏蔽
					PowerDown=0;M800Power=0;NetWork=0;//断电计时器
				}
				Uart2SendString("AT+CIPSHUT\x0d\x0a"); 
				break;
				case 5:
					memset(S2RecBuf,0,110);//初始化接收缓冲区
					S2RecCount=0;    //for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;接收区清零  //为接收做好准备  
					Uart2SendString("AT+CCID\x0d\x0a");
					
				break;
				case 7:
					if(SearchID()==0)     //查找卡ID串号
					{
						RorS_Flag=0; NetProce=0x10;  //进入发送命令进程
					}
				break;
				default: break;
			}
			++NetProce;
    break;
    case 0x10:                  //寻网
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //接收区清零
				memset(S2RecBuf,0,110);//初始化接收缓冲区
//				RestData(S2RecBuf);
        S2RecCount=0;      //为接收做好准备
				Uart2SendString("AT+CSQ\x0d\x0a");//发送寻网命令
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
				Uart2SendString("AT+CIPMODE=1\x0d\x0a");//发送透传命令
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
      { //strcpy(S2TransBuf,"AT+CIPSTART=\"TCP\",\"39.106.74.186\",9999\x0d\x0a"); S2TransLen=40; //发送IP
                         //012345678901 2345 67 8
        strcpy(S2TransBuf,"AT+CIPSTART=\"TCP\",\"");
        //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21  22 23 24 25 26  
        //n 0 3 9 . 1 0 6 . 0 7  4  .  1  8  6 \"                  0  9   9  9  9   
        ch1=IportBuf[0];
        memcpy(&S2TransBuf[19],&IportBuf[1],ch1);
        ch1+=19;
        S2TransBuf[ch1]='"';
        ++ch1;S2TransBuf[ch1]=',';
        ++ch1;
        memcpy(&S2TransBuf[ch1],&IportBuf[22],5);    //端口
        ch1+=5;S2TransBuf[ch1]=0x0d;
        ++ch1;S2TransBuf[ch1]=0x0a;
        ++ch1;
        S2TransLen=ch1;	
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //接收区清零
				memset(S2RecBuf,0,110);//初始化接收缓冲区
        S2RecCount=0;      //为接收做好准备
				for(ch1=0;ch1<S2TransLen;++ch1)
				{
					Uart2SendData(S2TransBuf[ch1]);
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
//        for(ch1=0;ch1<100;++ch1) S2TransBuf[ch1]=0;
				memset(S2RecBuf,0,110);//初始化接收缓冲区
        memcpy(S2TransBuf,Str102,11);  //登陆包头
        memcpy(&S2TransBuf[11],Cdatas,4);  //设备号
        S2TransBuf[4]=SheClass;               //设备类型	多出水口售水机时间计费型
        memcpy(&S2TransBuf[15],SimStr,20);    //Sim字串
//王雷雨修改，修改登陆包中历史数据条数
//				if((ShanNum*30+LuNum)>(SendShanNum*30+SendLuNum))//如果保存的数据条数大于发送的数据条数，则剩余的数据条数等于保存的-发送成功的
//				{
//					int1=(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
//				}
//				else
//				{
//					int1=0;ShanNum=0;LuNum=0;SendShanNum=0;SendLuNum=0;
//				}
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
							if(SaveFull)
							{
								int1=(MaxShan+1)*30+(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
								if(int1>((MaxShan+1)*30))int1=(MaxShan+1)*30;
							}
							else
							{
								if((ShanNum*30+LuNum)>(SendShanNum*30+SendLuNum))//如果保存的数据条数大于发送的数据条数，则剩余的数据条数等于保存的-发送成功的
								{
									int1=(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
								}
								else
								{
									int1=0;ShanNum=0;LuNum=0;SendShanNum=0;SendLuNum=0;
								}
							}
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
//        int1=ShanNum;int1*=32;int1+=LuNum;
//王雷雨修改
        S2TransBuf[35]=EpromDataGu.DataNum.AreaCode/0x100;
        S2TransBuf[36]=EpromDataGu.DataNum.AreaCode%0x100;
        S2TransBuf[37]=int1/0x100;                              
        S2TransBuf[38]=int1%0x100;                              
        S2TransBuf[39]=EpromDataGu.DataNum.StartHeat;
        S2TransBuf[40]=EpromDataGu.DataNum.StopHeat;
        PushZhenNum(S2TransBuf);
        PushCRC(S2TransBuf);              
//        S2TransLen=S2TransBuf[2]+1;	//S2TransCount=0;
        NetOverTime=0;     //重新开始心跳计时
//        for(ch1=0;ch1<110;++ch1) S2RecBuf[ch1]=0;  //接收区清零
				memset(S2RecBuf,0,110);//初始化接收缓冲区
        S2RecCount=0;      //为接收做好准备
//				for(ch1=0;ch1<S2TransLen;++ch1)
//				{
//					Uart2SendData(S2TransBuf[ch1]);
//				}
				SendFixedLength(S2TransBuf);
        RorS_Flag=1;
      }
      else
      { 
//				if(CompCRC(S2RecBuf))return;//如果CRC不对，不处理退出
				if((S2RecBuf[0]==0x7e)&&(S2RecBuf[9]==2)&&(S2RecBuf[10]==2)&&(CompCRC(S2RecBuf)==0))   //如果是回复的202
        {	NetProce=0x50;	NetWork=1;							//
          NetOverTime=0;     //重新开始心跳计时
//					if(YuanipFlag)          //如果是新IP
//          { YuanipFlag=0;
//            WriteEeprom(SysIpPort,27,IportBuf);  
//          }
					NetProce=0x50;TimingFlag=1;HeartFlag=1;ParameterFlag=1;ZSTimeFlag=1;//上电后所有主动发送的数据都上传一遍
	//					SendData(TimingFlag);SendData(ParameterFlag);
					if(EpromDataGu.DataNum.AreaCode!=(S2RecBuf[15]*0x100+S2RecBuf[16]))
					{
						
						EpromDataGu.DataNum.AreaCode=S2RecBuf[15]*0x100+S2RecBuf[16];//如果服务器端的区域码和设备上 区域码不同则修改
						WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //保存
						AreaIntToChar();//读出区域码
					}
          if(YuanipFlag)          //如果是新IP
          { YuanipFlag=0;
            WriteEeprom(SysIpPort,27,IportBuf); SoundFlag=3; 
          }
        }
        else
        { ++NetProce;
          ch2=NetProce%0x10;
          if(ch2>10)
          { NetProce=0;
            if(YuanipFlag)
            { ++NewIPTime;
              if(NewIPTime>3) while(1);    //如果3次不成功,放弃转网的IP
            }
          }
        }          
      } 
      break;
	}
}

//在数组中放入CRC-----------------------
void PushCRC(uchar Str[])
{ uchar ch1;
  uint int1;
  ch1=Str[2];    //长度
  ch1-=2;        //CRC长度
  int1=CRC_Calculate(ch1,&Str[1]);  //计算CRC
  ++ch1;        //指向第一个CRC
  Str[ch1]=(uchar)(int1/0x100);
  ++ch1;
  Str[ch1]=(uchar)(int1%0x100);
}
//在数组中放入帧序号--------------------------
void PushZhenNum(uchar Str[])
{  Str[5]=(uchar)(ZhenNum/0x100);
   Str[6]=(uchar)(ZhenNum%0x100);
   ++ZhenNum;            //下一次的帧序
}
//=======================================
//计算CRC------------------------------------
uint CRC_Calculate(uchar Length,uchar *Address)
{ uchar i,j;
  uint  CRC;
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
bit CompCRC(uchar Str[])
{ uchar ch1,ch2;
  uint int1;

  if(Str[0]!=0x7e) return 1;  //如果包头不对
  ch1=Str[2];                 //发送的长度
  ch1-=2;                     //CRC的长度
  int1=CRC_Calculate(ch1,&Str[1]); //计算CRC
  ch2=(uchar)(int1/0x100);    //CRC高字节
  ++ch1;                      //指向CRC第一字节
  if(ch2!=Str[ch1])  return 1; //CRC第一字节未通过
  ++ch1;
  ch2=(uchar)(int1%0x100);     //CRC低字节
  if(ch2!=Str[ch1])  return 1; //CRC第2字节未通过
  return 0;
}
//-----------------------------------------------------------------
//                  C51中字符串函数的扩充            
// 函数名称    : strsearch ()
// 函数功能    : 在指定的数组里连续找到相同的内容
// 入口参数   ： ptr2=要找的内容, ptr1 当前数组
// 出口参数 ： 0-没有 找到   >1 查找到
//----------------------------------------------------------------
uchar strsearch(uchar *ptr2,uchar *ptr1_at)//查字符串*ptr2在*ptr1中的位置
//本函数是用来检查字符串*ptr2是否完全包含在*ptr1中
//返回:  0  没有找到
//       1-255 从第N个字符开始相同
{ uchar i,j,k;

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

void LostConnection()//掉线结算数据处理
{
	uint int1;
	if(Order0104)//如果有结算数据&&Expenditure
	{
		Order0104=0;
		if(Expenditure)//如果本次消费金额不为0，则保存离线数据
		{
			RamDataLS.DataNum[LuNum].TypeFlag=0;
			if(CardFlag||AnliangFlag)
			{
				memcpy(&RamDataLS.DataNum[LuNum].CardCode,CardCode,4);RamDataLS.DataNum[LuNum].Cardtype=CardType;
				if(AnliangFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
				else RamDataLS.DataNum[LuNum].TypeFlag|=0x10;
				
			}////放入卡号如果是刷卡打水
			else if(RemoteFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
			else RamDataLS.DataNum[LuNum].TypeFlag|=0x20;//投币打水
	//		else RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
			if(ConfiscateFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x02;//没收金额
			else RamDataLS.DataNum[LuNum].TypeFlag|=0x01;//正常消费
			RamDataLS.DataNum[LuNum].TypeFlag|=0x04;
			
			MoneyArray(TotalMoney,&RamDataLS.DataNum[LuNum].OldMoney);//放入消费前金额	
			Int_Array(Expenditure,&RamDataLS.DataNum[LuNum].YMoney);//放入本次消费金额
			RamDataLS.DataNum[LuNum].TimeByte[0]=Year;
			RamDataLS.DataNum[LuNum].TimeByte[1]=Month;
			RamDataLS.DataNum[LuNum].TimeByte[2]=Day;	
			RamDataLS.DataNum[LuNum].TimeByte[3]=Hour;	
			RamDataLS.DataNum[LuNum].TimeByte[4]=Minute;	
			RamDataLS.DataNum[LuNum].TimeByte[5]=Second;
			int1=ShanNum*0x200;
			WriteEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);            //保存历史数据
			++LuNum;                //指向本扇区下一条记录
			if(LuNum>29)            //如果扇区满
			{ LuNum=0; ++ShanNum;   //指向下一条扇区
		//王雷雨修改
				if(ShanNum>MaxShan)// ShanOverFlag=1;  //如果存储数量超出,建立超出标志
		//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了		
				{
					ShanNum=0;LuNum=0;SaveFull=1;
				}
				int1=ShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//读一个扇区数据
			}
		//王雷雨修改						
			WriSecNum();//SendData(ShanNum);SendData(LuNum);SendData(SendShanNum);SendData(SendLuNum);
		}
		ClearTemp();
	}
}
//读当前离线数据扇区数
//读当前离线数据扇区数
void ReadSecNum()
{ 
	uint int1;
 //定位历史记录------------------------------------------------
    PReadEeprom(SysShan,sizeof(EpromData),EpromData);//读数据 
//王雷雨修改    
    if((EpromData[0]==0x55)&&(EpromData[6]==0xaa))
    { ShanNum=EpromData[1];     //历史记录所在扇区指针
      LuNum=EpromData[2];       //下一条历史记录所在扇区相对位置
			SendShanNum=EpromData[3];     //历史记录所在扇区指针
      SendLuNum=EpromData[4];
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
			SaveFull=EpromData[5];
//王雷雨修改
      SoundFlag=1;
      if(ShanNum>MaxShan)
      { ShanNum=3;LuNum=0;SaveFull=1;SoundFlag=5;}  //如果超出了则从头开始存SendShanNum=3;SendLuNum=0; 
//			{ShanNum=0;LuNum=0;}//如果历史数据保存超出，则重新从0地址开始保存
      if(LuNum>29) LuNum=0;      
      int1=ShanNum*0x200;
      PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte); //读已经保存的历史记录扇区
//      PReadEeprom(ShuiSector,sizeof(EpromData),EpromData);//读数据 
    }
		else 
    {  
			PReadEeprom(SysShan1,sizeof(EpromData),EpromData);//读备份扇区数据数据 
			if((EpromData[0]==0x55)&&(EpromData[6]==0xaa))
			{
				ShanNum=EpromData[1];     //历史记录所在扇区指针
				LuNum=EpromData[2];       //下一条历史记录所在扇区相对位置
				SendShanNum=EpromData[3];SendLuNum=EpromData[4];
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
			SaveFull=EpromData[5];
				WriteEeprom(SysShan,7,EpromData);//写入扇区指针    
				int1=SendShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),&RamDataLS.DataByte);//读一个扇区数据     
			}
			else
			{
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
			ShanNum=3;LuNum=0;SendShanNum=3;SendLuNum=0;SaveFull=0;
				 WriSecNum(); //第一条历史记录的扇区位置和记录位置
			}
    }
}
//设置离线数据扇区初始值
void  WriSecNum()
{ EpromData[0]=0x55;
  EpromData[1]=ShanNum;
  EpromData[2]=LuNum;
  EpromData[3]=SendShanNum;
	EpromData[4]=SendLuNum;
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
	EpromData[5]=SaveFull;
	EpromData[6]=0xaa;
  WriteEeprom(SysShan,7,EpromData);//写数据     
  WriteEeprom(SysShan1,7,EpromData);//写数据     
}
//保存历史数据
void JiTimerDelay()
{ //uchar ch1;
	uint int1;
	memcpy(&RamDataLS.DataNum[LuNum],JiluTemp,17);    ////卡号4+时间6+消费类别1+消费前金额3++卡类型1+消费金额2=17
	int1=ShanNum*0x200;
//        WriteEeprom(int1,512,RamDataLS.DataByte);            //保存历史数据
	 WriteEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);            //保存历史数据
	++LuNum;                //指向本扇区下一条记录
	if(LuNum>29)            //如果扇区满
	{ LuNum=0; ++ShanNum;   //指向下一条扇区
//王雷雨修改
		if(ShanNum>MaxShan)//  ShanOverFlag=1;    //如果扇区已经存满从头开始存
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了		
		{
			ShanNum=3;LuNum=0;SaveFull=1;
		}
		int1=ShanNum*0x200;
    PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte); //读已经保存的历史记录扇区
	}        
//王雷雨修改
	WriSecNum();    //保存扇区指针
      
}
void SaveExpenditureData()
{
	uchar ch1=0;
	memset(JiluTemp,0,17);//初始化历史记录数组
	memcpy(JiluTemp,&S2TransBuf[15],14);  //卡号4+时间6+消费类别1+消费前金额3=14B,后边再加(本次消费额2)
	if(S2TransBuf[25]>=3)ch1|=0x40;//此字节bit6标记无卡打水，bit5标记投币打水，bit4标记刷卡打水，bit3标记二号出水口，bit2标记1号出水口，bit1标记没收金额消费，bit0标记正常消费
	else if(S2TransBuf[25]==2)ch1|=0x20;//投币打水
	else ch1|=0x10;//刷卡打水
	if(S2TransBuf[29]==1)ch1|=0x01;//正常消费
	else ch1|=0x02;//没收金额
//	if(S2TransBuf[7]<0x20)ch1|=0x04;//1号出水口
//	else ch1|=0x08;//2号出水口
//	ch1=S2TransBuf[25]&0x0f; ch1*=0x10;                    //将消费类别变换在高半字节
//	ch1|=(S2TransBuf[29]&0x0f);                            //与金额类别组成一个字节
//	if(S2TransBuf[7]<0x20){ch1|=0x04;ch1&=0xf7;}//出水口字节为0或1则认为是1号出水口，否则是二号出水口
//	else {ch1|=0x08;ch1&=0xfb;}//此字节高半字节表示消费类型，低半字节的bit3表示二号出水口，bit2标记为一号出水口，两者互斥，bit1标记为负消费，bit0标记为正消费
	JiluTemp[10]=ch1;                  //消费类别+金额类别进数据结构
	JiluTemp[14]=S2TransBuf[42];       //卡类型字节
	JiluTemp[15]=S2TransBuf[30];       //消费金额高位
	JiluTemp[16]=S2TransBuf[31];       //消费金额低位
	ZhenBakH=S2TransBuf[5];ZhenBakL=S2TransBuf[6];//保存结算包帧序，用来判断是否需要保存离线数据
}
//---------------------------------------------------

void MakeLiProg()     //网络不通时保存的数据翻译为数据包
{ uchar ch1;
  uint int1;
//SoundFlag=3;
  //目前指针是下一条的，需要调整到刚才保存的记录
  LuBakNum=SendLuNum;           //先保存历史记录数，防止发送失败时回滚
  ShanBakNum=SendShanNum;
//王雷雨修改
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了
	if(SaveFull)//如果扇区没存满,发送地址大于保存地址
	{
		if(SendLuNum>29)
		{
			SendLuNum=0;
			if(SendShanNum<MaxShan)//如果发送扇区小于最大扇区
			{
				SendShanNum++;
			}
			else
			{
				SendShanNum=3;SaveFull=0;//超出之后又重新指向历史数据开始的位置
			}
			int1=SendShanNum*0x200;
			PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//读一个扇区数据     
//			SoundFlag=1;
		}
	}
	else//发送地址小于保存地址
	{
		if((SendShanNum*30+SendLuNum)>=(ShanNum*30+LuNum))//如果发送数据地址大于保存数据地址，则发送完毕
		{
			LiTimerUnit=0; 
			SendLishiFlag=0;      //发送历史数据标志
			SoundFlag=3;          //两声提示无数据
			SendShanNum=0;SendLuNum=0;ShanNum=0;LuNum=0;
			WriSecNum();
			return;   //传输结束,将计时单元清零，退出
		}
		else
		{
			if(SendLuNum>29)
			{
				SendLuNum=0;
				if(SendShanNum<ShanNum)//当发送数据扇区号小于保存数据扇区号时
				{
					SendShanNum++;
//				SoundFlag=1;
				}
				int1=SendShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//读一个扇区数据     
			}
		}
	}
//20200718增加离线数据存满之后从头开始存，此时保存的地址就小于发送的地址了	
  for(ch1=0;ch1<55;++ch1) S2TransBuf[ch1]=0;
  S2TransBuf[0]=0x7e;      //头长
  S2TransBuf[2]=0x30;        //长度
  S2TransBuf[3]=0x20;          //版本
  S2TransBuf[4]=SheClass;          //设备类型
  if(S2ReTime==0) PushZhenNum(S2TransBuf);     //首次主发将帧序放在数组中
  else { S2TransBuf[5]=ZhenBakH; S2TransBuf[6]=ZhenBakL; }  //重发放上一次帧序
  ZhenBakH=S2TransBuf[5];ZhenBakL=S2TransBuf[6];   //保存帧序备下一次重发用
  S2TransBuf[9]=0x11; S2TransBuf[10]=4;    //1104历史数据包
  memcpy(&S2TransBuf[11],Cdatas,4);  //取设备号
  //数据-----------------------------
  memcpy(&S2TransBuf[15],RamDataLS.DataNum[SendLuNum].CardCode,14);    //卡号4,时间6,类别1,消费前金额3
//  //新旧卡处理----------------------------------
//  if(S2TransBuf[15]>0x9f)
//  { if((S2TransBuf[15]&0xf0)==0xa0) S2TransBuf[42]=0;  //如果是A卡
//    else S2TransBuf[42]=((S2TransBuf[15]/0x10)-9);     //B卡: CardTemp[0]=0xB0,S2RecTemp[42]=2,3=C卡,4=D卡,5=E卡,6=F卡;
//    S2TransBuf[15]&=0x0f;                 //屏蔽新卡高4位 
//  }
//  else S2TransBuf[42]=1;     //老卡Cardtype
	S2TransBuf[42]=RamDataLS.DataNum[SendLuNum].Cardtype;//放入卡类型
  //----------------------------------
  ch1=RamDataLS.DataNum[SendLuNum].TypeFlag;               //类型
	if(ch1&0x01)S2TransBuf[29]=0x01;//表示为正消费
	if(ch1&0x02)S2TransBuf[29]=0x02;//表示误扣费
	S2TransBuf[7]=0x10;//出水口发送一号出水口
//	if(ch1&0x04)S2TransBuf[7]=0x10;//一号出水口的打水记录
//	if(ch1&0x08)S2TransBuf[7]=0x20;//表示二号出水口的打水记录
//  S2TransBuf[29]=ch1&0x0f;         //金额类别1=+，2=扣款
	ch1=ch1/0x10;
	if(ch1==1)S2TransBuf[25]=1;//刷卡打水
	else if(ch1==2)S2TransBuf[25]=2;//投币打水
	else if(ch1==4)S2TransBuf[25]=3;//扫码打水
	else S2TransBuf[25]=0;//消费方式异常
//	if((S2TransBuf[25]==0)||(S2TransBuf[19]>0x30)||(S2TransBuf[19]<19))//如果消费方式异常或时间年异常，则跳过此笔离线数据,
//	{
//			S2ingTimer=0;S2ReTime=0;  //为未收到回复重发服务
//			SendLiWait=0;     //清除等待标志，发送下一条记录
//			SendLuNum++;//发送成功后发送数据地址自增
//			WriSecNum();//保存扇区指针
//		return;
//	}
//  S2TransBuf[25]=ch1/0x10;         //消费类别1=刷卡2=投币3=远程打水
  S2TransBuf[26]=RamDataLS.DataNum[SendLuNum].OldMoney[0];  //消费前金额高位
  S2TransBuf[27]=RamDataLS.DataNum[SendLuNum].OldMoney[1];  //中位
  S2TransBuf[28]=RamDataLS.DataNum[SendLuNum].OldMoney[2];  //低位
  S2TransBuf[30]=RamDataLS.DataNum[SendLuNum].YMoney[0];  //消费金额高位
  S2TransBuf[31]=RamDataLS.DataNum[SendLuNum].YMoney[1];  //低位
  //updateflag---------------------------
  S2TransBuf[43]=0xff;       //设备号,卡号的updateflag  
  S2TransBuf[44]=0xff;       //时间,金额类型的updateflag
  S2TransBuf[45]=0x1f;       //消费前,消费,消费后三个金额的updateflag
  S2TransBuf[46]|=0x80;      //新旧卡标记Flag
  SendLiWait=1;     //发送后，等待回复标志
  S2ingTimer=20;     //如果20秒钟内没收到回复，重新发送
  ++S2ReTime;        //同一个记录连续发送次数
	SendDataPackage(S2TransBuf);
//	SendFixedLength(S2TransBuf,S2TransBuf[2]+1);//SoundFlag=3;
}//发送历史数据
void SendLishiPrg()
{ 
	uint int1;
	
	//发送历史数据-----------------------------
    if(SendLishiFlag)
    { 
//王雷雨修改发送离线数据顺序
			if(ReadSendflag==0)//如果是从非发送历史数据状态到发送历史数据状态，需要读取发送地址所保存的历史数据
			{
				ReadSendflag=1;
				int1=SendShanNum*0x200;
        PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//读一个扇区数据
			}
//王雷雨修改发送离线数据顺序
			while(LiTimerUnit)
      { 
				WDT_CONTR=0x35;           //喂狗1.25秒
				if(S2ProceFlag){S2ProceFlag=0;UartDataTranslate();}//串口接收数据处理
        if(SecFlag)
        { 
					SecFlag=0;
          if(SendLiWait==0) MakeLiProg(); //如果不是等待回复状态，整理历史数据
          //定时退出逻辑----------------------------
          if(S2ingTimer)          //如果正在等待回复
          { --S2ingTimer;         //等待回复计时器
            if(S2ingTimer==0)     //如果等待时间到，还没回复
            { if(S2ReTime>2)      //如果已经连续发送3次,退出发送
              { S2ReTime=0;SendLiWait=0;
                SendLishiFlag=0;
                SendLuNum=LuBakNum;           //发送失败，回滚回原来的地址
                SendShanNum=ShanBakNum;
                LiTimerUnit=0;
                SendLishiFlag=0; //KeyFlag=0; 
                break;
              }
              SendLiWait=0;
            }
          }
        } 
      }
      SendLishiFlag=0;             //清除发送记录标志和计时器
    }  
//王雷雨修改发送离线数据顺序
		else
		{
			if(ReadSendflag)//如果是从发送历史数据状态到非发送历史数据状态，需要读取写地址所保存的历史数据
			{
				ReadSendflag=0;
				int1=ShanNum*0x200;
        PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//读一个扇区数据
			}
		}
//王雷雨修改发送离线数据顺序
}
//发送定长数据，第三个字节表示长度
void SendFixedLength(uchar Str[])
{
	uchar ch1;
	for(ch1=0;ch1<Str[2]+1;ch1++)
	{
		Uart2SendData(Str[ch1]);
	}
}
void SendDataPackage(uchar Str[])
{
	PushCRC(Str);//放入CRC
	SendFixedLength(Str);
}