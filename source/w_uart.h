#ifndef	_W_UART_H
#define	_W_UART_H

#include "w_main.h"
#include "w_eeprom.h"

#define HeartInterval	30		//30�뷢��һ�����������
#define	StateInterval	15		//15���ӷ���һ��״̬�����
#define TimeInterval	30		//30����Уʱһ��
#define	DataInterval	2//���ݰ����Ϊ500���룬��ʱ������100ms����

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
uint8_t SearchID();     //���ҿ�ID��
void InitModel();//�����봦������
void FrameHead();//�̶�֡ͷ
void SendDataJoint(void);
void W_Send2001Data_1();
void W_Send2001Data_2();
void W_Send2001Data_3();//���͸���ʾ��
void W_Joint2001Data();
uint8_t strsearch(uint8_t *ptr2,uint8_t *ptr1_at);//���ַ���*ptr2��*ptr1�е�λ��
void Int_Array(uint16_t int1,uint8_t *pArray);
uint16_t Array_Int(uint8_t *pArray);
void SendFixedLength(uint8_t uartx,uint8_t Str[]);
void SendDataPackage(uint8_t uartx,uint8_t Str[]);
void PushCRC(uint8_t Str[]);
void PushZhenNum(uint8_t Str[]);
uint16_t CRC_Calculate(uint8_t Length,uint8_t *Address);
bit CompCRC(uint8_t Str[]);
void delay();    //��΢��ʱ  6usԼ>4.7us
void iic_start();   //�����ź�
void iic_stop();  //ֹͣ�ź�
void iic_ack();   //Ӧ���ź�
void iic_send_byte(uint8_t bat);  //��������
uint8_t iic_rev();  //��������
void iic_send_add_byte(uint8_t add,uint8_t bat); //��ĳ����ַ����ĳ����
uint8_t iic_rec_add_byte(uint8_t add); //��ĳ����ַ��������
void time_init(uint8_t Year,uint8_t Moom,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec,uint8_t Week);
void pcf_init();
uint8_t dec_bcd(uint8_t bat);
uint8_t bcd_dec(uint8_t bat);
void Readtime();



#endif