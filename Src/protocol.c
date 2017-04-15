#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "led.h"
#include "stm32f10x_it.h"
#include "xn297.h"
#include "protocol.h"
#include "stmflash.h"

u8 TX_ADDRESS_DEF[5] = {0,0,0,0,0};
u8 TX_Channel_DEF[4] = {10,20,30,40};

golbal_flag flag;
remoteMsg remoteData;

uint8_t RX_DATA_BUF[PAYLOAD_WIDTH],TX_DATA_BUF[PAYLOAD_WIDTH];


void modifyAddress(void)
{
	static bool i;
	static uint8_t count;
	
	if(flag.change_addr)//��һ���룬�ı�һ�ε�ַ
	{
		flag.change_addr = 0;
		
		XN297_SetChannel(DEFAULT_CHANNEL);//�����ʱ��һֱʹ��Ĭ���ŵ���Ĭ�ϵ�ַ
		XN_Write_Buf(W_REGISTER + TX_ADDR, ( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));//Writes TX_Address to PN006
		
		flag.ADDR_Channel[1] = micros();
		flag.ADDR_Channel[2] = micros()>>1;
		flag.TX_Addr[0] = 0x2a;//
		flag.TX_Addr[1] = flag.ADDR_Channel[1];
		flag.TX_Addr[2] = flag.ADDR_Channel[1]>>8;
		flag.TX_Addr[3] = flag.ADDR_Channel[2];
		flag.TX_Addr[4] = flag.ADDR_Channel[2]>>8;
		flag.TX_Addr[5] = 0;//useless?
		
		flag.TX_Channel[0] = micros()%30;//��ȡһ��С��30�������
		flag.TX_Channel[1] = flag.TX_Channel[0]+20;
		flag.TX_Channel[2] = flag.TX_Channel[1]+20;
		flag.TX_Channel[3] = flag.TX_Channel[2]+20;//ÿ���ŵ�֮����20Mhz����ֹ�����ŵ�����
		flag.ADDR_Channel[3] = (flag.TX_Channel[0]<<8) | flag.TX_Channel[1];
		flag.ADDR_Channel[4] = (flag.TX_Channel[2]<<8) | flag.TX_Channel[3];

		flag.ADDR_Channel[0] = 0x1234;
		STMFLASH_Write(TABLE_ADDRESS,flag.ADDR_Channel,sizeof(flag.ADDR_Channel));//д���ڲ�flash
	}
	
	memcpy(RX_DATA_BUF,&remoteData,sizeof(remoteData));
	XN297_TxData(RX_DATA_BUF,  sizeof(remoteData));//����ƥ���ַ���ŵ�����Ϣ
	
	count++;
	
	if(count > 10) {
		count = 0;
		i = !i;
		LedSet(5,i);
	}


}


void rx_send(void)
{
	static u8 count;
	memcpy(RX_DATA_BUF,&remoteData,sizeof(remoteData));
	XN297_TxData(RX_DATA_BUF, sizeof(remoteData));
	XN297_SetChannel(flag.TX_Channel[count & 0x11]);//�л�����һ�ŵ�
	count++;
	

	
}

