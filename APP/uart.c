//���ɹ̶���104���ݰ�
void Make104Data(uchar Str[])
{
//	uchar ch1;
	memset(Str,0,50);//��ʼ�����ͻ�����
//  for(ch1=0;ch1<50;++ch1) Str[ch1]=0;
	Str[0]=0x7e;Str[3]=0x20;  Str[4]=SheClass;        //�汾
	Str[2]=48;Str[7]=0x10;Str[9]=1;Str[10]=4;//����֡ͷ
	memcpy(&Str[11],Cdatas,4);  //ȡ�豸���к�
//	PushZhenNum(Str);//����֡��
	memcpy(&Str[15],CardCode,4);  //ȡ����
	Str[19]=Year;Str[20]=Month;Str[21]=Day;Str[22]=Hour;Str[23]=Minute;Str[24]=Second;//���봥��ʱ��
	Str[43]=0xff;Str[44]=0xff;
	if(ReadCardData)Str[45]=0x03;
	else Str[45]=0x1f;
	PushZhenNum(Str);//0104���ݰ�����֡�����ں����������ݰ�
}
void FrameHead()//�̶�֡ͷ
{
	S2TransBuf[0]=0x7e;S2TransBuf[3]=0x20;S2TransBuf[4]=SheClass;S2TransBuf[7]=0x10;S2TransBuf[9]=1;
	//ͷ									Э��汾						�豸����								��ˮ��
}
void ConsumeDataPackage(uchar Str[])//�����������ݰ�
{
	Make104Data(Str);
	if(CardFlag)Str[25]=0x01;//�����ˢ������
	else if(CoinFlag)Str[25]=0x02;
	else if(AnliangFlag||RemoteFlag)Str[25]=0x03;
	else {}
	if(TotalMoney==0){CardFlag=0;}//�������ǰ���Ϊ0����˵���ǿ����޽���ˮ��ˢ�����ݣ���Ҫ���ˢ����ˮ��־
	MoneyArray(TotalMoney,&Str[26]);//��������ǰ���
	if(Expenditure)//����������ѽ�Ϊ0
	{
		if(ConfiscateFlag){Str[29]=0x02;}//�����û��
		else Str[29]=0x01;//��������
		Int_Array(Expenditure,&Str[30]);//���뱾�����ѽ��
		SaveLiData=1;ZhenBakH=Str[5];ZhenBakL=Str[6];//������ʷ���ݱ�־��λ���յ�0204�ظ���֮�����������̶�ʱ���ڻ����´ν�������ʱ��û���յ��ظ�����ֱ�ӱ���Ϊ�������ݡ�
		
	}
//	if(ReadCardData)Str[29]=0;//����Ƕ�������
//	else//��������
//	{
//		
//	}
	if(CardType)Str[42]=CardType;
//	SendDataPackage(Str);
}
void ClearTemp()//������������ڽ�����������ѹ����еı��������Լ���־λ
{
	CardFlag=0;CoinFlag=0;RemoteFlag=0;AnliangFlag=0;ConfiscateFlag=0;Confiscate=0;YuKouOk=0;
	SellState=0;TotalMoney=0;CardType=0;memset(CardCode,0,4);Expenditure=0;Balance=0;//SoundFlag=5;//�����ˮ״̬������ǰ���������ѽ������ͣ�����Expenditure=0;
	MaxMoneyFlag=0;
}
void SendDataJoint()//��������ƴ��
{
	SendDataInterval=DataInterval;//�������͵����ݶ���Ҫ��2s�ļ��ʱ��
	memset(S2TransBuf,0,110);//��ʼ�����ͻ�����
	if(Order0104)//����Ƿ���0104����ָ��
	{
		Order0104=0;
		if(Expenditure&&SaveLiData&&NetWork){SaveLiData=0;JiTimerDelay();}//SoundFlag=3;������������ѽ��ұ������ݱ�־����λ�򱣴��ϴε����Ѽ�¼,����״̬�²���������������ʱֱ�Ӿʹ�Ϊ����������
		ConsumeDataPackage(S2TransBuf);ZhenNumFlag=1;//�Ѿ�����֡���ˣ����治�����·���֡��
		if(Expenditure)//������������ѽ��
		{
			SaveExpenditureData();//����������ת��Ϊ�������ݸ�ʽ��ţ��ݲ�����,
			if(NetWork)
			{
				SaveLiData=1;SaveLiData1s=5;//���������ݱ�־��λ,�ȴ��ظ�ʱ����Ϊ5�룬��ʱ��δ�յ��ظ�ֱ�ӱ���
			}
			else JiTimerDelay();//����������ת��Ϊ�������ݸ�ʽ����������������
		}
		ClearTemp();
	}//ע�⣺�������ѽ��Ϊ0ʱ�����ͽ������������
	else if(TimingFlag)//Уʱָ��ƴ��0103,�ϵ�����֮��Уʱһ��
	{
//		SoundFlag=1;
		TimingFlag=0;
		S2TransBuf[2]=14;S2TransBuf[10]=3;S2TransBuf[11]=1;//���󱱾�ʱ��
	}
	else if(ParameterFlag)//�豸����ָ��0108,ÿСʱ��һ�Σ�ÿ����ˮ�ڷ���һ��
	{
//		SoundFlag=3;
		ParameterFlag=0;
		S2TransBuf[2]=44;S2TransBuf[10]=8;S2TransBuf[15]=EpromDataGu.DataNum.FlowRate1;S2TransBuf[17]=EpromDataGu.DataNum.OnceCXTime;S2TransBuf[18]=EpromDataGu.DataNum.CXInterval;//��ϴ���ʱ��
		S2TransBuf[19]=EpromDataGu.DataNum.OnceOzoneTime;S2TransBuf[20]=EpromDataGu.DataNum.OzoneInterval;//��������ʱ��
		S2TransBuf[21]=EpromDataGu.DataNum.StartHeat;S2TransBuf[22]=EpromDataGu.DataNum.StopHeat;//�¿�
		S2TransBuf[23]=EpromDataGu.DataNum.StartLEDHour;S2TransBuf[24]=EpromDataGu.DataNum.StartLEDMinute;//���ƿ���ʱ��
		S2TransBuf[26]=EpromDataGu.DataNum.StopLEDHour;S2TransBuf[27]=EpromDataGu.DataNum.StopLEDMinute;//���ƹر�ʱ��
		S2TransBuf[31]=EpromDataGu.DataNum.MaxCoin;
		S2TransBuf[7]=0x10;
		Int_Array(EpromDataGu.DataNum.YuKou,&S2TransBuf[29]);//������������
		Int_Array(EpromDataGu.DataNum.CoinPluse,&S2TransBuf[32]);//Ӳ���������Ӧ�����
		Int_Array(EpromDataGu.DataNum.CardPrice/10,&S2TransBuf[34]);//ˢ������
		Int_Array(EpromDataGu.DataNum.VirtualPrice/10,&S2TransBuf[36]);//ɨ�뵥��
		Int_Array(EpromDataGu.DataNum.CoinPrice/10,&S2TransBuf[38]);//Ͷ�ҵ���
		ParameterTime=ParameterInterval;
	}
	else if(ZSTimeFlag)
	{
		ZSTimeFlag=0;PushZhenNum(S2TransBuf);ZhenNumFlag=1;S2TransBuf[2]=18;S2TransBuf[10]=7;Int_Array(ZSTime1h,&S2TransBuf[15]);//������ˮʱ��
//		FrameH=S2TransBuf[5];FrameL=S2TransBuf[6];
	}
	else if(StateFlag)//״̬��0105��10���ӷ���һ�Σ�ÿ����ˮ�ڷ���һ����״̬�ı���������һ��
	{
//		SoundFlag=5;
		StateFlag=0;
		S2TransBuf[2]=30;S2TransBuf[10]=5;
		S2TransBuf[17]=EpromDataGu.DataNum.OnceCXTime;S2TransBuf[20]=Temperature;//��������ʱ�䣬�����¶�
		if(TDS_array[0]>TDS_array[1])//TDS������
		{
			Int_Array(TDS_array[0],&S2TransBuf[21]);//ԭˮTDS
			Int_Array(TDS_array[1],&S2TransBuf[23]);//��ˮTDS
		}
		else
		{
			Int_Array(TDS_array[1],&S2TransBuf[21]);//ԭˮTDS
			Int_Array(TDS_array[0],&S2TransBuf[23]);//��ˮTDS
		}
		S2TransBuf[25]=WorkStateByte;//����״̬�ֽ�
//		Int_Array(ZSTime1h,&S2TransBuf[26]);//�ۼ���ˮʱ��
		S2TransBuf[28]=CommVal;//�ź�ǿ��
		S2TransBuf[7]=0x10;
		Int_Array(EpromDataGu.DataNum.YuKou,&S2TransBuf[15]);//Ԥ��
		Int_Array(EpromDataGu.DataNum.CardPrice/10,&S2TransBuf[18]);//ˢ������
		StateTime=StateInterval;
	}
	else if(HeartFlag) //���������ȼ����0101	25�뷢��һ��
	{
		S2TransBuf[2]=16;S2TransBuf[10]=1;HeartFlag=0;HeartTime=HeartInterval;//���ȣ���������
	}
	else return;//û������Ҫ���ͣ�������
	HeartCnt++;//����һ�����ݰ�����������һ��
	if(HeartCnt>6){HeartCnt=0;NetProce=0;PowerDown=0;M800Power=0;NetWork=0;}//6�����ݰ�δ�յ��ظ���ϵ磬��������
	FrameHead();
	memcpy(&S2TransBuf[11],Cdatas,4);//�����豸��
	if(ZhenNumFlag==0)PushZhenNum(S2TransBuf);//����֡��
	ZhenNumFlag=0;
	PushCRC(S2TransBuf);//����CRC
	if(S2TransBuf[9]==1&&S2TransBuf[10]==4)//������͵���0104ָ����ұ������ѽ�Ϊ0���򱣴�Ϊ�������ݸ�ʽ
	{
		if(S2TransBuf[30]==0&&S2TransBuf[31]==0)return;//����������ѽ��Ϊ0,�򲻷��͸�������
	}
	SendFixedLength(S2TransBuf);
}
void Uart2() interrupt 8
{
  uchar DATA_uart;
  if (S2CON & S2RI)     //RI
  { S2CON &= ~S2RI;         //���S2RIλ
    DATA_uart=S2BUF;
    if(NetProce<0x50) {S2RecBuf[S2RecCount++]=DATA_uart;if(S2RecCount>=100)S2RecCount=0;}  //�������
    else                 //����ڽ�������״̬
    { if(S2Begin==3)              //4������
      { S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
        --S2DataLen;
				if(S2DataLen==0)
        { S2Begin=0;S2RecCount=0;//S2ProceFlag=1;S2RecFlag=0;
					S2RecNum++;if(S2RecNum>7)S2RecNum=0;//������һ�����ݣ���һ������ڽ��ջ������һ��
          return;
        }
      }
      if(S2Begin==2)               //3���ҳ���
      { ++S2Begin; S2DataLen=DATA_uart-2; 
        S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
      }
      if(S2Begin==1)               //2����0
      { if(DATA_uart==0)
        { ++S2Begin;
          S2RecDataBuf[S2RecNum][S2RecCount++]=DATA_uart;
        }
        else 
        { S2Begin=0;S2RecCount=0;}
      }
      if(S2Begin==0)                //1������7e
      { if(DATA_uart==0x7e) 
        { S2Begin=1;S2RecCount=0;
					S2RecDataBuf[S2RecNum][S2RecCount]=DATA_uart; ++S2RecCount;
//					if(S2ProceFlag){S2RecBuf[S2RecCount++]=DATA_uart;TwoPackage=1;}
//					else {S2RecBuf[S2RecCount]=DATA_uart; ++S2RecCount;}//����ϴ����ݴ�������ˣ�����ռ��������㣬��������
          
        }
      }
      S2RecFlag=1;S2CharSpace=0;      //���ձ�־���ַ��м�ļ������,Ϊ�޴����ַ��˳���׼��
			if(S2RecCount>=49) S2RecCount=0;//������յ���ͨѶ���ݣ���ÿ��������50���ֽ�
    }
    /*��������,���н��ܵ������ݾͱ��浽���һ���ֽڣ���֤ճ��ʱ��һ�����ݿ��Դ�����ȷ*/
//�������޸�
//		 if(S2RecCount>100) S2RecCount=0;
  }
  if (S2CON & S2TI)
  {
    S2CON &= ~S2TI;         //���S2TIλ
    busy2 = 0;               //��æ��־
  } 
}

/*----------------------------
���ʹ�������
----------------------------*/
void Uart2SendData(uchar dat)
{
    while (busy2);               //�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)
    busy2 = 1;
    S2BUF = ACC;                //д���ݵ�UART2���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void Uart2SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        Uart2SendData(*s++);         //���͵�ǰ�ַ�
    }
}
uchar Code_CardType(uchar *pCode)
{
	uchar ch1;
	ch1=*pCode/0x10;
	if(ch1<=9)return 1;//�Ͽ�����1
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
����һ��long�����ݣ�ת��Ϊ����char������
���ڰѽ��ת����ͨѶ���͵ĸ�ʽ
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
uchar SearchID()     //���ҿ�ID��
{ uchar ch1,ch2,Error;

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
//                     ͷ    ��   �汾 ����  ֡���    ��   ��  ��������
uchar code Str102[]={0x7e,0,12+32,0x20,SheClass,0x00,0x00,0x10,0x00,0x01,0x02};
void InitModel()//�����봦������
{ 
	uchar ch1,ch2;
	uint int1;
	ch1=NetProce&0xf0;       //��λ=1Ѱ��,=2����͸��,=3����IP,=4�ɹ�
  switch(ch1)
	{
		case 0:          //�ϵ��͵����
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
				if(AutoReconnection>=6)//6��û�������ɹ������ģ��ϵ�����
				{
					AutoReconnection=0;
//��ʱ����
					PowerDown=0;M800Power=0;NetWork=0;//�ϵ��ʱ��
				}
				Uart2SendString("AT+CIPSHUT\x0d\x0a"); 
				break;
				case 5:
					memset(S2RecBuf,0,110);//��ʼ�����ջ�����
					S2RecCount=0;    //for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;����������  //Ϊ��������׼��  
					Uart2SendString("AT+CCID\x0d\x0a");
					
				break;
				case 7:
					if(SearchID()==0)     //���ҿ�ID����
					{
						RorS_Flag=0; NetProce=0x10;  //���뷢���������
					}
				break;
				default: break;
			}
			++NetProce;
    break;
    case 0x10:                  //Ѱ��
      if(RorS_Flag==0)
      { 
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //����������
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
//				RestData(S2RecBuf);
        S2RecCount=0;      //Ϊ��������׼��
				Uart2SendString("AT+CSQ\x0d\x0a");//����Ѱ������
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
				Uart2SendString("AT+CIPMODE=1\x0d\x0a");//����͸������
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
      { //strcpy(S2TransBuf,"AT+CIPSTART=\"TCP\",\"39.106.74.186\",9999\x0d\x0a"); S2TransLen=40; //����IP
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
        memcpy(&S2TransBuf[ch1],&IportBuf[22],5);    //�˿�
        ch1+=5;S2TransBuf[ch1]=0x0d;
        ++ch1;S2TransBuf[ch1]=0x0a;
        ++ch1;
        S2TransLen=ch1;	
//        for(ch1=0;ch1<100;++ch1) S2RecBuf[ch1]=0;  //����������
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
        S2RecCount=0;      //Ϊ��������׼��
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
					NetProce=0;							//Ӧ���յ�CONNECT
          RorS_Flag=0;  break;
				}
				else if(ch1=strsearch("CONNECT\x0d\x0a",S2RecBuf))
        {	NetProce=0x40;							//Ӧ���յ�CONNECT
          RorS_Flag=0;   
					AutoReconnection=0;//͸�����óɹ�����������������			
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
//        for(ch1=0;ch1<100;++ch1) S2TransBuf[ch1]=0;
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
        memcpy(S2TransBuf,Str102,11);  //��½��ͷ
        memcpy(&S2TransBuf[11],Cdatas,4);  //�豸��
        S2TransBuf[4]=SheClass;               //�豸����	���ˮ����ˮ��ʱ��Ʒ���
        memcpy(&S2TransBuf[15],SimStr,20);    //Sim�ִ�
//�������޸ģ��޸ĵ�½������ʷ��������
//				if((ShanNum*30+LuNum)>(SendShanNum*30+SendLuNum))//�������������������ڷ��͵�������������ʣ��������������ڱ����-���ͳɹ���
//				{
//					int1=(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
//				}
//				else
//				{
//					int1=0;ShanNum=0;LuNum=0;SendShanNum=0;SendLuNum=0;
//				}
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
							if(SaveFull)
							{
								int1=(MaxShan+1)*30+(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
								if(int1>((MaxShan+1)*30))int1=(MaxShan+1)*30;
							}
							else
							{
								if((ShanNum*30+LuNum)>(SendShanNum*30+SendLuNum))//�������������������ڷ��͵�������������ʣ��������������ڱ����-���ͳɹ���
								{
									int1=(ShanNum*30+LuNum)-(SendShanNum*30+SendLuNum);
								}
								else
								{
									int1=0;ShanNum=0;LuNum=0;SendShanNum=0;SendLuNum=0;
								}
							}
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
//        int1=ShanNum;int1*=32;int1+=LuNum;
//�������޸�
        S2TransBuf[35]=EpromDataGu.DataNum.AreaCode/0x100;
        S2TransBuf[36]=EpromDataGu.DataNum.AreaCode%0x100;
        S2TransBuf[37]=int1/0x100;                              
        S2TransBuf[38]=int1%0x100;                              
        S2TransBuf[39]=EpromDataGu.DataNum.StartHeat;
        S2TransBuf[40]=EpromDataGu.DataNum.StopHeat;
        PushZhenNum(S2TransBuf);
        PushCRC(S2TransBuf);              
//        S2TransLen=S2TransBuf[2]+1;	//S2TransCount=0;
        NetOverTime=0;     //���¿�ʼ������ʱ
//        for(ch1=0;ch1<110;++ch1) S2RecBuf[ch1]=0;  //����������
				memset(S2RecBuf,0,110);//��ʼ�����ջ�����
        S2RecCount=0;      //Ϊ��������׼��
//				for(ch1=0;ch1<S2TransLen;++ch1)
//				{
//					Uart2SendData(S2TransBuf[ch1]);
//				}
				SendFixedLength(S2TransBuf);
        RorS_Flag=1;
      }
      else
      { 
//				if(CompCRC(S2RecBuf))return;//���CRC���ԣ��������˳�
				if((S2RecBuf[0]==0x7e)&&(S2RecBuf[9]==2)&&(S2RecBuf[10]==2)&&(CompCRC(S2RecBuf)==0))   //����ǻظ���202
        {	NetProce=0x50;	NetWork=1;							//
          NetOverTime=0;     //���¿�ʼ������ʱ
//					if(YuanipFlag)          //�������IP
//          { YuanipFlag=0;
//            WriteEeprom(SysIpPort,27,IportBuf);  
//          }
					NetProce=0x50;TimingFlag=1;HeartFlag=1;ParameterFlag=1;ZSTimeFlag=1;//�ϵ�������������͵����ݶ��ϴ�һ��
	//					SendData(TimingFlag);SendData(ParameterFlag);
					if(EpromDataGu.DataNum.AreaCode!=(S2RecBuf[15]*0x100+S2RecBuf[16]))
					{
						
						EpromDataGu.DataNum.AreaCode=S2RecBuf[15]*0x100+S2RecBuf[16];//����������˵���������豸�� �����벻ͬ���޸�
						WriteEeprom(SetDataAdd,sizeof(EpromDataGu),EpromDataGu.DataStr); //����
						AreaIntToChar();//����������
					}
          if(YuanipFlag)          //�������IP
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
              if(NewIPTime>3) while(1);    //���3�β��ɹ�,����ת����IP
            }
          }
        }          
      } 
      break;
	}
}

//�������з���CRC-----------------------
void PushCRC(uchar Str[])
{ uchar ch1;
  uint int1;
  ch1=Str[2];    //����
  ch1-=2;        //CRC����
  int1=CRC_Calculate(ch1,&Str[1]);  //����CRC
  ++ch1;        //ָ���һ��CRC
  Str[ch1]=(uchar)(int1/0x100);
  ++ch1;
  Str[ch1]=(uchar)(int1%0x100);
}
//�������з���֡���--------------------------
void PushZhenNum(uchar Str[])
{  Str[5]=(uchar)(ZhenNum/0x100);
   Str[6]=(uchar)(ZhenNum%0x100);
   ++ZhenNum;            //��һ�ε�֡��
}
//=======================================
//����CRC------------------------------------
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
//�Ƚ����ݰ��е�CRC-----------------------
bit CompCRC(uchar Str[])
{ uchar ch1,ch2;
  uint int1;

  if(Str[0]!=0x7e) return 1;  //�����ͷ����
  ch1=Str[2];                 //���͵ĳ���
  ch1-=2;                     //CRC�ĳ���
  int1=CRC_Calculate(ch1,&Str[1]); //����CRC
  ch2=(uchar)(int1/0x100);    //CRC���ֽ�
  ++ch1;                      //ָ��CRC��һ�ֽ�
  if(ch2!=Str[ch1])  return 1; //CRC��һ�ֽ�δͨ��
  ++ch1;
  ch2=(uchar)(int1%0x100);     //CRC���ֽ�
  if(ch2!=Str[ch1])  return 1; //CRC��2�ֽ�δͨ��
  return 0;
}
//-----------------------------------------------------------------
//                  C51���ַ�������������            
// ��������    : strsearch ()
// ��������    : ��ָ���������������ҵ���ͬ������
// ��ڲ���   �� ptr2=Ҫ�ҵ�����, ptr1 ��ǰ����
// ���ڲ��� �� 0-û�� �ҵ�   >1 ���ҵ�
//----------------------------------------------------------------
uchar strsearch(uchar *ptr2,uchar *ptr1_at)//���ַ���*ptr2��*ptr1�е�λ��
//����������������ַ���*ptr2�Ƿ���ȫ������*ptr1��
//����:  0  û���ҵ�
//       1-255 �ӵ�N���ַ���ʼ��ͬ
{ uchar i,j,k;

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

void LostConnection()//���߽������ݴ���
{
	uint int1;
	if(Order0104)//����н�������&&Expenditure
	{
		Order0104=0;
		if(Expenditure)//����������ѽ�Ϊ0���򱣴���������
		{
			RamDataLS.DataNum[LuNum].TypeFlag=0;
			if(CardFlag||AnliangFlag)
			{
				memcpy(&RamDataLS.DataNum[LuNum].CardCode,CardCode,4);RamDataLS.DataNum[LuNum].Cardtype=CardType;
				if(AnliangFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
				else RamDataLS.DataNum[LuNum].TypeFlag|=0x10;
				
			}////���뿨�������ˢ����ˮ
			else if(RemoteFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
			else RamDataLS.DataNum[LuNum].TypeFlag|=0x20;//Ͷ�Ҵ�ˮ
	//		else RamDataLS.DataNum[LuNum].TypeFlag|=0x40;
			if(ConfiscateFlag)RamDataLS.DataNum[LuNum].TypeFlag|=0x02;//û�ս��
			else RamDataLS.DataNum[LuNum].TypeFlag|=0x01;//��������
			RamDataLS.DataNum[LuNum].TypeFlag|=0x04;
			
			MoneyArray(TotalMoney,&RamDataLS.DataNum[LuNum].OldMoney);//��������ǰ���	
			Int_Array(Expenditure,&RamDataLS.DataNum[LuNum].YMoney);//���뱾�����ѽ��
			RamDataLS.DataNum[LuNum].TimeByte[0]=Year;
			RamDataLS.DataNum[LuNum].TimeByte[1]=Month;
			RamDataLS.DataNum[LuNum].TimeByte[2]=Day;	
			RamDataLS.DataNum[LuNum].TimeByte[3]=Hour;	
			RamDataLS.DataNum[LuNum].TimeByte[4]=Minute;	
			RamDataLS.DataNum[LuNum].TimeByte[5]=Second;
			int1=ShanNum*0x200;
			WriteEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);            //������ʷ����
			++LuNum;                //ָ��������һ����¼
			if(LuNum>29)            //���������
			{ LuNum=0; ++ShanNum;   //ָ����һ������
		//�������޸�
				if(ShanNum>MaxShan)// ShanOverFlag=1;  //����洢��������,����������־
		//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��		
				{
					ShanNum=0;LuNum=0;SaveFull=1;
				}
				int1=ShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//��һ����������
			}
		//�������޸�						
			WriSecNum();//SendData(ShanNum);SendData(LuNum);SendData(SendShanNum);SendData(SendLuNum);
		}
		ClearTemp();
	}
}
//����ǰ��������������
//����ǰ��������������
void ReadSecNum()
{ 
	uint int1;
 //��λ��ʷ��¼------------------------------------------------
    PReadEeprom(SysShan,sizeof(EpromData),EpromData);//������ 
//�������޸�    
    if((EpromData[0]==0x55)&&(EpromData[6]==0xaa))
    { ShanNum=EpromData[1];     //��ʷ��¼��������ָ��
      LuNum=EpromData[2];       //��һ����ʷ��¼�����������λ��
			SendShanNum=EpromData[3];     //��ʷ��¼��������ָ��
      SendLuNum=EpromData[4];
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
			SaveFull=EpromData[5];
//�������޸�
      SoundFlag=1;
      if(ShanNum>MaxShan)
      { ShanNum=3;LuNum=0;SaveFull=1;SoundFlag=5;}  //������������ͷ��ʼ��SendShanNum=3;SendLuNum=0; 
//			{ShanNum=0;LuNum=0;}//�����ʷ���ݱ��泬���������´�0��ַ��ʼ����
      if(LuNum>29) LuNum=0;      
      int1=ShanNum*0x200;
      PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte); //���Ѿ��������ʷ��¼����
//      PReadEeprom(ShuiSector,sizeof(EpromData),EpromData);//������ 
    }
		else 
    {  
			PReadEeprom(SysShan1,sizeof(EpromData),EpromData);//������������������ 
			if((EpromData[0]==0x55)&&(EpromData[6]==0xaa))
			{
				ShanNum=EpromData[1];     //��ʷ��¼��������ָ��
				LuNum=EpromData[2];       //��һ����ʷ��¼�����������λ��
				SendShanNum=EpromData[3];SendLuNum=EpromData[4];
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
			SaveFull=EpromData[5];
				WriteEeprom(SysShan,7,EpromData);//д������ָ��    
				int1=SendShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),&RamDataLS.DataByte);//��һ����������     
			}
			else
			{
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
			ShanNum=3;LuNum=0;SendShanNum=3;SendLuNum=0;SaveFull=0;
				 WriSecNum(); //��һ����ʷ��¼������λ�úͼ�¼λ��
			}
    }
}
//������������������ʼֵ
void  WriSecNum()
{ EpromData[0]=0x55;
  EpromData[1]=ShanNum;
  EpromData[2]=LuNum;
  EpromData[3]=SendShanNum;
	EpromData[4]=SendLuNum;
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
	EpromData[5]=SaveFull;
	EpromData[6]=0xaa;
  WriteEeprom(SysShan,7,EpromData);//д����     
  WriteEeprom(SysShan1,7,EpromData);//д����     
}
//������ʷ����
void JiTimerDelay()
{ //uchar ch1;
	uint int1;
	memcpy(&RamDataLS.DataNum[LuNum],JiluTemp,17);    ////����4+ʱ��6+�������1+����ǰ���3++������1+���ѽ��2=17
	int1=ShanNum*0x200;
//        WriteEeprom(int1,512,RamDataLS.DataByte);            //������ʷ����
	 WriteEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);            //������ʷ����
	++LuNum;                //ָ��������һ����¼
	if(LuNum>29)            //���������
	{ LuNum=0; ++ShanNum;   //ָ����һ������
//�������޸�
		if(ShanNum>MaxShan)//  ShanOverFlag=1;    //��������Ѿ�������ͷ��ʼ��
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��		
		{
			ShanNum=3;LuNum=0;SaveFull=1;
		}
		int1=ShanNum*0x200;
    PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte); //���Ѿ��������ʷ��¼����
	}        
//�������޸�
	WriSecNum();    //��������ָ��
      
}
void SaveExpenditureData()
{
	uchar ch1=0;
	memset(JiluTemp,0,17);//��ʼ����ʷ��¼����
	memcpy(JiluTemp,&S2TransBuf[15],14);  //����4+ʱ��6+�������1+����ǰ���3=14B,����ټ�(�������Ѷ�2)
	if(S2TransBuf[25]>=3)ch1|=0x40;//���ֽ�bit6����޿���ˮ��bit5���Ͷ�Ҵ�ˮ��bit4���ˢ����ˮ��bit3��Ƕ��ų�ˮ�ڣ�bit2���1�ų�ˮ�ڣ�bit1���û�ս�����ѣ�bit0�����������
	else if(S2TransBuf[25]==2)ch1|=0x20;//Ͷ�Ҵ�ˮ
	else ch1|=0x10;//ˢ����ˮ
	if(S2TransBuf[29]==1)ch1|=0x01;//��������
	else ch1|=0x02;//û�ս��
//	if(S2TransBuf[7]<0x20)ch1|=0x04;//1�ų�ˮ��
//	else ch1|=0x08;//2�ų�ˮ��
//	ch1=S2TransBuf[25]&0x0f; ch1*=0x10;                    //���������任�ڸ߰��ֽ�
//	ch1|=(S2TransBuf[29]&0x0f);                            //����������һ���ֽ�
//	if(S2TransBuf[7]<0x20){ch1|=0x04;ch1&=0xf7;}//��ˮ���ֽ�Ϊ0��1����Ϊ��1�ų�ˮ�ڣ������Ƕ��ų�ˮ��
//	else {ch1|=0x08;ch1&=0xfb;}//���ֽڸ߰��ֽڱ�ʾ�������ͣ��Ͱ��ֽڵ�bit3��ʾ���ų�ˮ�ڣ�bit2���Ϊһ�ų�ˮ�ڣ����߻��⣬bit1���Ϊ�����ѣ�bit0���Ϊ������
	JiluTemp[10]=ch1;                  //�������+����������ݽṹ
	JiluTemp[14]=S2TransBuf[42];       //�������ֽ�
	JiluTemp[15]=S2TransBuf[30];       //���ѽ���λ
	JiluTemp[16]=S2TransBuf[31];       //���ѽ���λ
	ZhenBakH=S2TransBuf[5];ZhenBakL=S2TransBuf[6];//��������֡�������ж��Ƿ���Ҫ������������
}
//---------------------------------------------------

void MakeLiProg()     //���粻ͨʱ��������ݷ���Ϊ���ݰ�
{ uchar ch1;
  uint int1;
//SoundFlag=3;
  //Ŀǰָ������һ���ģ���Ҫ�������ղű���ļ�¼
  LuBakNum=SendLuNum;           //�ȱ�����ʷ��¼������ֹ����ʧ��ʱ�ع�
  ShanBakNum=SendShanNum;
//�������޸�
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��
	if(SaveFull)//�������û����,���͵�ַ���ڱ����ַ
	{
		if(SendLuNum>29)
		{
			SendLuNum=0;
			if(SendShanNum<MaxShan)//�����������С���������
			{
				SendShanNum++;
			}
			else
			{
				SendShanNum=3;SaveFull=0;//����֮��������ָ����ʷ���ݿ�ʼ��λ��
			}
			int1=SendShanNum*0x200;
			PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//��һ����������     
//			SoundFlag=1;
		}
	}
	else//���͵�ַС�ڱ����ַ
	{
		if((SendShanNum*30+SendLuNum)>=(ShanNum*30+LuNum))//����������ݵ�ַ���ڱ������ݵ�ַ���������
		{
			LiTimerUnit=0; 
			SendLishiFlag=0;      //������ʷ���ݱ�־
			SoundFlag=3;          //������ʾ������
			SendShanNum=0;SendLuNum=0;ShanNum=0;LuNum=0;
			WriSecNum();
			return;   //�������,����ʱ��Ԫ���㣬�˳�
		}
		else
		{
			if(SendLuNum>29)
			{
				SendLuNum=0;
				if(SendShanNum<ShanNum)//����������������С�ڱ�������������ʱ
				{
					SendShanNum++;
//				SoundFlag=1;
				}
				int1=SendShanNum*0x200;
				PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//��һ����������     
			}
		}
	}
//20200718�����������ݴ���֮���ͷ��ʼ�棬��ʱ����ĵ�ַ��С�ڷ��͵ĵ�ַ��	
  for(ch1=0;ch1<55;++ch1) S2TransBuf[ch1]=0;
  S2TransBuf[0]=0x7e;      //ͷ��
  S2TransBuf[2]=0x30;        //����
  S2TransBuf[3]=0x20;          //�汾
  S2TransBuf[4]=SheClass;          //�豸����
  if(S2ReTime==0) PushZhenNum(S2TransBuf);     //�״�������֡�����������
  else { S2TransBuf[5]=ZhenBakH; S2TransBuf[6]=ZhenBakL; }  //�ط�����һ��֡��
  ZhenBakH=S2TransBuf[5];ZhenBakL=S2TransBuf[6];   //����֡����һ���ط���
  S2TransBuf[9]=0x11; S2TransBuf[10]=4;    //1104��ʷ���ݰ�
  memcpy(&S2TransBuf[11],Cdatas,4);  //ȡ�豸��
  //����-----------------------------
  memcpy(&S2TransBuf[15],RamDataLS.DataNum[SendLuNum].CardCode,14);    //����4,ʱ��6,���1,����ǰ���3
//  //�¾ɿ�����----------------------------------
//  if(S2TransBuf[15]>0x9f)
//  { if((S2TransBuf[15]&0xf0)==0xa0) S2TransBuf[42]=0;  //�����A��
//    else S2TransBuf[42]=((S2TransBuf[15]/0x10)-9);     //B��: CardTemp[0]=0xB0,S2RecTemp[42]=2,3=C��,4=D��,5=E��,6=F��;
//    S2TransBuf[15]&=0x0f;                 //�����¿���4λ 
//  }
//  else S2TransBuf[42]=1;     //�Ͽ�Cardtype
	S2TransBuf[42]=RamDataLS.DataNum[SendLuNum].Cardtype;//���뿨����
  //----------------------------------
  ch1=RamDataLS.DataNum[SendLuNum].TypeFlag;               //����
	if(ch1&0x01)S2TransBuf[29]=0x01;//��ʾΪ������
	if(ch1&0x02)S2TransBuf[29]=0x02;//��ʾ��۷�
	S2TransBuf[7]=0x10;//��ˮ�ڷ���һ�ų�ˮ��
//	if(ch1&0x04)S2TransBuf[7]=0x10;//һ�ų�ˮ�ڵĴ�ˮ��¼
//	if(ch1&0x08)S2TransBuf[7]=0x20;//��ʾ���ų�ˮ�ڵĴ�ˮ��¼
//  S2TransBuf[29]=ch1&0x0f;         //������1=+��2=�ۿ�
	ch1=ch1/0x10;
	if(ch1==1)S2TransBuf[25]=1;//ˢ����ˮ
	else if(ch1==2)S2TransBuf[25]=2;//Ͷ�Ҵ�ˮ
	else if(ch1==4)S2TransBuf[25]=3;//ɨ���ˮ
	else S2TransBuf[25]=0;//���ѷ�ʽ�쳣
//	if((S2TransBuf[25]==0)||(S2TransBuf[19]>0x30)||(S2TransBuf[19]<19))//������ѷ�ʽ�쳣��ʱ�����쳣���������˱���������,
//	{
//			S2ingTimer=0;S2ReTime=0;  //Ϊδ�յ��ظ��ط�����
//			SendLiWait=0;     //����ȴ���־��������һ����¼
//			SendLuNum++;//���ͳɹ��������ݵ�ַ����
//			WriSecNum();//��������ָ��
//		return;
//	}
//  S2TransBuf[25]=ch1/0x10;         //�������1=ˢ��2=Ͷ��3=Զ�̴�ˮ
  S2TransBuf[26]=RamDataLS.DataNum[SendLuNum].OldMoney[0];  //����ǰ����λ
  S2TransBuf[27]=RamDataLS.DataNum[SendLuNum].OldMoney[1];  //��λ
  S2TransBuf[28]=RamDataLS.DataNum[SendLuNum].OldMoney[2];  //��λ
  S2TransBuf[30]=RamDataLS.DataNum[SendLuNum].YMoney[0];  //���ѽ���λ
  S2TransBuf[31]=RamDataLS.DataNum[SendLuNum].YMoney[1];  //��λ
  //updateflag---------------------------
  S2TransBuf[43]=0xff;       //�豸��,���ŵ�updateflag  
  S2TransBuf[44]=0xff;       //ʱ��,������͵�updateflag
  S2TransBuf[45]=0x1f;       //����ǰ,����,���Ѻ���������updateflag
  S2TransBuf[46]|=0x80;      //�¾ɿ����Flag
  SendLiWait=1;     //���ͺ󣬵ȴ��ظ���־
  S2ingTimer=20;     //���20������û�յ��ظ������·���
  ++S2ReTime;        //ͬһ����¼�������ʹ���
	SendDataPackage(S2TransBuf);
//	SendFixedLength(S2TransBuf,S2TransBuf[2]+1);//SoundFlag=3;
}//������ʷ����
void SendLishiPrg()
{ 
	uint int1;
	
	//������ʷ����-----------------------------
    if(SendLishiFlag)
    { 
//�������޸ķ�����������˳��
			if(ReadSendflag==0)//����ǴӷǷ�����ʷ����״̬��������ʷ����״̬����Ҫ��ȡ���͵�ַ���������ʷ����
			{
				ReadSendflag=1;
				int1=SendShanNum*0x200;
        PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//��һ����������
			}
//�������޸ķ�����������˳��
			while(LiTimerUnit)
      { 
				WDT_CONTR=0x35;           //ι��1.25��
				if(S2ProceFlag){S2ProceFlag=0;UartDataTranslate();}//���ڽ������ݴ���
        if(SecFlag)
        { 
					SecFlag=0;
          if(SendLiWait==0) MakeLiProg(); //������ǵȴ��ظ�״̬��������ʷ����
          //��ʱ�˳��߼�----------------------------
          if(S2ingTimer)          //������ڵȴ��ظ�
          { --S2ingTimer;         //�ȴ��ظ���ʱ��
            if(S2ingTimer==0)     //����ȴ�ʱ�䵽����û�ظ�
            { if(S2ReTime>2)      //����Ѿ���������3��,�˳�����
              { S2ReTime=0;SendLiWait=0;
                SendLishiFlag=0;
                SendLuNum=LuBakNum;           //����ʧ�ܣ��ع���ԭ���ĵ�ַ
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
      SendLishiFlag=0;             //������ͼ�¼��־�ͼ�ʱ��
    }  
//�������޸ķ�����������˳��
		else
		{
			if(ReadSendflag)//����Ǵӷ�����ʷ����״̬���Ƿ�����ʷ����״̬����Ҫ��ȡд��ַ���������ʷ����
			{
				ReadSendflag=0;
				int1=ShanNum*0x200;
        PReadEeprom(int1,sizeof(RamDataLS),RamDataLS.DataByte);//��һ����������
			}
		}
//�������޸ķ�����������˳��
}
//���Ͷ������ݣ��������ֽڱ�ʾ����
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
	PushCRC(Str);//����CRC
	SendFixedLength(Str);
}