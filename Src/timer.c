#include "stm32f10x_it.h"
#include "timer.h"

u16 _2hz,_20hz,_50hz,_100hz,_200hz;
u16 flag2Hzcnt,flag20Hzcnt,flag50Hzcnt,flag100Hzcnt,flag200Hzcnt;

void TIM4_IRQHandler(void)		//1ms中断一次,用于遥控器主循环
{
    if( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	{     
		if(++flag2Hzcnt == 500)//2hz
		{
			flag2Hzcnt = 0;
			_2hz = 1;
		}
		
		if(++flag20Hzcnt == 50)//20Hz
		{
			flag20Hzcnt = 0;
			_20hz = 1;
		} 

		if(++flag50Hzcnt == 20)//50Hz
		{
			flag50Hzcnt = 0;
			_50hz = 1;
		}   
		if(++flag100Hzcnt == 10) //100Hz
		{
			flag100Hzcnt = 0;
			_100hz = 1;
		}
		if(++flag200Hzcnt == 5) //200Hz
		{
			flag200Hzcnt = 0;
			_200hz = 1;
		}	
		
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update); //清除中断标志   
    }
	
}


//定时器4初始化
void TIM4_Init(char clock,int Preiod)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//打开时钟
    
    TIM_DeInit(TIM4);

    TIM_TimeBaseStructure.TIM_Period = Preiod;
    TIM_TimeBaseStructure.TIM_Prescaler = clock-1;//定时1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM4,ENABLE);
	
}	

void TimerNVIC_Configuration()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* NVIC_PriorityGroup 2 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//定时器4优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

} 

