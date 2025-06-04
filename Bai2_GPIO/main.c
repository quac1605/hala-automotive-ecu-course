#include "stm32f10x_gpio.h"             // Device:StdPeriph Drivers:GPIO
#include "delay.h"

void RCC_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}	



int main(){
	RCC_Config();
	TIM_Config(TIM2);
	GPIO_Config();
	while(1){
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){
//			GPIO_SetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 10000);
//			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 10000);
//		}  
//		else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//	}
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
			delay_ms(TIM2, 10000);
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			delay_ms(TIM2, 10000);
	}
}
