#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "kalman.h"

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//72MHz
	TIM_InitStruct.TIM_Prescaler = 7200 - 1;					
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);
}

void delay_ms(uint32_t timedelay)
{
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) < timedelay * 10){}
}

void ADC_Config(){
	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

//	NVIC_InitTypeDef nvic;
//	nvic.NVIC_IRQChannel = ADC1_2_IRQn;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
//	nvic.NVIC_IRQChannelSubPriority = 0x00;
//	
//	NVIC_Init(&nvic);
}
uint16_t val;
uint16_t sum = 0;
uint16_t final = 0;

//void ADC1_2_IRQHandler(void){

//	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET){
//		val = ADC_GetConversionValue(ADC1);
//		final = updateEstimate(val);
//		ADC_ClearFlag(ADC1, ADC_IT_EOC);
//	}
//}
int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	ADC_Config();
	SimpleKalmanFilter(0.05, 0.01, 0.1); 
	while(1){
		val = ADC_GetConversionValue(ADC1);
		final = updateEstimate(val);
	}
}  