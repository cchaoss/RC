#include "math.h"
#include "stick.h"
#include "delay.h"
#include "stm32f10x_adc.h"
#include "protocol.h"

//初始化ADC
//这里我们仅以规则通道为例
//开启ADC1的通道0 1 2 3 对应 PA0 PA1 PA2	PA3		
//Ch0---->PA0
//Ch1---->PA1
//Ch2---->PA2
//Ch3---->PA3
void  Adc_Init(void)
{ 
	
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1, ENABLE );
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//最大不能超过14M
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	ADC_DeInit(ADC1);  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);   

	ADC_Cmd(ADC1, ENABLE);
	delay_us(100);//GD32移植需要在使能之后立即加100us左右延时
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}				  

/*
获取通道ch的转换值，取times次,然后平均 
ch:通道编号
times:获取次数
返回值:通道ch的times次转换结果平均值
*/
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u8 i = 0;
	u32 temp_val = 0;
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_71Cycles5);//ADC1,ADC通道ch,规则采样顺序值序列为1,采样时间为239.5周期	  			    
	for(i=0;i<times;i++) {
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件启动转换启动功能
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//等待转换结束
		ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//清除转换标志位，GD32必须手动清除
		temp_val += ADC_GetConversionValue(ADC1);
	}
	return temp_val/times;//返回平均值
}	 


/*
摇杆数据采集
输入参数为美国手(0)和日本手(1)
*/
void LoadRCdata(u8 rc_mode_style)
{
	if(rc_mode_style == AMERICAN_RC_MODE)
	{
		flag.Stick_Data[THRO]= 1000 - Get_Adc_Average(THRO_Channel,5)/4.096;	
		flag.Stick_Data[PITCH]= Get_Adc_Average(PITCH_Channel,5)/4.096 - 30;
	}
	else 
	{
		flag.Stick_Data[THRO]= Get_Adc_Average(PITCH_Channel,5)/4.096;	
		flag.Stick_Data[PITCH]= 1030 - Get_Adc_Average(THRO_Channel,5)/4.096;
	}
	flag.Stick_Data[YAW] = Get_Adc_Average(YAW_Channel,5)/4.096;
	flag.Stick_Data[ROLL] = Get_Adc_Average(ROLL_Channel,5)/4.096 + 10;	
	
	//下面是对摇杆数据限制大小、设置死区
	flag.Stick_Data[ROLL] = Bound((fabs(flag.Stick_Data[ROLL]-500)>Deadband?flag.Stick_Data[ROLL]:500),900,100);
	flag.Stick_Data[PITCH]= Bound((fabs(flag.Stick_Data[PITCH]-500)>Deadband?flag.Stick_Data[PITCH]:500),900,100);
	flag.Stick_Data[YAW]  = Bound((fabs(flag.Stick_Data[YAW]-500)>Deadband?flag.Stick_Data[YAW]:500),900,100);
	flag.Stick_Data[THRO] = Bound((fabs(flag.Stick_Data[THRO]-500)>Deadband_Thro?flag.Stick_Data[THRO]:500),1000,0);

	
	remoteData.roll_l = flag.Stick_Data[ROLL];
	remoteData.roll_h = flag.Stick_Data[ROLL]>>8;
	remoteData.pitch_l = flag.Stick_Data[PITCH];
	remoteData.pitch_h = flag.Stick_Data[PITCH]>>8;
	remoteData.yaw_l = flag.Stick_Data[YAW];
	remoteData.yaw_h = flag.Stick_Data[YAW]>>8;
	remoteData.thro_l = flag.Stick_Data[THRO];
	remoteData.thro_h = flag.Stick_Data[THRO]>>8;

}
