#ifndef _TIMER_H_
#define _TIMER_H_
#include "stm32f10x.h"

#define TIME4_Preiod  1000

void TIM4_Init(char clock,int Preiod);//用于监测系统
void TimerNVIC_Configuration(void);//定时器中断向量表配置

extern u16 _2hz,_20hz,_50hz,_100hz,_200hz;

#endif
