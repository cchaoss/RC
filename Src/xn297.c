#include "xn297.h"
#include "protocol.h"
#include "delay.h"
#include "spi.h"


uint8_t XN_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    SPI_CSN_L();
    status = SPI_RW(reg);  
    SPI_RW(value);		 
    SPI_CSN_H();					
    return 	status;
}



uint8_t XN_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    SPI_CSN_L();					 
    SPI_RW(reg);			  
    reg_val = SPI_RW(0);	 
    SPI_CSN_H();	
    return 	reg_val;
}

uint8_t XN_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
    uint8_t i;
    uint8_t status;
    SPI_CSN_L();				       
    status = SPI_RW(reg);	
    for(i=0; i<uchars; i++)
    {
        SPI_RW(pBuf[i]);		
    }
    SPI_CSN_H();						
    return 	status;	
}


uint8_t XN_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
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



void XN297_TxMode(void)
{
    SPI_CE_L();
	XN_Write_Reg(W_REGISTER + CONFIG,  0X8E);
    delay_us(10);
}

void XN297_RxMode(void)
{
    SPI_CE_L();
    XN_Write_Reg(W_REGISTER + CONFIG,  0X8F );					
    SPI_CE_H();	// Set CE pin high ��ʼ��������
    delay_us(2000);
}

//��ȡRF��״̬ 
unsigned char XN297_GetStatus(void)
{
   uint8_t state;
   state=XN_Read_Reg(STATUS)&0x70;
   return state;								
}

//���RF��IRQ��־ 
void XN297_ClearStatus(void)
{
    XN_Write_Reg(W_REGISTER + STATUS,0x70);							
}

//���FIFO
void XN297_ClearFIFO(void)
{
    XN_Write_Reg(FLUSH_TX, 0);//���RF �� TX FIFO		
    XN_Write_Reg(FLUSH_RX, 0);//���RF �� RX FIFO	
}


//Set RF TX/RX channel:Channel
void XN297_SetChannel( uint8_t Channel)
{    
    SPI_CE_L();
    XN_Write_Reg(W_REGISTER + RF_CH, Channel);
}

/******************************************************************************/
//�������ݣ�
//������1. ucPayload����Ҫ���͵������׵�ַ
//      2. length:  ��Ҫ���͵����ݳ���
//Return:
//      1. MAX_RT: TX Failure  (Enhance mode)
//      2. TX_DS:  TX Successful (Enhance mode)
//      note: Only use in Tx Mode
//      length ͨ������ PAYLOAD_WIDTH
/******************************************************************************/
void XN297_TxData( uint8_t *ucPayload,  uint8_t length)
{
    /*if(0==ucRF_GetStatus())                                                       // rf free status                                                                                                                                                                   
   {
    RF_WriteBuf(W_TX_PAYLOAD, ucPayload, length); 
    CE=1;                                                                    		//rf entery tx mode start send data 
    Delay20us();                                                              		//keep ce high at least 600us
    CE=0;                                                                                     //rf entery stb3                                                        			
   } */
	uint8_t   Status_Temp,i = 0,flag = 0;
    
    SPI_CE_H();                                                                    		//rf entery tx mode start send data 
    delay_us(20);                                                             		//keep ce high at least 16us
    SPI_CE_L();
	XN_Write_Buf(W_TX_PAYLOAD, ucPayload, length);                               		//write data to txfifo        
  
	SPI_CE_H();                                                                  		//rf entery tx mode start send data 
    delay_us(20);                                                             		//keep ce high at least 16us
    SPI_CE_L();
			
				                                                                   	//rf entery stb3
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) & flag)                          //waite irq pin low 
	{
		delay_us(25);
		if(++i > 80)  flag = 0;
	}
    Status_Temp = XN_Read_Reg(STATUS) & 0x70;                                      //��ȡ������ɺ��status
    XN_Write_Reg(W_REGISTER + STATUS, Status_Temp);                                 	//���Status
    XN_Write_Reg(FLUSH_TX,0); 
	/* if(ucRF_ReadReg(OBSERVE_TX) > 0xCF)						
    {
	RF_SetChannel(ucCurrent_Channel);					//��0 OBSERVE_TX
    } */                                                //�� FIFO
   
    //return Status_Temp;

}	


/******************************************************************************
�������յ������ݣ�
������
1. ucPayload���洢��ȡ�������ݵ�Buffer
2. length:    ��ȡ�����ݳ���
����ֵ:
0: û�н��յ�����
1: ��ȡ���յ������ݳɹ�
******************************************************************************/
unsigned char XN297_DumpRxData( uint8_t *ucPayload,  uint8_t length)
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1)	//��IRQ PINΪ�ߣ���û�н��յ�����
      return 0;                                                                 
   
    SPI_CE_L();
    XN_Read_Buf(R_RX_PAYLOAD, ucPayload, length);		//�����յ������ݶ�����ucPayload�������rxfifo
    XN_Write_Reg(FLUSH_RX, 0);	
    XN_Write_Reg(W_REGISTER + STATUS, 0x70);		//���Status
    SPI_CE_H();
    return 1;
}

//����XN297��ƵоƬ
void XN297_Init(void)
{
    uint8_t  BB_cal_data[]    = {0x0A,0x6D,0x67,0x9C,0x46}; 
    uint8_t  RF_cal_data[]    = {0xF6,0x37,0x5D};
    uint8_t  RF_cal2_data[]   = {0x45,0x21,0xef,0x2C,0x5A,0x50};
    uint8_t  Dem_cal_data[]   = {0x01};  
    uint8_t  Dem_cal2_data[]  = {0x0b,0xDF,0x02};   
	//IRQ_STATUS=0;
	
    SPI1_INIT();//��ʼ��Ӳ��SPI
	delay_ms(2);
    SPI_CE_L();
	SPI_CSN_H();
	
    XN_Write_Reg(RST_FSPI, 0x5A);								//Software Reset    			
    XN_Write_Reg(RST_FSPI, 0XA5);
    
    XN_Write_Reg(FLUSH_TX, 0);									// CLEAR TXFIFO		    			 
    XN_Write_Reg(FLUSH_RX, 0);									// CLEAR  RXFIFO
    XN_Write_Reg(W_REGISTER + STATUS, 0x70);					// CLEAR  STATUS	
    XN_Write_Reg(W_REGISTER + EN_RXADDR, 0x01);					// Enable Pipe0
    XN_Write_Reg(W_REGISTER + SETUP_AW,  0x03);					// address witdth is 5 bytes
    XN_Write_Reg(W_REGISTER + RF_CH, DEFAULT_CHANNEL);    		// 2400M HZ
    XN_Write_Reg(W_REGISTER + RX_PW_P0,  PAYLOAD_WIDTH);		// 8 bytes
    XN_Write_Buf(W_REGISTER + TX_ADDR,   ( uint8_t*)flag.TX_Addr, sizeof(flag.TX_Addr));	// Writes TX_Address to PN006
    //XN_Write_Buf(W_REGISTER + RX_ADDR_P0,( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));	// RX_Addr0 same as TX_Adr for Auto.Ack   
    XN_Write_Buf(W_REGISTER + BB_CAL,    BB_cal_data,  sizeof(BB_cal_data));
    XN_Write_Buf(W_REGISTER + RF_CAL2,   RF_cal2_data, sizeof(RF_cal2_data));
    XN_Write_Buf(W_REGISTER + DEM_CAL,   Dem_cal_data, sizeof(Dem_cal_data));
    XN_Write_Buf(W_REGISTER + RF_CAL,    RF_cal_data,  sizeof(RF_cal_data));
    XN_Write_Buf(W_REGISTER + DEM_CAL2,  Dem_cal2_data,sizeof(Dem_cal2_data));
    XN_Write_Reg(W_REGISTER + DYNPD, 0x00);					
    XN_Write_Reg(W_REGISTER + FEATURE, 0x00);
    XN_Write_Reg(W_REGISTER + RF_SETUP,  RF_POWER);				//DBM  		
  
    
#if(TRANSMIT_TYPE == TRANS_ENHANCE_MODE)    //ʹ����ǿ��ģʽ��  
    XN_Write_Reg(W_REGISTER + SETUP_RETR,0x03);					//  3 retrans... 	
    XN_Write_Reg(W_REGISTER + EN_AA,     0x01);					// Enable Auto.Ack:Pipe0  	
#elif(TRANSMIT_TYPE == TRANS_BURST_MODE)                                                              
    XN_WriteReg(W_REGISTER + SETUP_RETR,0x00);					// Disable retrans... 	
    XN_WriteReg(W_REGISTER + EN_AA,     0x00);					// Disable AutoAck 
#endif

	if(PAYLOAD_WIDTH <33)											
		XN_Write_Reg(W_REGISTER +FEATURE, 0x00);				//�л���32byteģʽ
	else
		XN_Write_Reg(W_REGISTER +FEATURE, 0x18);				//�л���64byteģʽ	   
}


