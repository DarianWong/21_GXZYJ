/*�˵�˵��
����
1A1	һ�ų�ˮ��Ԥ��		ˢ����ˮʱ
1A2	���ų�ˮ��Ԥ��		ˢ����ˮʱ
2A1	һ�ų�ˮ��������1��ˮ��Ӧ������		�����ͻ���ʹ��
2A2	���ų�ˮ��������1��ˮ��Ӧ������		�����ͻ���ʹ��
2AA	һ�ų�ˮ��Ͷ���������Ӧ�����		Ͷ�Ҵ�ˮ�����⴦ʹ��
2AB	���ų�ˮ��Ͷ���������Ӧ�����		Ͷ�Ҵ�ˮ�����⴦ʹ��
3AA	һ�ų�ˮ��ˢ������
3AB	һ�ų�ˮ��ɨ�뵥��
3AC	һ�ų�ˮ��Ͷ�ҵ���
4AA	���ų�ˮ��ˢ������
4AB	���ų�ˮ��ɨ�뵥��
4AC	���ų�ˮ��Ͷ�ҵ���
5A1	һ�ų�ˮ�ڵ���������ѽ��		Ͷ�ҽ��������̨����ˮʱʹ�ã�����������
5A2	���ų�ˮ�ڵ���������ѽ��		Ͷ�ҽ��������̨����ˮʱʹ�ã�����������
6A1	һ�ų�ˮ��
6A2	���ų�ˮ��
7A	������ʱ��				ˢ�����Ͷ��δ����ʼʱ����ˮ��������ͣʱ���յ���ֹ��ˮָ��ʱ
8A	������ʱ						ˢ����ɨ�룬Ͷ�Ҵ�ˮ����ʱ�����û�պ�//����������ˮ�ڶ���δ��ˮʱ
1B	��ˮ��ʱ						ˮ��ˮ�����Һλ�Ͽ���ˮ�೤ʱ���ʼ��ˮ����P6����ʱ��
2B	��ϴʱ��						�ϵ粻ȱˮ��δˮ��ʱ����ˮ��ˮ��ʱ�����ʱ�䵽ʱ��
4B	����ʱ��						ż������ʱ����
*/
/*����˵��
0��ӭʹ�����˽���ˮվ����ź�������������װ��
1��װ��ͣ������һ������������װ��
2Ͷ�Ҵﵽ���ֵ����������װ��
3ллʹ�����˽���ˮվ����ӭ�´ι��٣�
4��ֵ��ϣ�ע��ȡ����
5��ֵʧ�ܣ������²�����
6���Ŀ������ڴ�ˮվ��ˮ��������ˢ���˿
7�˿�Ϊ��Ч����
8֧���ɹ�����ź�������������װ��
9������ˮ����ȴ��豸����ʱ��ʹ��
10Ϊ������ˮ����ȫ����ˢ���˿�ȴ�15���ˢ��ȡˮ��
11����쳣������ϵˮվ������Ա��
12�˿��ѹ�ʧ������ϵˮվ������Ա��
13�˿���ʧЧ������ϵˮվ������Ա��
14�˿��Ѵﵱ�������޶
15Ϊ������ˮ����ȫ������15�������ˢ��ȡˮ
16���㣬�뼰ʱ��ֵ��
17���豸�Ѵﵱ�������޶�
18��������ϣ�ע��ȡ����
19������ʧ�ܣ������²�����
20��������ϣ�ע��ȡ����
21������ʧ�ܣ������²�����
22�����豸�Ѵﵱ�������޶
23�����豸��������ͣ��
24�����豸��δע�ᡣ
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


#define FOSC 22118400L          //ϵͳƵ��
#define BAUD 115200             //���ڲ�����

uchar code Cdatas[4] _at_ 0x7ff5;   //32KоƬ
#define Version	501//5��S  01�ǰ汾
#define MaxShan 47    //ÿ������30����47�����ɷ�1440��

#define SysIpPort 48*0x200     //ϵͳ����:IP�Ͷ˿�
#define SysShan   49*0x200     //ϵͳ����:��ǰ����ָ��������ڼ�¼ָ��
#define SysShan1   50*0x200     //ϵͳ����:��ǰ����ָ��������ڼ�¼ָ��
#define ShuiSector 51*0x200  //�洢δ���͵���ˮ��
#define	SetDataAdd	52*0x200	//ϵͳ��������
#define	LishiTimer	30*60
#define NetBreakTime 10      //δ�յ��ظ���Ϊ������ʱ��

#define	SheClass	0x51

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

//����IO�ڶ���
sbit GZKey = P4^3;//��װ1��
sbit ZTKey = P0^3;//��ͣ1��
sbit HighLevel = P0^4;//��Һλ
sbit MediumLevel = P0^5;//��Һλ
sbit LowLevel = P0^6;//��Һλ
sbit LowTension = P0^7;//��ѹ����
sbit SZKey = P6^5;//���ü�
sbit ZJKey = P6^7;//���Ӽ�
sbit JSKey = P6^6;//���ټ�
//�޸�
sbit Coin = P6^0;//Ͷ�Ҽ��


//���IO�ڶ���
sbit GZPump = P3^7;//��װ��
sbit Heat1 = P4^1;//ԡ��1
sbit Heat2 = P7^0;//ԡ��2
sbit LED_BL = P7^1;//����
sbit Ozone = P7^2;//����ɱ��
sbit ZSPump = P7^3;//��ˮ��
sbit CXValve = P2^0;//��ϴ��
//sbit FloodLight = P7^4;//������
//sbit DoorLock = P7^5;//�����
sbit LightLock = P4^2;//������
sbit Buzzer = P2^3;//������
sbit TBControl = P6^1;//Ͷ�ҿ���

sbit M800Power = P7^7;//ģ���Դ����

//-----------�ƿ�������V02��
sbit YuyinData=P5^1;//50
//ʱ�ӣ�8563
sbit	SDA_clock=P5^3;//05
sbit	SCL_clock=P5^2;//06
//=================================1640��ʾ
sbit  CLK=P7^5;
sbit  DIN=P7^6;
//=================================1640��ʾ

#define WRITE_DATA_MODE_Z   0x40        // ��ַ�Զ���
#define WRITE_DATA_MODE_G   0x44        // �̶���ַ
#define START_DATA          0xC0        // 
#define DISPLAY_EN          0x8A        // ����ʾ
#define DISPLAY_DIS         0x80        // ����ʾ
uchar xdata DisBuf[16]={0};           // ��ʾ����
//
//sbit  sda=P4^5; //        //��ʾ1640
//sbit  scl=P7^7; //        //��ʾ1640

#define HeartInterval	30		//30�뷢��һ�����������
#define	StateInterval	15		//15���ӷ���һ��״̬�����
//#define	UpdataTime	
#define	ParameterInterval	60	//һСʱͬ��һ�β���ͬ��ָ����

#define	PacketDropout	5//�����������ﵽ����Ϊ����
#define	DataInterval	2//���ݰ����Ϊ500���룬��ʱ������100ms����

#define Disp_A  0xa
#define Disp_B  0xb
#define Disp_C  0xc
#define Disp_D  0xd
#define Disp_E  0xe
#define Disp_F  0xf
#define Disp_SP 0x10     //�ո�
#define Disp_N  0x11
#define Disp_P  0x12
#define Disp_H  0x13
#define Disp_Z  0x14      //�л���
#define Disp_X  0x15      //�»���
#define Disp_W  0x16      //�ʺ�         
#define Disp_r  0x17      //r
#define Disp_J  0x18      //J
#define Disp_L  0x19      //L
#define Disp_Deng 0x1a    //=
#define Disp_u  0x1b      //u
#define Disp_n  0x1c      //n
#define Disp_T  0x1d      //T
#define Disp_o  0x1e      //Сo
#define Disp_y  0x1f      //y
uchar code DispData[32]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,   //0--9
                         0x77,0x7c,0x39,0x5e,0x79,0x71,                       //A--F
                         0x00,0x37,0x73,0x76,0x40,0x08,0xa3,0x50,0x0e,0x38,  //SP N P H - _ ? r J L
                         0x48,0x1c,0x54,0x31,0x5c,0x6e};   //= u n o

//Eprom----------------------------
//sfr IAP_DATA    =   0xC2;           //IAP���ݼĴ���
//sfr IAP_ADDRH   =   0xC3;           //IAP��ַ�Ĵ������ֽ�
//sfr IAP_ADDRL   =   0xC4;           //IAP��ַ�Ĵ������ֽ�
//sfr IAP_CMD     =   0xC5;           //IAP����Ĵ���
//sfr IAP_TRIG    =   0xC6;           //IAP������Ĵ���
//sfr IAP_CONTR   =   0xC7;           //IAP���ƼĴ���
//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
//#define ENABLE_IAP 0x83           //if SYSCLK<12MHz
#define CMD_IDLE    0               //����ģʽ
#define CMD_READ    1               //IAP�ֽڶ�����
#define CMD_PROGRAM 2               //IAP�ֽڱ������
#define CMD_ERASE   3               //IAP������������
//---------------ADC----------------
#define ADC_POWER   0x80            //ADC��Դ����λ
#define ADC_FLAG    0x20            //ADC��ɱ�־
#define ADC_START   0x40            //ADC��ʼ����λ
#define ADC_SPEEDLL 0x00            //540��ʱ��
#define ADC_SPEEDL  0x20            //360��ʱ��
#define ADC_SPEEDH  0x40            //180��ʱ��
#define ADC_SPEEDHH 0x60            //90��ʱ��


//---------------ADC----------------
//---------------Ӳ��SPI----------------
sbit     MF522_RST  =    P6^2;// /    //RC500Ƭѡ 
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
//---------------Ӳ��SPI----------------
void AutoRead();//��������
void CoinCheck();//Ͷ�Ҽ��
void UartDataTranslate();//ͨѶ���ݽ���,����״̬�²�ִ�д˴�
void ShowMoneyWay();
void ShowEquipment();
void ReadEquipment();
void CloseOutput();
void OpenOutput();
void Show();
void SendData(uchar dat);

void delay();    //��΢��ʱ  6usԼ>4.7us
void Delay1ms(uchar temp);		//@22.1184MHz
uchar dec_bcd(uchar bat);//ʮ����תBCD
uchar bcd_dec(uchar bat);//BCDתʮ����
void AreaIntToChar();

void  DispSetData(uint int1);
ulong	ReadMoney(uchar *pData);
void SaveMoney(ulong Money,uchar *pData);
uchar ReadCard4_6(uchar *CardBuff);
uchar WriteCard4_6(uchar *CardBuff);
void MoneyArray(ulong money,uchar *pData);
void SecDispose();//�봦����
void MakeWater();//��ˮ�߼���ÿ�봦��һ��
void AdvertisingLamp();//�����ж�//������������ʵ��Ż�������,��������ִ�к�����
void HeatingControl();//���ȿ��ƣ���������ִ�к�����
void OzoneControl();

void SPI_Init(void);
uchar SPI_ReadWriteByte(uchar Byte);

void delay1(unsigned char dlength);

bit CompCRC(uchar Str[]);
void PushCRC(uchar Str[]);//�������з���CRC-----------------------
void SendFixedLength(uchar Str[]);//���Ͷ������ݣ��������ֽڱ�ʾ����
void ReadSecNum();//�ϵ��ȡ��ʷ����ָ��
uchar strsearch(uchar *ptr2,uchar *ptr1_at);//���ַ���*ptr2��*ptr1�е�λ��
void PushZhenNum(uchar Str[]);
uint CRC_Calculate(uchar Length,uchar *Address);
void  WriSecNum();
void SendDataPackage(uchar Str[]);
void Int_Array(uint int1,uchar *pArray);
void JiTimerDelay();
void SaveExpenditureData();