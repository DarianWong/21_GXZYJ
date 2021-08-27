#ifndef	_W_MAIN_H
#define	_W_MAIN_H

#include "STC8.H"
#include <absacc.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <intrins.h>

typedef unsigned char uint8_t;
typedef unsigned int	uint16_t;
typedef unsigned long	uint32_t;

#define	SheClass	0x56

void SysInit(void);
void ReadEquipment();

#endif