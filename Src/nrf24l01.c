#include "string.h"
#include "stdbool.h"
#include "nrf24l01.h"
#include "spi.h"
#include "delay.h"
#include "protocol.h"

uint8_t NRF24L01_RXDATA[NRF_PLOAD_WIDTH];//nrf24l01接收到的数据
uint8_t NRF24L01_TXDATA[NRF_PLOAD_WIDTH];//nrf24l01需要发送的数据


uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    SPI_CSN_L();
    status = SPI_RW(reg);  
    SPI_RW(value);		
    SPI_CSN_H();		
    return 	status;
}


uint8_t NRF_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    SPI_CSN_L();					 
    SPI_RW(reg);			  
    reg_val = SPI_RW(0);	
    SPI_CSN_H();	
 
    return 	reg_val;
}



uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
    uint8_t i;
    uint8_t status;
    SPI_CSN_L();			//选通器件
    status = SPI_RW(reg);	//写寄存器地址 
    for(i=0; i<uchars; i++)
    {
        SPI_RW(pBuf[i]);	//写数据 
    }
    SPI_CSN_H();			//禁止该器件
    return 	status;	
}


//读缓冲区
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
    uint8_t i;
    uint8_t status;
    SPI_CSN_L();				
    status = SPI_RW(reg);	
    for(i=0; i<uchars; i++)
    {
        pBuf[i] = SPI_RW(0); 
    }
    SPI_CSN_H();						
    return 	status;
}


//写数据包
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len)
{	
    SPI_CE_L();		 //StandBy I模式	
    NRF_Write_Buf(WR_TX_PLOAD, tx_buf, len);// 装载数据	
    SPI_CE_H();		 //置高CE，激发数据发送
}


void SetRX_Mode(void)
{
    SPI_CE_L();
	NRF_Write_Reg(FLUSH_RX,0xff);//清除TX FIFO寄存器			 
  	NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)TX_ADDRESS_DEF,RX_ADR_WIDTH);//写RX节点地址
   	NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  	NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  	NRF_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  	NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,NRF_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  	NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
    SPI_CE_H();
    
} 

void SetTX_Mode(void)
{
    SPI_CE_L();
    NRF_Write_Reg(FLUSH_TX,0xff);										//清除TX FIFO寄存器		  
    NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS_DEF,TX_ADR_WIDTH);		//写TX节点地址 
  	NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)TX_ADDRESS_DEF,RX_ADR_WIDTH); 	//设置TX节点地址,主要为了使能ACK	  
  	NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  	NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  	NRF_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  	NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
    SPI_CE_H();
} 


//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
	SPI_CE_L();//NRF24L01_CE=0;
	NRF_Write_Buf(WR_TX_PLOAD,txbuf,NRF_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	SPI_CE_H();//NRF24L01_CE=1;//启动发送	
	while(NRF24L01_IRQ!=0);//等待发送完成
	sta = NRF_Read_Reg(NRFRegSTATUS);  //读取状态寄存器的值	   
	NRF_Write_Reg(NRF_WRITE_REG+NRFRegSTATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}

//接受数据
void Nrf_Irq(void)
{
    uint8_t sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);
    if(sta & (1<<RX_DR))//接收轮训标志位
    {
			NRF_Read_Buf(RD_RX_PLOAD,NRF24L01_RXDATA,NRF_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
				
			memcpy(&remoteData,NRF24L01_RXDATA,sizeof(remoteData));
			
			NRF_Write_Reg(NRF_WRITE_REG+NRFRegSTATUS, sta);//清除nrf的中断标志位
    }
 
}


//初始化
bool NRF24L01_INIT(void)
{
   SPI1_INIT();
   if(NRF24L01_Check()) {
	   SetRX_Mode();
	   return true;
   }
   else return false;
}

//判断SPI接口是否接入NRF芯片是否可用
bool NRF24L01_Check(void) 
{ 
   NRF_Write_Reg(NRF_WRITE_REG+TX_ADDR,0x32); 

   if(NRF_Read_Reg(TX_ADDR) == 0x32) 
		return true;
   else return false;
} 

