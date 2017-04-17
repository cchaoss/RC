#ifndef _LT8900_H_
#define _LT8900_H_
#include "stm32f10x.h"

#define	IDLE		0
#define STANDBY		1
#define RECEIVE		2
#define SENDDATA	3
#define TESTPA		4
#define ACK			5
#define NO_CHANGE   255
#define MAXSize  32

#define LT8900_WRITE		0x7F
#define LT8900_READ			0x80


void LT8900_init(void);
void LT8900_Pin_init(void);
void LT8900_Config(void);
void LT8900_Set_Recv_Mode(void);
u16 LT8900_Read_REG(u8 reg);
void LT8900_Write_REG(u8 reg, u16 data);
void LT8900_Send_Date(void);
void LT8900_Recv_Data(void);


extern u8 LT8900_Recv_Buffer[MAXSize];
extern u8 LT8900_Send_Buffer[MAXSize];


#endif

