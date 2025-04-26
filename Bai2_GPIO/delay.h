#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Device:StdPeriph Drivers:TIM

void TIM_Config(TIM_TypeDef *TIM_Struct);

void delay_ms(TIM_TypeDef *TIM_Struct,uint16_t time);

#endif
