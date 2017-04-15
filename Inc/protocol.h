#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "stm32f10x.h"

extern unsigned char TX_ADDRESS_DEF[5];

enum rc_channel
{	
	ROLL,
	PITCH,
	YAW,
	THRO,
};


typedef struct
{
	u8 key;
	u8 change_addr;
	u8 modifyaddr;
	u8 TX_Addr[6];
	u8 TX_Channel[4];
	u16 ADDR_Channel[5];
	u16 Stick_Data[4];
	
}golbal_flag;
extern golbal_flag flag; 


typedef struct
{
	u8 header;
	u8 rate_mode;
	u8 flip_headfree;
	u8 ch_inv;
	u8 roll_h;
	u8 roll_l;
	u8 pitch_h;
	u8 pitch_l;
	u8 yaw_h;
	u8 yaw_l;
	u8 thro_h;
	u8 thro_l;
	
}remoteMsg;
extern remoteMsg remoteData;


void modifyAddress(void);
void rx_send(void);

#endif

