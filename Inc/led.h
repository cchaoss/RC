#ifndef _Led_H_
#define _Led_H_
#include "stm32f10x.h"


enum led_number
{
	led1 = 1,
	led2,
	led3,
	led4,
	led5,//左上那个灯
};

void LedInit(void);
void LedSet(u8 ch,char sta);//LED点亮关闭



#endif

