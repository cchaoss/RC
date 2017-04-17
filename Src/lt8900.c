#include "lt8900.h"
#include "delay.h"
#include "spi.h"

#define PTK GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)

u8 LT8900_Recv_Buffer[MAXSize];
u8 LT8900_Send_Buffer[MAXSize];
u8 LT8900_Recv_Len = 16;
u8 LT8900_Send_Len = 16;

void LT8900_init(void)
{
	SPI1_INIT();//初始化硬件SPI
	delay_ms(2);
   
    LT8900_Write_REG(0, 0x6fe0);
    LT8900_Write_REG(1, 0x5681);  
    LT8900_Write_REG(2, 0x6617);  
    LT8900_Write_REG(4, 0x9cc9);  
    LT8900_Write_REG(5, 0x6637);  
    LT8900_Write_REG(7, 0x0000);  
    LT8900_Write_REG(8, 0x6c90);
    LT8900_Write_REG(9, 0x4800);	// 5.5dBm
    LT8900_Write_REG(10, 0x7ffd);
    LT8900_Write_REG(11, 0x0008);
    LT8900_Write_REG(12, 0x0000);
    LT8900_Write_REG(13, 0x48bd);
    
    LT8900_Write_REG(22, 0x00ff);
    LT8900_Write_REG(23, 0x8005);
    LT8900_Write_REG(24, 0x0067);
    LT8900_Write_REG(25, 0x1659);
    LT8900_Write_REG(26, 0x19e0);
    LT8900_Write_REG(27, 0x1300);	//1200
    LT8900_Write_REG(28, 0x1800);
    
    LT8900_Write_REG(32, 0x5000);	//5004
    LT8900_Write_REG(33, 0x3fc7);
    LT8900_Write_REG(34, 0x2000);
    LT8900_Write_REG(35, 0x0F00);	//0300  0320
    LT8900_Write_REG(36, 0x0380);
    LT8900_Write_REG(37, 0x0380);
    LT8900_Write_REG(38, 0x5a5a);
    LT8900_Write_REG(39, 0x0380);
    LT8900_Write_REG(40, 0x4401);
    LT8900_Write_REG(41, 0xB800);  //crc on scramble off ,1st byte packet length ,auto ack on
    LT8900_Write_REG(42, 0xfdb0);  //176us
    LT8900_Write_REG(43, 0x000f);
    LT8900_Write_REG(50, 0x0000);
	delay_ms(200);
    LT8900_Write_REG(7, 0x0100);
	delay_ms(2);
    LT8900_Write_REG(7, 0x0030);
}



u16 LT8900_Read_REG(u8 reg)
{
	u16 temp;
	SPI_CSN_L();
	SPI_I2S_SendData(SPI1, LT8900_READ | reg);
	temp = SPI_I2S_ReceiveData(SPI1);
	SPI_CSN_H();
	return temp;
}

void LT8900_Write_REG(u8 reg, u16 data)
{
		SPI_CSN_L();
		SPI_I2S_SendData(SPI1, LT8900_WRITE & reg);
		SPI_I2S_SendData(SPI1, data);
		SPI_CSN_H();
}

void LT8900_Send_Date(void)
{
		u8 i;
		LT8900_Write_REG(7, 0x0000);	//2402 + 48 = 2.45GHz
		LT8900_Write_REG(52, 0x8000);	//清空发送缓存区	
	
		LT8900_Write_REG(50, LT8900_Send_Len);	//发送LT8900_Send_Len + 1个字节(fifo一次只能读1byte)
		for(i = 0; i < LT8900_Send_Len ; i++)
				LT8900_Write_REG(50,LT8900_Send_Buffer[i]);

		LT8900_Write_REG(7, 0x0130);	//允许发射使能
		while (PTK == 0);	//PTK=1，发送完毕
}

void LT8900_Config(void)
{
		LT8900_Write_REG(32, 0x4800);   
		LT8900_Write_REG(34, 0x2000);	//传输CW调制信号的时间 
		LT8900_Write_REG(11, 0x0008);	//关闭RSSI
}

void LT8900_Set_Recv_Mode(void)
{
		LT8900_Write_REG(7, 0x0030);
		delay_ms(3);
		LT8900_Write_REG(52, 0x0080);	//清接收缓存区
		LT8900_Write_REG(7, 0x00B0);	//允许接收使能
		delay_ms(5);					
}

void LT8900_Recv_Data(void)
{
		u16 temp;
		u8 i;
		if (PTK == 1)	//PTK=1，接收到有效数据
		{
			temp = LT8900_Read_REG(48);	//Read CRC result
			if (0x0000 == (temp & 0x8000))
			{
					temp = LT8900_Read_REG(50);
					LT8900_Recv_Len = temp;
					for(i = 0;i < LT8900_Recv_Len; i++)
						LT8900_Recv_Buffer[i] = LT8900_Read_REG(50);
			}
			LT8900_Write_REG(52, 0x0080); 
			LT8900_Write_REG(7, 0x00B0);
		}				
}
