#ifndef	_W_UART_H
#define	_W_UART_H

#include "w_main.h"
#include "w_eeprom.h"

#define HeartInterval	30		//30秒发送一次心跳包间隔
#define	StateInterval	15		//15分钟发送一次状态包间隔
#define TimeInterval	30		//30分钟校时一次
#define	DataInterval	2//数据包间隔为500毫秒，计时器放在100ms里面

sbit M800Power=P0^7;

sbit 	SCL_clock=P2^4;
sbit 	SDA_clock=P2^5;

void W_Uart1Config(void);
void W_Uart2Config(void);
void W_Uart3Config(void);
void W_Uart4Config(void);
void W_UartConfig(void);
void Uart1Send(char dat);
void Uart1SendStr(char *p);
void Uart2Send(char dat);
void Uart2SendStr(char *p);
void Uart3Send(char dat);
void Uart3SendStr(char *p);
void Uart4Send(char dat);
void Uart4SendStr(char *p);
void Uart1DataDispose();
void Uart2DataDispose();
void Uart3DataDispose();
void Uart4DataDispose();
void UartDataDispose();
void ReceiveTimeOut();
uint8_t SearchID();     //查找卡ID号
void InitModel();//放在秒处理函数里
void FrameHead();//固定帧头
void SendDataJoint(void);
void W_Send2001Data_1();
void W_Send2001Data_2();
void W_Send2001Data_3();//发送给显示屏
void W_Joint2001Data();
uint8_t strsearch(uint8_t *ptr2,uint8_t *ptr1_at);//查字符串*ptr2在*ptr1中的位置
void Int_Array(uint16_t int1,uint8_t *pArray);
uint16_t Array_Int(uint8_t *pArray);
void SendFixedLength(uint8_t uartx,uint8_t Str[]);
void SendDataPackage(uint8_t uartx,uint8_t Str[]);
void PushCRC(uint8_t Str[]);
void PushZhenNum(uint8_t Str[]);
uint16_t CRC_Calculate(uint8_t Length,uint8_t *Address);
bit CompCRC(uint8_t Str[]);
void delay();    //略微延时  6us约>4.7us
void iic_start();   //启动信号
void iic_stop();  //停止信号
void iic_ack();   //应答信号
void iic_send_byte(uint8_t bat);  //发送数据
uint8_t iic_rev();  //接受数据
void iic_send_add_byte(uint8_t add,uint8_t bat); //向某个地址发送某数据
uint8_t iic_rec_add_byte(uint8_t add); //从某个地址读出数据
void time_init(uint8_t Year,uint8_t Moom,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec,uint8_t Week);
void pcf_init();
uint8_t dec_bcd(uint8_t bat);
uint8_t bcd_dec(uint8_t bat);
void Readtime();



#endif