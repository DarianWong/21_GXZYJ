/*
20210529��Т��Ҫ��ÿ��ˮ20������һ����ˮ����ÿ��һ��ˮ����һ�����ݸ���ʾ�������ʾ

*/

#include "w_main.h"
#include "w_timer.h"
#include "w_uart.h"
#include "w_show.h"
#include "w_work.h"
#include "w_adc.h"
#include "w_eeprom.h"

extern uint8_t SoundFlag;
extern	bit Sys1sFlag,Sys100msFlag,Sys200msFLag,Sys1mFlag,SendShowStateFlag;
extern uint8_t code Cdatas[4];
extern bit TestFlag,PowerOn,ShutDown,NetWork;
extern uint8_t WorkStateByte,ShowState;
uint32_t EquipmentNumber=0;
extern uint8_t SendDataInterval,S2PacketNum,NetProce;
extern bit NetTimeFlag,PowerDown;

extern struct 
{ uint8_t		FirstChar;   //��ͷ      1B
	uint16_t	CardPrice1;			//ˢ������
	uint16_t	CardPrice2;			//ˢ������
	uint16_t	VirtualPrice1;	//ɨ�뵥��
	uint16_t	VirtualPrice2;	//ɨ�뵥��
	uint16_t	Pluse1;					//1�ſ�������������
	uint16_t	Pluse2;					//2�ſ�������������
	uint16_t	Pluse3;					//��ˮ������������
	uint16_t	MaxMoney;				//��󵥴γ�ˮ��
	
	uint8_t		ParameterLock;	//�豸������
//	uint8_t		Confiscate;		//û��ʱ��				1B
	uint8_t 	OnceCXTime;	//���γ�ϴʱ��					1B
	uint8_t		CXInterval;	//��ϴ���							1B
	uint8_t		HaltState;//ͣ��״̬
		uint8_t 	OnceOzoneTime;//��������ʱ��				1B
		uint8_t		OzoneInterval;//�����������ʱ��		1B
	uint8_t 	LastChar;    //��β      1B
}SetParameterData;
void main(void)
{
	SysInit();
	ReadEquipment();
	W_EepromData();
	S2PacketNum=0;
	W_Joint2001Data();
	W_Send2001Data_1();W_Send2001Data_2();
	WDT_CONTR=0x35;//
	
	while(1)
	{
		WDT_CONTR=0x35;//
//		if(Sys1sFlag)
//		{
//			Sys1sFlag=0;Uart1Send(UsedWater.LXLife1);Uart1Send(UsedWater.LXLife4);//Uart1Send(sizeof(UsedWater.ZSYield));Uart1Send((UsedWater.FirstChar));
//		}
		if(ShutDown)//������ڹػ�״̬
		{
			WorkStateByte&=0xfe;ShowState&=0xfe;
			//if(PowerOn==0)WDT_CONTR=0x35;�ر���ʾ//memset(DisBuf,0,16);Update_Display();
			W_CloseAll();//�ر��������
		}
		else
		{
			ShowState|=0x01;
			if(TestFlag)
			{
				W_OpenAll();W_CheckWaterLevel();
			}
			else
			{
//				if(Sys100msFlag){Sys100msFlag=0;Uart2Send(SetParameterData.HaltState);}
				W_WorkLogic();
			}
			W_Menu();
		}
		W_Show();
		if(SendShowStateFlag)
		{
			SendShowStateFlag=0;W_SendShowState();
		}
//		NetProce=0x50;NetWork=1;
		if(NetWork==0||NetProce<0x50){if(NetTimeFlag){NetTimeFlag=0;if(PowerDown)InitModel();}}
		if(SendDataInterval==0&&NetWork){SendDataJoint();}
		UartDataDispose();//NetProce=0x50;NetWork=1;//Uart2Send(2);
//		Uart1Send(1);Uart3Send(3);Uart4Send(4);
	}
}
void SysInit(void)//ϵͳ��ʼ��
{
	P0M0 = 0x90;P0M1 = 0x10;       //����P04Ϊ��©����ڣ�����׼˫��
	P1M0 = 0x00;P1M1 = 0xf8;       //����P13-P17ΪADC��
	P2M0 = 0x40;P2M1 = 0x4e;       //����P04Ϊ�����
	P3M0 = 0x80;P3M1 = 0x00;       //����P3ȫΪ׼˫��
	P4M0 = 0x16;P4M1 = 0x00;       //����P41,P44Ϊ�����
	P5M0 = 0x01;P5M1 = 0x00;       //����P5ȫΪ׼˫��
	P6M0 = 0x00;P6M1 = 0xc3;       //����P63Ϊ�����,P60,61,66,67Ϊ��������
	P7M0 = 0x0f;P7M1 = 0x00;       //����P70-P74Ϊ�����
	W_CloseAll();
	M800Power=1;
	W_TimerConfig();
	W_ExintConfig();
	W_ADCConfig();
	W_UartConfig();
	EA=1;
	Update_Display();
	pcf_init();Readtime();
	
}
void ReadEquipment()
{
	EquipmentNumber=Cdatas[0];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[1];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[2];EquipmentNumber*=0x100;
  EquipmentNumber+=Cdatas[3];
	
}