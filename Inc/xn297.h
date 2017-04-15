#ifndef   _XN297_H_
#define   _XN297_H_

#include "stm32f10x.h"

#define         DEFAULT_CHANNEL      0						//初始化时的频率：  2400 MHz           
#define         PAYLOAD_WIDTH        64						//Payload宽度：     64bytes(最大支持32或者64bytes)				
#define         TRANSMIT_TYPE        TRANS_ENHANCE_MODE		//TRANS_BURST_MODE 	//使用增强型模式           TRANS_BURST_MODE    TRANS_ENHANCE_MODE
#define         DATA_RATE            DR_1M                	//通信速率1Mbps     DR_2M    DR_1M
#define         RF_POWER             (RF13dBm|DATA_RATE)	//发射功率13dBm 



#define         TRANS_ENHANCE_MODE    1      //传输类型增强型
#define         TRANS_BURST_MODE      2      //传输类型普通型

#define     RF13dBm     	0x3f			//44//0x3F      // 13dBm 
#define     RF10dBm			0X0F                            // 10dBm 
#define     RF8dBm        	0x15                            // 8dbm      
#define     RF7dBm         	0x07                            // 7dbm   
#define     RF5dBm        	0x2c                            // 5dbm   
#define     RF4dBm         	0x06                            // 4dbm   
#define     RF2dBm         	0x05                            // 2dbm  
#define     RF0dBm        	0X0B                            // 0dBm  
#define     RF_3dBm       	0x04                            // -3dBm     
#define     RF_6dBm        	0x0A                            // -6dBm 
#define     RF_10dBm      	0x02                            // -10dBm 
#define     RF_18dBm      	0x01                            // -18dBm 
#define     RF_30dBm      	0x00                            // -30dBm 


/********************SPI  REGISTER  *********************/
#define		R_REGISTER			0x00                        //SPI read RF data
#define		W_REGISTER			0x20                        //SPI write RF data
#define		R_RX_PAYLOAD		0x61                   		//Read RX Payload
#define		W_TX_PAYLOAD		0xA0                   		//Write TX Payload
#define		FLUSH_TX			0xE1                        //Flush RX FIFO
#define		FLUSH_RX			0xE2                        //Flush TX FIFO
#define		REUSE_TX_PL			0xE3                        //Reuse TX Payload
#define		ACTIVATE			0x50                        //ACTIVATE
#define		DEACTIVATE			0x50                        //DEACTIVATE
#define		R_RX_PL_WID			0x60                        //Read width of RX data 
#define		W_ACK_PAYLOAD		0xA8                        //Data with ACK
#define		W_TX_PAYLOAD_NOACK	0xB0                        //TX Payload no ACK Request
#define		CE_FSPI_ON	        0xFD                        // CE HIGH
#define		CE_FSPI_OFF	        0xFC                        // CE LOW
#define		RST_FSPI	        0x53                        // RESET
#define		NOP_N				0xFF
#define     TX_DS               0x20      

/******************CONTROL  REGISTER*********************/
#define		CONFIG              0x00            
#define		EN_AA				0x01
#define		EN_RXADDR			0x02
#define		SETUP_AW			0x03
#define		SETUP_RETR			0x04
#define		RF_CH				0x05
#define		RF_SETUP			0x06
#define		STATUS				0x07
#define		OBSERVE_TX			0x08
#define		RPD			        0x09
#define		RX_ADDR_P0			0x0A
#define		RX_ADDR_P1			0x0B
#define		RX_ADDR_P2			0x0C
#define		RX_ADDR_P3			0x0D
#define		RX_ADDR_P4			0x0E
#define		RX_ADDR_P5			0x0F
#define		TX_ADDR				0x10
#define		RX_PW_P0			0x11
#define		RX_PW_P1			0x12
#define		RX_PW_P2			0x13
#define		RX_PW_P3			0x14
#define		RX_PW_P4			0x15
#define		RX_PW_P5			0x16
#define		FIFO_STATUS			0x17
#define		DEM_CAL				0x19
#define     RF_CAL2				0x1A
#define     DEM_CAL2			0x1B
#define		DYNPD				0x1C
#define		FEATURE				0x1D	
#define		RF_CAL				0x1E
#define		BB_CAL				0x1F

/*******************CONTROL CMD**************************/
#define         DR_1M     		0X00	//通信速率 1Mbps
#define         DR_2M           0X40	//通信速率 2Mbps
#define         DR_250K         0XC0	//通信速率 2Mbps


uint8_t XN_Write_Reg(uint8_t reg, uint8_t value);
uint8_t XN_Read_Reg(uint8_t reg);
uint8_t XN_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t XN_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);

void XN297_Init(void);                                                    
void XN297_TxMode(void);
void XN297_RxMode(void);
u8   XN297_GetStatus(void);
void XN297_ClearStatus(void);
void XN297_ClearFIFO(void);
void XN297_SetChannel( uint8_t Channel);
void XN297_TxData( uint8_t *ucPayload,  uint8_t length);
u8	 XN297_DumpRxData( uint8_t *ucPayload,  uint8_t length);

#endif


