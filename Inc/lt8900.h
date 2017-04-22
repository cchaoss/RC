#ifndef _LT8900_H_
#define _LT8900_H_
#include "sys_fun.h"
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

#define PTK PAin(11)  //IRQ


u16 LT8900_Read_REG(u8 reg);
u8 LT8900_Read_REG_U8(u8 reg);
void LT8900_Write_REG(u8 reg, u8 dataH, u8 dataL);
void LT8900_Write_REG_U8(u8 reg, u8 data);

void LT8900_Config(void);
void LT8900_Set_Rx_Mode(void);
void LT8900_init(void);
void LT8900_Send_Date(u8 *data,u8 length);
void LT8900_Recv_Data(void);

extern u8 LT8900_Recv_Buffer[MAXSize];
extern u8 LT8900_Send_Buffer[MAXSize];


#endif

