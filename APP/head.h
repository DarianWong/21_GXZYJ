/*菜单说明
名称
1A1	一号出水口预扣		刷卡打水时
1A2	二号出水口预扣		刷卡打水时
2A1	一号出水口流量计1升水对应脉冲数		流量型机器使用
2A2	二号出水口流量计1升水对应脉冲数		流量型机器使用
2AA	一号出水口投币器脉冲对应金额数		投币打水脉冲检测处使用
2AB	二号出水口投币器脉冲对应金额数		投币打水脉冲检测处使用
3AA	一号出水口刷卡单价
3AB	一号出水口扫码单价
3AC	一号出水口投币单价
4AA	二号出水口刷卡单价
4AB	二号出水口扫码单价
4AC	二号出水口投币单价
5A1	一号出水口单次最大消费金额		投币金额数，后台余额打水时使用，流量贴卡型
5A2	二号出水口单次最大消费金额		投币金额数，后台余额打水时使用，流量贴卡型
6A1	一号出水口
6A2	二号出水口
7A	金额回收时间				刷卡后或投币未按开始时；打水过程中暂停时；收到禁止打水指令时
8A	灯锁延时						刷卡，扫码，投币打水结束时；金额没收后；//必须两个出水口都在未售水时
1B	制水延时						水箱水满后高液位断开出水多长时间后开始制水，即P6持续时间
2B	冲洗时间						上电不缺水且未水满时；制水到水满时；间隔时间到时；
4B	臭氧时间						偶数整点时开启
*/
/*语音说明
0欢迎使用人人健康水站，请放好容器，启动灌装；
1灌装暂停，请在一分钟内重启灌装；
2投币达到最大值，请启动灌装；
3谢谢使用人人健康水站，欢迎下次光临；
4充值完毕，注意取卡；
5充值失败，请重新操作；
6您的卡不能在此水站打水，请重新刷卡退款；
7此卡为无效卡；
8支付成功，请放好容器，启动灌装；
9正在制水，请等待设备可售时再使用
10为是您的水卡安全，请刷卡退款，等待15秒后刷卡取水。
11余额异常，请联系水站工作人员；
12此卡已挂失，请联系水站工作人员；
13此卡已失效，请联系水站工作人员；
14此卡已达当日消费限额。
15为了您的水卡安全，请间隔15秒后重新刷卡取水
16余额不足，请及时充值。
17此设备已达当日消费限额
18：重置完毕，注意取卡。
19：重置失败，请重新操作。
20：清零完毕，注意取卡。
21：清零失败，请重新操作。
22：此设备已达当日消费限额。
23：此设备服务已暂停。
24：此设备暂未注册。
*/
#include "STC8.H"
#include <absacc.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <intrins.h>

	
typedef unsigned char uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;


#define FOSC 22118400L          //系统频率
#define BAUD 115200             //串口波特率

uchar code Cdatas[4] _at_ 0x7ff5;   //32K芯片
#define Version	501//5是S  01是版本
#define MaxShan 47    //每个扇区30条，47扇区可放1440条

#define SysIpPort 48*0x200     //系统内容:IP和端口
#define SysShan   49*0x200     //系统内容:当前扇区指针和扇区内记录指针
#define SysShan1   50*0x200     //系统内容:当前扇区指针和扇区内记录指针
#define ShuiSector 51*0x200  //存储未发送的制水量
#define	SetDataAdd	52*0x200	//系统设置数据
#define	LishiTimer	30*60
#define NetBreakTime 10      //未收到回复认为断网的时间

#define	SheClass	0x51

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

//输入IO口定义
sbit GZKey = P4^3;//罐装1键
sbit ZTKey = P0^3;//暂停1键
sbit HighLevel = P0^4;//高液位
sbit MediumLevel = P0^5;//中液位
sbit LowLevel = P0^6;//低液位
sbit LowTension = P0^7;//低压开关
sbit SZKey = P6^5;//设置键
sbit ZJKey = P6^7;//增加键
sbit JSKey = P6^6;//减少键
//修改
sbit Coin = P6^0;//投币检测


//输出IO口定义
sbit GZPump = P3^7;//罐装泵
sbit Heat1 = P4^1;//浴霸1
sbit Heat2 = P7^0;//浴霸2
sbit LED_BL = P7^1;//广告灯
sbit Ozone = P7^2;//臭氧杀菌
sbit ZSPump = P7^3;//制水泵
sbit CXValve = P2^0;//冲洗阀
//sbit FloodLight = P7^4;//照明灯
//sbit DoorLock = P7^5;//电磁锁
sbit LightLock = P4^2;//照明灯
sbit Buzzer = P2^3;//蜂鸣器
sbit TBControl = P6^1;//投币控制

sbit M800Power = P7^7;//模块电源控制

//-----------黄卡宁语音V02板
sbit YuyinData=P5^1;//50
//时钟，8563
sbit	SDA_clock=P5^3;//05
sbit	SCL_clock=P5^2;//06
//=================================1640显示
sbit  CLK=P7^5;
sbit  DIN=P7^6;
//=================================1640显示

#define WRITE_DATA_MODE_Z   0x40        // 地址自动加
#define WRITE_DATA_MODE_G   0x44        // 固定地址
#define START_DATA          0xC0        // 
#define DISPLAY_EN          0x8A        // 开显示
#define DISPLAY_DIS         0x80        // 关显示
uchar xdata DisBuf[16]={0};           // 显示缓存
//
//sbit  sda=P4^5; //        //显示1640
//sbit  scl=P7^7; //        //显示1640

#define HeartInterval	30		//30秒发送一次心跳包间隔
#define	StateInterval	15		//15分钟发送一次状态包间隔
//#define	UpdataTime	
#define	ParameterInterval	60	//一小时同步一次参数同步指令间隔

#define	PacketDropout	5//丢包次数，达到则认为掉线
#define	DataInterval	2//数据包间隔为500毫秒，计时器放在100ms里面

#define Disp_A  0xa
#define Disp_B  0xb
#define Disp_C  0xc
#define Disp_D  0xd
#define Disp_E  0xe
#define Disp_F  0xf
#define Disp_SP 0x10     //空格
#define Disp_N  0x11
#define Disp_P  0x12
#define Disp_H  0x13
#define Disp_Z  0x14      //中划线
#define Disp_X  0x15      //下划线
#define Disp_W  0x16      //问好         
#define Disp_r  0x17      //r
#define Disp_J  0x18      //J
#define Disp_L  0x19      //L
#define Disp_Deng 0x1a    //=
#define Disp_u  0x1b      //u
#define Disp_n  0x1c      //n
#define Disp_T  0x1d      //T
#define Disp_o  0x1e      //小o
#define Disp_y  0x1f      //y
uchar code DispData[32]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,   //0--9
                         0x77,0x7c,0x39,0x5e,0x79,0x71,                       //A--F
                         0x00,0x37,0x73,0x76,0x40,0x08,0xa3,0x50,0x0e,0x38,  //SP N P H - _ ? r J L
                         0x48,0x1c,0x54,0x31,0x5c,0x6e};   //= u n o

//Eprom----------------------------
//sfr IAP_DATA    =   0xC2;           //IAP数据寄存器
//sfr IAP_ADDRH   =   0xC3;           //IAP地址寄存器高字节
//sfr IAP_ADDRL   =   0xC4;           //IAP地址寄存器低字节
//sfr IAP_CMD     =   0xC5;           //IAP命令寄存器
//sfr IAP_TRIG    =   0xC6;           //IAP命令触发寄存器
//sfr IAP_CONTR   =   0xC7;           //IAP控制寄存器
//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
//#define ENABLE_IAP 0x83           //if SYSCLK<12MHz
#define CMD_IDLE    0               //空闲模式
#define CMD_READ    1               //IAP字节读命令
#define CMD_PROGRAM 2               //IAP字节编程命令
#define CMD_ERASE   3               //IAP扇区擦除命令
//---------------ADC----------------
#define ADC_POWER   0x80            //ADC电源控制位
#define ADC_FLAG    0x20            //ADC完成标志
#define ADC_START   0x40            //ADC起始控制位
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟


//---------------ADC----------------
//---------------硬件SPI----------------
sbit     MF522_RST  =    P6^2;// /    //RC500片选 
sbit     MF522_NSS  =    P1^6;//
sbit sck        =   P1^5;     //SCK
sbit si         =   P1 ^ 3;     //MOSI
sbit so         =   P1 ^ 4;	    //MISO
#define SPIF        0x80        //SPSTAT.7                                
#define WCOL        0x40        //SPSTAT.6 
#define SSIG        0x80        //SPCTL.7                                 
#define SPEN        0x40        //SPCTL.6                                 
#define DORD        0x20        //SPCTL.5                                 
#define MSTR        0x10        //SPCTL.4                                 
#define CPOL        0x08        //SPCTL.3                                 
#define CPHA        0x04        //SPCTL.2                                 
#define SPDHH       0x00        //CPU_CLK/4                               
#define SPDH        0x01        //CPU_CLK/16                              
#define SPDL        0x02        //CPU_CLK/64                              
#define SPDLL       0x03        //CPU_CLK/128      
//---------------硬件SPI----------------
void AutoRead();//读卡处理
void CoinCheck();//投币检测
void UartDataTranslate();//通讯数据解析,联网状态下才执行此处
void ShowMoneyWay();
void ShowEquipment();
void ReadEquipment();
void CloseOutput();
void OpenOutput();
void Show();
void SendData(uchar dat);

void delay();    //略微延时  6us约>4.7us
void Delay1ms(uchar temp);		//@22.1184MHz
uchar dec_bcd(uchar bat);//十进制转BCD
uchar bcd_dec(uchar bat);//BCD转十进制
void AreaIntToChar();

void  DispSetData(uint int1);
ulong	ReadMoney(uchar *pData);
void SaveMoney(ulong Money,uchar *pData);
uchar ReadCard4_6(uchar *CardBuff);
uchar WriteCard4_6(uchar *CardBuff);
void MoneyArray(ulong money,uchar *pData);
void SecDispose();//秒处理函数
void MakeWater();//制水逻辑，每秒处理一次
void AdvertisingLamp();//广告灯判断//根据需求可以适当优化代码量,放置在秒执行函数里
void HeatingControl();//加热控制，放置在秒执行函数里
void OzoneControl();

void SPI_Init(void);
uchar SPI_ReadWriteByte(uchar Byte);

void delay1(unsigned char dlength);

bit CompCRC(uchar Str[]);
void PushCRC(uchar Str[]);//在数组中放入CRC-----------------------
void SendFixedLength(uchar Str[]);//发送定长数据，第三个字节表示长度
void ReadSecNum();//上电读取历史数据指针
uchar strsearch(uchar *ptr2,uchar *ptr1_at);//查字符串*ptr2在*ptr1中的位置
void PushZhenNum(uchar Str[]);
uint CRC_Calculate(uchar Length,uchar *Address);
void  WriSecNum();
void SendDataPackage(uchar Str[]);
void Int_Array(uint int1,uchar *pArray);
void JiTimerDelay();
void SaveExpenditureData();