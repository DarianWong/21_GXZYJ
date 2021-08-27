#ifndef _W_SHOW_H
#define	_W_SHOW_H

#include "w_main.h"
#include "w_work.h"

sbit  CLK=P4^2;
sbit  DIN=P2^0;

#define WRITE_DATA_MODE_Z   0x40        // 地址自动加
#define WRITE_DATA_MODE_G   0x44        // 固定地址
#define START_DATA          0xC0        // 
#define DISPLAY_EN          0x8A        // 开显示
#define DISPLAY_DIS         0x80        // 关显示


#define Disp_A  0xa
#define Disp_B  0x0b
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

void I2CStart(void);
void I2CStop(void);
void I2CWritebyte(uint8_t oneByte);
void Update_Display(void);
void W_Show(void);
void W_MenuShow(void);
void  DispSetData(uint16_t int1);
void W_TestShow(void);

#endif