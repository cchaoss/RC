#ifndef __ADC_H
#define __ADC_H	
#include "stm32f10x.h"
							  
#define ADC_CH1  		1  			//通道1		 	    
#define ADC_CH_TEMP  	16 			//温度传感器通道


#define AMERICAN_RC_MODE 0
#define ROLL_Channel 	0
#define PITCH_Channel 	1
#define YAW_Channel 	2
#define THRO_Channel 	3

#define Deadband 8
#define Deadband_Thro 30
  		
#define Bound(x,max,min) ((x) > (max) ? (max) : (x) < (min) ? (min) : (x))

void Adc_Init(void); 				//ADC通道初始化
u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道10次采样的平均值 	  
void LoadRCdata(u8 rc_mode_style);	//获取摇杆值

#endif 















