#include "delay.h"

void TIM_Config(TIM_TypeDef *TIM_Struct){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Prescaler = 7200 -1;  	//0.1ms ++
	TIM_InitStruct.TIM_Period = 0xFFFF;				//No overflow
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM_Struct, &TIM_InitStruct);
	TIM_Cmd(TIM_Struct, ENABLE);
}

void delay_ms(TIM_TypeDef *TIM_Struct,uint16_t time){
	TIM_SetCounter(TIM_Struct, 0);
	while(TIM_GetCounter(TIM_Struct) < time * 10);
}
