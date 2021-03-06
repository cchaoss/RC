#include "stdio.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "sys_fun.h"
#include "delay.h"
#include "uart.h"
#include "stmflash.h"
#include "led.h"
#include "key.h"
#include "stick.h"
#include "timer.h"
#include "xn297.h"
#include "nrf24l01.h"
#include "lt8900.h"
#include "protocol.h"


int main(void) {
	
	SystemClock_HSI(9);           			//系统时钟初始化，时钟源内部HSI
	cycleCounterInit();				    	//Init cycle counter
	SysTick_Config(SystemCoreClock/1000);	//SysTick开启系统tick定时器并初始化其中断，1ms
	UART1_init(SysClock,115200); 			//串口1初始化
	NVIC_INIT();	                		//中断初始化
	Read_ADDR_Channel();					//读出已保存的参数
	LedInit();	                  			//IO初始化
	Adc_Init();								//摇杆AD初始化
	KeyInit();								//按键初始化
#ifdef NRF24L01
	NRF24L01_INIT();
#endif
#ifdef XN297
	XN297_Init();							//初始化射频芯片
#endif
#ifdef LT8900	
	LT8900_init();
#endif
	TIM4_Init(SysClock,TIME4_Preiod);		//定时器4初始化，定时时间单位：(TIME4_Preiod)微秒
	LoadRCdata(AMERICAN_RC_MODE);           //摇杆赋值
	
	
	while(1) { 
		
		if(_20hz) {		
			_20hz = 0;
			checkKey();
			
			//printf("len:%d\r,thro:%d\r\r\r",LT8900_Recv_Buffer[0],LT8900_Recv_Buffer[1]);
			//printf("rol:%d\r,pit:%d\r,thr:%d\r,yaw:%d\r\r\r",remoteData.roll_h<<8|remoteData.roll_l,remoteData.pitch_h<<8|remoteData.pitch_l,remoteData.thro_h<<8|remoteData.thro_l,remoteData.yaw_h<<8|remoteData.yaw_l);
			//printf("modif:%d\r,change:%d\r\r\r",flag.modifyaddr,flag.change_addr);
			//printf("c0:%d\r,c1:%d\r,c2:%d\r,c3:%d\r\r\r",flag.TX_Channel[0],flag.TX_Channel[1],flag.TX_Channel[2],flag.TX_Channel[3]);
			//printf("d0:%d\r,d1:%d\r,d2:%d\r,d3:%d\r,d4:%d\r\r\r",flag.TX_Addr[0],flag.TX_Addr[1],flag.  
		}
		
		if(_50hz) {
			_50hz = 0;
			LoadRCdata(AMERICAN_RC_MODE);
			
		}
		
		
		if(_100hz) {
			_100hz = 0;
			rx_send();
			/*
			if(flag.modifyaddr) 
				modifyAddress();
			else {
				LedSet(5,0);
				rx_send();
			}
			*/
		}
	

	}
	
}

