#include <intrins.h>

/*�����õ��ĳ�ʼ�����������������PCF8563��������TM1640��������RC522�������򣬵�Ƭ��IO�ڼ����Ӳ����ʼ����*/
uchar xdata Second,Minute=0,Hour=0,Day=0,Month=0,Year=0;

void SysInit()
{
	P0M0 = 0x00;		
	P0M1 = 0xf8;//P03,04,05,06,07���Ǹ�������
	P1M0 = 0x78;//RC522SPIͨѶ����Ҫ��Ϊ��©���
	P1M1 = 0xf8;//P10��11��12׼˫��P13��14��15��16��©�����P17AD��������������
//	P1M0 = 0x00;//RC522SPIͨѶ����Ҫ��Ϊ��©���
//	P1M1 = 0x00;//P10��11��12׼˫��P13��14��15��16��©�����P17AD��������������
	P2M0 = 0x09;//
	P2M1 = 0x00;//P20��23���������������׼˫��
	P3M0 = 0x80;
	P3M1 = 0x00;//P37���������������׼˫��
	P4M0 = 0x06;
	P4M1 = 0x08;//P41,42���������P43��������
	P5M0 = 0x03;	
	P5M1 = 0x00;//P50�������
	P6M0 = 0x06;
	P6M1 = 0xe5;//p60,65,66,67�Ǹ������룬62�ǿ�©�����61���������
//	P6M0 = 0x00;
//	P6M1 = 0x00;//p60,65,66,67�Ǹ������룬62�ǿ�©�����61���������
	P7M0 = 0x8f;
	P7M1 = 0x00;	//P70,71,72,73,77���������������׼˫��
	M800Power=1;//�ϵ�ֱ�Ӹ�ͨѶģ�鹩��
//��ʱ��0��ʼ����ϵͳƵ��22.1184Mhz,1ms�ж�һ��
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x9A;		//���ö�ʱ��ֵ
	TH0 = 0xA9;		//���ö�ʱ��ֵ
//	TL0 = 0xCD;		//���ö�ʱ��ֵ		11.0592Mhz,1ms�ж�һ��
//	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0=1;//������ʱ���ж�
////����1�ʹ���2��ʼ��	
	SCON = 0x50;		//8λ����,�ɱ䲨����
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
	T2H = (65536 - (FOSC/4/BAUD))>>8;
	AUXR |= 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
	AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
	ES = 1;                     //ʹ�ܴ���1�ж�
	IE2 = 0x01;                 //ʹ�ܴ���2�ж�

//	INT0 = 1;
	IT0 = 1;                    //����INT0���ж����� (1:���½��� 0:�����غ��½���)
	EX0 = 1;                    //ʹ��INT0�ж�
//	INT1 = 1;
	IT1 = 1;                    //����INT1���ж����� (1:���½��� 0:�����غ��½���)
	EX1 = 1;                    //ʹ��INT1�ж�
//	INT_CLKO |= 0x10;               //(EX2 = 1)ʹ��INT2�ж�	
//	INT_CLKO |= 0x20;               //(EX3 = 1)ʹ��INT3�ж�

	ADCCFG = 0x0f;                              //����ADCʱ��Ϊϵͳʱ��/2/16/16
//	ADC_CONTR = 0x80;                           //ʹ��ADCģ��
	Delay1ms(2);                       //ADC�ϵ粢��ʱ
//	SPI_Init();
	EA=1;
	ReadSecNum();//CloseOutput();
}
//�жϷ������
void exint0() interrupt 0       //INT0�ж��������TDS���
{
	if(ZSFlag)
	{
		TDS1++;
		if(TDS1==1)
		{
			TDStime1=0;
			TDS1flag=1;//��ʼ��ʱ
		} 
	}
  else {TDS1=0;TDStime1=0;TDS1flag=0;}
}
//�жϷ������
void exint1() interrupt 2       //INT1�ж��������TDS���
{
	if(ZSFlag)
	{
		TDS2++;
		if(TDS2==1)
		{
			TDStime2=0;
			TDS2flag=1;//��ʼ��ʱ
		}
	}
	else {TDS2=0;TDStime2=0;TDS2flag=0;}
   
}
/*----------------------------
��ȡADC���
----------------------------*/
uchar GetADCResult(uchar ch)
{
    ADC_CONTR = ADC_POWER | ch | ADC_START;
    _nop_();                        //�ȴ�4��NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//�ȴ�ADCת�����
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC
	
    return ADC_RES;                 //����ADC���
}
void Delay1ms(uchar temp)		//@22.1184MHz
{
	unsigned char i, j;
	while(temp)
	{
		i = 29;
		j = 183;
		do
		{
			while (--j);
		} while (--i);
		temp--;
	}
}
void Delay_k(uint k)
{
	while(k--);
}
//--------------1640��ʾ
/********************Start����*************************/
void I2CStart()
{
    DIN=1;
    CLK=1;
	_nop_();
	_nop_();
	DIN=1; 
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CLK=0;
	_nop_();
	_nop_();
}
/********************Stop����*************************/
void I2CStop()
{
	CLK=1;
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
	DIN=1;
	_nop_();
	_nop_();
	CLK=0;
	DIN=0;

}

/***************����8bit���ݣ��ӵ�λ��ʼ**************/
void I2CWritebyte(unsigned char oneByte)
{
  unsigned char i;
  for(i=0;i<8;i++)
  {
    CLK=0;
	if(oneByte&0x01) 
	  DIN=1;
	else 
	  DIN=0;
	_nop_();
	_nop_();
	_nop_();
	CLK=1;
	oneByte=oneByte>>1;
	}
	CLK=0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	DIN=0;
	_nop_();
	_nop_();
}

/************��ʾ�����������1~16λ��ʾ0~F************/
void Update_Display()
{
 unsigned char i;

 I2CStart();
 I2CWritebyte(0x40);	//�����������ã���ͨģʽ����ַ�Զ���һ
 I2CStop();

 I2CStart();
 I2CWritebyte(0xc0);	//��ַ�������ã���ʼ��ַ00H
 for(i=0;i<16;i++)	//����16λ��ʾ����
 {
   I2CWritebyte(DisBuf[i]);
 }
 I2CStop();

 I2CStart();
 I2CWritebyte(0x8c);	//��ʾ���ƣ���ʾ������������Ϊ11/16
 I2CStop();
}
//// ��ʼ
//void start()
//{
//    SCLK=1;DIO=1;	_nop_();
//    DIO=0;				_nop_();
//    SCLK=0;  		  _nop_();
//}

//void send_data(uchar x)          //������    ��λ�ȴ�
//{
//    unsigned char  i;
//    for(i=0;i<8;i++)
//    {
//        SCLK=0;
//        _nop_();
//        _nop_();
//        DIO=(bit)(x&0x01);
//        _nop_();
//        _nop_();
//        SCLK=1;
//        x>>=1;
//    }
//    SCLK=0;
//}

//// ����
//void stop()
//{
//    SCLK=1;DIO=0;	_nop_();
//    DIO=1;				_nop_();
//    SCLK=0;				_nop_();
//}

//void Init_Display()
//{
//    start();
//    send_data(DISPLAY_DIS);         // ����ʾ
//    stop();

//    start();
//    send_data(WRITE_DATA_MODE_Z);   // �Զ���ַ
//    stop();

//    start();
//    send_data(0x8c);                // ����ʾ//������������������8fΪ����send_data(0x88);
////		send_data(0x8f);
//    stop();
//}

//void Update_Display(void)
//{
//    unsigned char i;
//        start();
//        send_data(START_DATA);              //��ʼ��ַ
//        for(i=0;i<16;i++)                   //��16λ��
//        {
//            send_data(DisBuf[i]);
//        }
//        stop();//SoundFlag=1;
//}
//--------------1640��ʾ
//15ϵ��EPROM=============================================
//�ر�IAP
//----------------------------
void IapIdle()
{
    IAP_CONTR = 0;                  //�ر�IAP����
    IAP_CMD = 0;                    //�������Ĵ���
    IAP_TRIG = 0;                   //��������Ĵ���
    IAP_ADDRH = 0x80;               //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}
//----------------------------
//��ISP/IAP/EEPROM�����ȡһ�ֽ�
//----------------------------
uchar IapReadByte(uint addr)
{ uchar dat;                       //���ݻ�����

    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_READ;             //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    dat = IAP_DATA;                 //��ISP/IAP/EEPROM����
    IapIdle();                      //�ر�IAP����

    return dat;                     //����
}

//----------------------------
//дһ�ֽ����ݵ�ISP/IAP/EEPROM����
//----------------------------
void IapProgramByte(uint addr, uchar dat)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_PROGRAM;          //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_DATA = dat;                 //дISP/IAP/EEPROM����
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}

//----------------------------
//��������
//----------------------------
void IapEraseSector(uint addr)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_ERASE;            //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}

void PReadEeprom(uint begin_addr, uint counter, uchar array[])
{   uint i = 0;
    
    for(i=0; i<counter; i++)
    {   array[i]=IapReadByte(begin_addr+i);    // ��һ���ֽ�
    }
    IapIdle();
    return;
}
void WriteEeprom(uint begin_addr, uint counter, uchar array[])
{   uint i = 0;
    
    IapEraseSector(begin_addr);
    for(i=0; i<counter; i++)
    {   IapProgramByte(begin_addr+i, array[i]);    // ��һ���ֽ�
    }
    IapIdle();
    return;
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
	uchar i=0;
	SCL_clock=1;
	delay();
	while((SDA_clock==1)&&(i<255))
	i++;
	SCL_clock=0;
	delay();
}

void iic_send_byte(uchar bat)  //��������
{
	uchar i,temp;
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

uchar iic_rev()  //��������
{
  uchar temp,i;
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

void iic_send_add_byte(uchar add,uchar bat) //��ĳ����ַ����ĳ����
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

uchar iic_rec_add_byte(uchar add) //��ĳ����ַ��������
{
   uchar temp;
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

//-----------ʱ��Ԥ�趨ֵ---------uchar Sec,
void time_init(uchar Sec,uchar Min,uchar Hour,uchar Day,uchar Moom,uchar Year)
{
	Sec=dec_bcd(Sec);Min=dec_bcd(Min);Hour=dec_bcd(Hour);Day=dec_bcd(Day);Moom=dec_bcd(Moom);Year=dec_bcd(Year);
	iic_send_add_byte(0x02,Sec);         //50��
	iic_send_add_byte(0x03,Min);         //59����    
	iic_send_add_byte(0x04,Hour);        //19Сʱ             
	iic_send_add_byte(0x05,Day);  		//1��
//	iic_send_add_byte(0x06,Week);  		//����
	iic_send_add_byte(0x07,Moom);      //10��  20���͵�
	iic_send_add_byte(0x08,Year);      //2017��
}
//----------------PCF8563��ʼ��-----------------
void pcf_init()
{
	iic_send_add_byte(0x00,0x00);  //����ʱ��
}
uchar dec_bcd(uchar bat)
{
	uchar ch1;
	ch1=bat/10*0x10+bat%10;
	return ch1;
}
//----------------BCDת10����-----------
uchar bcd_dec(uchar bat)
{
	uchar temp1,temp2,tol;
	temp1=bat&0x0f;
	temp2=(bat&0xf0)>>4;
	tol=temp2*10+temp1;
	return tol; 
}
void Readtime()
{
	Second=bcd_dec(0x7f&iic_rec_add_byte(0x02));    //��ȡ��
	Minute=bcd_dec(0x7f&iic_rec_add_byte(0x03));   //��ȡ���Ӳ�ת��Ϊʮ������ʾ��
	Hour=bcd_dec(0x3f&iic_rec_add_byte(0x04));  //��ȡСʱ��ת��Ϊʮ������ʾ��
	Day=bcd_dec(0x3f&iic_rec_add_byte(0x05));   //��ȡ������ת��Ϊʮ������ʾ��
//	Set_time[5]=bcd_dec(0x07&iic_rec_add_byte(0x06));   //��ȡ���ڲ�ת��Ϊʮ������ʾ��
	Month=bcd_dec(0x1f&iic_rec_add_byte(0x07));  //��ȡ�²�ת��Ϊʮ������ʾ��
	Year=bcd_dec(0xff&iic_rec_add_byte(0x08));    //��ȡ�겢ת��Ϊʮ������ʾ��
}
//-------------PCF8563-----------
//-------------����-----------
void Delay2ms()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 58;
	j = 112;
	do
	{
		while (--j);
	} while (--i);
}
void Delay1200us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 35;
	j = 117;
	do
	{
		while (--j);
	} while (--i);
}
void Delay400us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 12;
	j = 122;
	do
	{
		while (--j);
	} while (--i);
}



void High_Bit()
{
	YuyinData = 1;
	Delay1200us();
	YuyinData = 0;
	Delay400us();	
}
void Low_Bit()
{
	YuyinData = 1;
	Delay400us();
	YuyinData = 0;
	Delay1200us();
}

void Audio_Play(uchar dat)
{	
	uchar i=0;
	EA = 0;
	YuyinData = 1;
	Delay1200us();
	YuyinData = 0;
	Delay2ms();
	Delay1ms(1);
	for(i=0;i<8;i++){
		if(dat & 0x01){
			High_Bit();
		}
		else Low_Bit();
		dat >>= 1;
	}
	YuyinData = 1;
	EA = 1;
}
//-------------����-----------
