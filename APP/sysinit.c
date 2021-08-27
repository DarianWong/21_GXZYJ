#include <intrins.h>

/*所有用到的初始化函数都在这里，包括PCF8563驱动程序，TM1640驱动程序，RC522驱动程序，单片机IO口及相关硬件初始化。*/
uchar xdata Second,Minute=0,Hour=0,Day=0,Month=0,Year=0;

void SysInit()
{
	P0M0 = 0x00;		
	P0M1 = 0xf8;//P03,04,05,06,07都是高阻输入
	P1M0 = 0x78;//RC522SPI通讯线需要设为开漏输出
	P1M1 = 0xf8;//P10，11，12准双向，P13，14，15，16开漏输出，P17AD采样，高阻输入
//	P1M0 = 0x00;//RC522SPI通讯线需要设为开漏输出
//	P1M1 = 0x00;//P10，11，12准双向，P13，14，15，16开漏输出，P17AD采样，高阻输入
	P2M0 = 0x09;//
	P2M1 = 0x00;//P20，23是推挽输出，其余准双向
	P3M0 = 0x80;
	P3M1 = 0x00;//P37是推挽输出，其余准双向
	P4M0 = 0x06;
	P4M1 = 0x08;//P41,42推挽输出，P43高阻输入
	P5M0 = 0x03;	
	P5M1 = 0x00;//P50推挽输出
	P6M0 = 0x06;
	P6M1 = 0xe5;//p60,65,66,67是高阻输入，62是开漏输出，61是推挽输出
//	P6M0 = 0x00;
//	P6M1 = 0x00;//p60,65,66,67是高阻输入，62是开漏输出，61是推挽输出
	P7M0 = 0x8f;
	P7M1 = 0x00;	//P70,71,72,73,77是推挽输出，其他准双向
	M800Power=1;//上电直接给通讯模块供电
//定时器0初始化，系统频率22.1184Mhz,1ms中断一次
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
//	TL0 = 0xCD;		//设置定时初值		11.0592Mhz,1ms中断一次
//	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;//开启定时器中断
////串口1和串口2初始化	
	SCON = 0x50;		//8位数据,可变波特率
	S2CON = 0x50;		//8位数据,可变波特率
	T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
	T2H = (65536 - (FOSC/4/BAUD))>>8;
	AUXR |= 0x14;                //T2为1T模式, 并启动定时器2
	AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
	ES = 1;                     //使能串口1中断
	IE2 = 0x01;                 //使能串口2中断

//	INT0 = 1;
	IT0 = 1;                    //设置INT0的中断类型 (1:仅下降沿 0:上升沿和下降沿)
	EX0 = 1;                    //使能INT0中断
//	INT1 = 1;
	IT1 = 1;                    //设置INT1的中断类型 (1:仅下降沿 0:上升沿和下降沿)
	EX1 = 1;                    //使能INT1中断
//	INT_CLKO |= 0x10;               //(EX2 = 1)使能INT2中断	
//	INT_CLKO |= 0x20;               //(EX3 = 1)使能INT3中断

	ADCCFG = 0x0f;                              //设置ADC时钟为系统时钟/2/16/16
//	ADC_CONTR = 0x80;                           //使能ADC模块
	Delay1ms(2);                       //ADC上电并延时
//	SPI_Init();
	EA=1;
	ReadSecNum();//CloseOutput();
}
//中断服务程序
void exint0() interrupt 0       //INT0中断入口用于TDS检测
{
	if(ZSFlag)
	{
		TDS1++;
		if(TDS1==1)
		{
			TDStime1=0;
			TDS1flag=1;//开始计时
		} 
	}
  else {TDS1=0;TDStime1=0;TDS1flag=0;}
}
//中断服务程序
void exint1() interrupt 2       //INT1中断入口用于TDS检测
{
	if(ZSFlag)
	{
		TDS2++;
		if(TDS2==1)
		{
			TDStime2=0;
			TDS2flag=1;//开始计时
		}
	}
	else {TDS2=0;TDStime2=0;TDS2flag=0;}
   
}
/*----------------------------
读取ADC结果
----------------------------*/
uchar GetADCResult(uchar ch)
{
    ADC_CONTR = ADC_POWER | ch | ADC_START;
    _nop_();                        //等待4个NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//等待ADC转换完成
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC
	
    return ADC_RES;                 //返回ADC结果
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
//--------------1640显示
/********************Start函数*************************/
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
/********************Stop函数*************************/
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

/***************发送8bit数据，从低位开始**************/
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

/************显示函数，数码管1~16位显示0~F************/
void Update_Display()
{
 unsigned char i;

 I2CStart();
 I2CWritebyte(0x40);	//数据命令设置：普通模式，地址自动加一
 I2CStop();

 I2CStart();
 I2CWritebyte(0xc0);	//地址命令设置：初始地址00H
 for(i=0;i<16;i++)	//发送16位显示数据
 {
   I2CWritebyte(DisBuf[i]);
 }
 I2CStop();

 I2CStart();
 I2CWritebyte(0x8c);	//显示控制：显示开，脉冲宽度设为11/16
 I2CStop();
}
//// 开始
//void start()
//{
//    SCLK=1;DIO=1;	_nop_();
//    DIO=0;				_nop_();
//    SCLK=0;  		  _nop_();
//}

//void send_data(uchar x)          //送数据    低位先传
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

//// 结束
//void stop()
//{
//    SCLK=1;DIO=0;	_nop_();
//    DIO=1;				_nop_();
//    SCLK=0;				_nop_();
//}

//void Init_Display()
//{
//    start();
//    send_data(DISPLAY_DIS);         // 关显示
//    stop();

//    start();
//    send_data(WRITE_DATA_MODE_Z);   // 自动地址
//    stop();

//    start();
//    send_data(0x8c);                // 开显示//调节数码管亮度在这里，8f为最亮send_data(0x88);
////		send_data(0x8f);
//    stop();
//}

//void Update_Display(void)
//{
//    unsigned char i;
//        start();
//        send_data(START_DATA);              //起始地址
//        for(i=0;i<16;i++)                   //送16位数
//        {
//            send_data(DisBuf[i]);
//        }
//        stop();//SoundFlag=1;
//}
//--------------1640显示
//15系列EPROM=============================================
//关闭IAP
//----------------------------
void IapIdle()
{
    IAP_CONTR = 0;                  //关闭IAP功能
    IAP_CMD = 0;                    //清除命令寄存器
    IAP_TRIG = 0;                   //清除触发寄存器
    IAP_ADDRH = 0x80;               //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}
//----------------------------
//从ISP/IAP/EEPROM区域读取一字节
//----------------------------
uchar IapReadByte(uint addr)
{ uchar dat;                       //数据缓冲区

    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_READ;             //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    dat = IAP_DATA;                 //读ISP/IAP/EEPROM数据
    IapIdle();                      //关闭IAP功能

    return dat;                     //返回
}

//----------------------------
//写一字节数据到ISP/IAP/EEPROM区域
//----------------------------
void IapProgramByte(uint addr, uchar dat)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_PROGRAM;          //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_DATA = dat;                 //写ISP/IAP/EEPROM数据
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    IapIdle();
}

//----------------------------
//扇区擦除
//----------------------------
void IapEraseSector(uint addr)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_ERASE;            //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    IapIdle();
}

void PReadEeprom(uint begin_addr, uint counter, uchar array[])
{   uint i = 0;
    
    for(i=0; i<counter; i++)
    {   array[i]=IapReadByte(begin_addr+i);    // 读一个字节
    }
    IapIdle();
    return;
}
void WriteEeprom(uint begin_addr, uint counter, uchar array[])
{   uint i = 0;
    
    IapEraseSector(begin_addr);
    for(i=0; i<counter; i++)
    {   IapProgramByte(begin_addr+i, array[i]);    // 读一个字节
    }
    IapIdle();
    return;
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
	uchar i=0;
	SCL_clock=1;
	delay();
	while((SDA_clock==1)&&(i<255))
	i++;
	SCL_clock=0;
	delay();
}

void iic_send_byte(uchar bat)  //发送数据
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

uchar iic_rev()  //接受数据
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

void iic_send_add_byte(uchar add,uchar bat) //向某个地址发送某数据
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

uchar iic_rec_add_byte(uchar add) //从某个地址读出数据
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

//-----------时间预设定值---------uchar Sec,
void time_init(uchar Sec,uchar Min,uchar Hour,uchar Day,uchar Moom,uchar Year)
{
	Sec=dec_bcd(Sec);Min=dec_bcd(Min);Hour=dec_bcd(Hour);Day=dec_bcd(Day);Moom=dec_bcd(Moom);Year=dec_bcd(Year);
	iic_send_add_byte(0x02,Sec);         //50秒
	iic_send_add_byte(0x03,Min);         //59分钟    
	iic_send_add_byte(0x04,Hour);        //19小时             
	iic_send_add_byte(0x05,Day);  		//1号
//	iic_send_add_byte(0x06,Week);  		//星期
	iic_send_add_byte(0x07,Moom);      //10月  20世纪的
	iic_send_add_byte(0x08,Year);      //2017年
}
//----------------PCF8563初始化-----------------
void pcf_init()
{
	iic_send_add_byte(0x00,0x00);  //启动时钟
}
uchar dec_bcd(uchar bat)
{
	uchar ch1;
	ch1=bat/10*0x10+bat%10;
	return ch1;
}
//----------------BCD转10进制-----------
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
	Second=bcd_dec(0x7f&iic_rec_add_byte(0x02));    //读取秒
	Minute=bcd_dec(0x7f&iic_rec_add_byte(0x03));   //读取分钟并转化为十进制显示。
	Hour=bcd_dec(0x3f&iic_rec_add_byte(0x04));  //读取小时并转化为十进制显示。
	Day=bcd_dec(0x3f&iic_rec_add_byte(0x05));   //读取天数并转化为十进制显示。
//	Set_time[5]=bcd_dec(0x07&iic_rec_add_byte(0x06));   //读取星期并转化为十进制显示。
	Month=bcd_dec(0x1f&iic_rec_add_byte(0x07));  //读取月并转化为十进制显示。
	Year=bcd_dec(0xff&iic_rec_add_byte(0x08));    //读取年并转化为十进制显示。
}
//-------------PCF8563-----------
//-------------语音-----------
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
//-------------语音-----------
