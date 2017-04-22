#include "stdio.h"
#include "string.h"
#include "lt8900.h"
#include "delay.h"
#include "spi.h"
#include "protocol.h"

u8 LT8900_Recv_Buffer[MAXSize];
u8 LT8900_Send_Buffer[MAXSize];


u16 LT8900_Read_REG(u8 reg)
{
	u16 temp;
	SPI_CSN_L();
	SPI_RW(LT8900_READ | reg);
	temp = SPI_RW(0);
	temp = temp <<8;
	temp |= SPI_RW(0);
	SPI_CSN_H();
	return temp;
}

u8 LT8900_Read_REG_U8(u8 reg)
{
	u8 temp;
	SPI_CSN_L();
	SPI_RW(LT8900_READ | reg);
	temp = SPI_RW(0);
	SPI_CSN_H();
	return temp;
}

void LT8900_Write_REG(u8 reg, u8 dataH, u8 dataL)
{
		SPI_CSN_L();
		SPI_RW(LT8900_WRITE & reg);
		SPI_RW(dataH);
		SPI_RW(dataL);
		SPI_CSN_H();
}

void LT8900_Write_REG_U8(u8 reg, u8 data)
{
		SPI_CSN_L();
		SPI_RW(LT8900_WRITE & reg);
		SPI_RW(data);
		SPI_CSN_H();
}



//LT8900初始化（默认设置为发射模式）
void LT8900_init(void)
{
	SPI1_INIT();
	SPI_CE_L();
    delay_ms(100);
    SPI_CE_H();
    delay_ms(200);
   
    //LT8900_Write_REG(0, 0x6f, 0xe0);
	LT8900_Write_REG(0, 0x6f, 0xef);
    LT8900_Write_REG(1, 0x56, 0x81);  
    LT8900_Write_REG(2, 0x66, 0x17);  
    LT8900_Write_REG(4, 0x9c, 0xc9);  
    LT8900_Write_REG(5, 0x66, 0x37);  
    LT8900_Write_REG(7, 0x00, 0x00);  
    LT8900_Write_REG(8, 0x6c, 0x90);
    LT8900_Write_REG(9, 0x48, 0x00);				// 5.5dBm
    LT8900_Write_REG(10, 0x7f, 0xfd);
    LT8900_Write_REG(11, 0x00, 0x08);
    LT8900_Write_REG(12, 0x00, 0x00);
    LT8900_Write_REG(13, 0x48, 0xbd);
    
    LT8900_Write_REG(22, 0x00, 0xff);
    LT8900_Write_REG(23, 0x80, 0x05);
    LT8900_Write_REG(24, 0x00, 0x67);
    LT8900_Write_REG(25, 0x16, 0x59);
    LT8900_Write_REG(26, 0x19, 0xe0);
    LT8900_Write_REG(27, 0x13, 0x00); //------------ 1200
    LT8900_Write_REG(28, 0x18, 0x00);
    
    LT8900_Write_REG(32, 0x50, 0x00); //5004
    LT8900_Write_REG(33, 0x3f, 0xc7);
    LT8900_Write_REG(34, 0x20, 0x00);
    LT8900_Write_REG(35, 0x0F, 0x00);//0300  0320
    LT8900_Write_REG(36, 0x03, 0x80);
    LT8900_Write_REG(37, 0x03, 0x80);
    LT8900_Write_REG(38, 0x03, 0x80);
    LT8900_Write_REG(39, 0x03, 0x80);
    LT8900_Write_REG(40, 0x44, 0x01);
    LT8900_Write_REG(41, 0xb8, 0x00);  //crc on scramble off ,1st byte packet length ,auto ack on
	LT8900_Write_REG(42, 0xfd, 0xff);  //255us 62.5kbps
    //LT8900_Write_REG(42, 0xfd, 0xb0);  //176us
    LT8900_Write_REG(43, 0x00, 0x0f);
	LT8900_Write_REG(44, 0x10, 0x00);
	LT8900_Write_REG(45, 0x05, 0x52);
    //LT8900_Write_REG(50, 0x00, 0x00);
}


void LT8900_Set_Rx_Mode(void)
{
		LT8900_Write_REG(52, 0x80,0X80);	//清接收缓存区
		LT8900_Write_REG(7, 0x00,0xb0);	//允许接收使能
		delay_ms(1);					
}

//数据接受
void LT8900_Recv_Data(void)
{
		u8 i,length;
		if (PTK == 1)	//PTK=1，接收到有效数据
		{
			length = LT8900_Read_REG_U8(50);
			for(i = 0;i < length; i++)
				LT8900_Recv_Buffer[i] = LT8900_Read_REG_U8(50);
			memcpy(&remoteData,LT8900_Recv_Buffer,sizeof(remoteData));/////
			LT8900_Write_REG(52, 0x00, 0x80); 
			LT8900_Write_REG(7, 0x00, 0XB0);
		}		
		
}



//数据发射
void LT8900_Send_Date(u8 *data,u8 length)
{
		u8 i;
		LT8900_Write_REG(52, 0x80,0X80);	//清空发送缓存区	
		LT8900_Write_REG(7, 0x00,0X00);		//进入发送模式
		
		LT8900_Write_REG_U8(50, length);
		for(i = 0; i < length ; i++)
				LT8900_Write_REG_U8(50,data[i]);

		LT8900_Write_REG(7, 0x01,0x30);		//允许发射使能  
		while (PTK == 0);	//PTK=1，发送完毕
}

