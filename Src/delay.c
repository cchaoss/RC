/*    
      ____                      _____                  +---+
     / ___\                     / __ \                 | R |
    / /                        / /_/ /                 +---+
   / /   ________  ____  ___  / ____/___  ____  __   __
  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
 / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
 \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
                                                 / /
                                            ____/ /
                                           /_____/
*/
 /* main.c file
编写者：小马  (Camel)
作者E-mail：375836945@qq.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2014-01-28
功能：
提供精确的延时API  有微秒级 和毫秒级延时
------------------------------------
*/
 
 
#include "delay.h"
#include "stm32f10x_it.h"

/**************************实现函数********************************************
*函数原型:		void delay_ms(u16 nms)
*功　　能:		毫秒级延时  延时nms  nms<=1864 
*******************************************************************************/
void delay_ms(uint16_t nms)
{
		uint32_t t0=micros();
		while(micros() - t0 < nms * 1000);
			
}
/**************************实现函数********************************************
*函数原型:		void delay_us(u32 nus)
*功　　能:		微秒级延时  延时nus  nms<=1864 
*******************************************************************************/		    								   
void delay_us(u32 nus)
{
		uint32_t t0=micros();
		while(micros() - t0 < nus);
			
}
