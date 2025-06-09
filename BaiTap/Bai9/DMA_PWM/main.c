#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA

#define SPI1_NSS 	GPIO_Pin_4
#define SPI1_SCK	GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7
#define SPI1_GPIO GPIOA


uint16_t buffer[1] = {0};

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// SPI Slave GPIO Configuration - CRITICAL FIX
	// NSS, SCK, MOSI as inputs for slave mode
	GPIO_InitStructure.GPIO_Pin = SPI1_NSS | SPI1_SCK | SPI1_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // Must be INPUT for slave
	GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
	
	// MISO as alternate function output for slave mode
	GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Output for slave
	GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
	
	// PWM output for servo (PA0 - TIM2_CH1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM_Config(){
  TIM_TimeBaseInitTypeDef TIM_InitStruct;
  
  // Timer base configuration for 50Hz (20ms period)
  TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_InitStruct.TIM_Prescaler = 72-1;        // 72MHz/72 = 1MHz (1µs resolution)
  TIM_InitStruct.TIM_Period = 20000-1;        // 20000µs = 20ms = 50Hz
  TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
  TIM_ARRPreloadConfig(TIM2, ENABLE);         // Enable ARR preload
  
  // PWM configuration
  TIM_OCInitTypeDef TIM_OC;
  TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OC.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC.TIM_Pulse = 1500;                    // 1.5ms pulse width (servo center position)
  
  TIM_OC1Init(TIM2, &TIM_OC);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
  TIM_Cmd(TIM2, ENABLE);
}

void DMA_Config(){
  DMA_InitTypeDef DMA_InitStruct;
	DMA_DeInit(DMA1_Channel2);
   
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStruct.DMA_BufferSize = 1;   // So luong byte muon truyen
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buffer;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;	//Data Register
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  
  // Thay doi tu DMA1_Channel2 sang DMA1_Channel3 cho truyen SPI1 TX
  DMA_Init(DMA1_Channel2, &DMA_InitStruct);
  DMA_Cmd(DMA1_Channel2, ENABLE);
   
   
}

void SPI_Config(){
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
}

uint16_t SPI_Receive1Byte(void){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
		uint16_t temp = SPI_I2S_ReceiveData(SPI1);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
    return temp;
}

void SetServoAngle(uint16_t angle_degrees) {
    // Convert angle (0-180°) to pulse width (1000-2000 µs)
		if (angle_degrees > 180) angle_degrees = 180;
    uint16_t pulse_width = 1000 + (angle_degrees * 1000) / 180;
    TIM_SetCompare1(TIM2, pulse_width);
}


uint16_t angle;
int main(void)
{
	RCC_Config();
	GPIO_Config();
	DMA_Config();
	TIM_Config();
	SPI_Config();
  while(1)
  {
//		while(GPIO_ReadInputDataBit(GPIOA, SPI1_NSS) == 1){}
//		if(GPIO_ReadInputDataBit(GPIOA, SPI1_NSS) == 0){
//			data = SPI_Receive1Byte();
//		}
		
		angle = ((uint32_t)buffer[0] * 180) / 4095;
		SetServoAngle(angle);
		
  }
}