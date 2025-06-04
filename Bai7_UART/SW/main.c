#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

#define TX_Pin	GPIO_Pin_9
#define RX_Pin	GPIO_Pin_10
#define UART_GPIO	GPIOA

#define bit_duration 104		//104us

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}


void GPIO_Config(){
	GPIO_InitTypeDef GPIOInitStruct;
	
	//Rx
	GPIOInitStruct.GPIO_Pin = RX_Pin;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART_GPIO, &GPIOInitStruct);
	
	//Tx
	GPIOInitStruct.GPIO_Pin = TX_Pin;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(UART_GPIO, &GPIOInitStruct);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	// Count up 1 every 1us
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//72MHz
	TIM_InitStruct.TIM_Prescaler = 72 - 1;			//1us count++	
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);	//Cho phep timer hoat dong
}

void delay_us(uint32_t timedelay)
{
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) < timedelay){}
}
//delay seconds
void delay(uint32_t time){
	for(int i = 0; i < time*1000; i++){
		delay_us(1000);
	}
}

void clock(){
	delay_us(bit_duration);
}

void UARTSoftware_Init(){
	GPIO_SetBits(UART_GPIO, TX_Pin);
	delay_us(1);
}

void UARTSoftware_Transmitt(char c) {
    // Start bit
    GPIO_ResetBits(GPIOA, TX_Pin);
    clock();

    // Truyen cac bit du lieu (LSB truoc)
    for (int i = 0; i < 8; i++) {
        if (c & (1 << i)) {
            GPIO_SetBits(GPIOA, TX_Pin);
        } else {
            GPIO_ResetBits(GPIOA, TX_Pin);
        }
        clock();
    }
		
    // Stop bit
    GPIO_SetBits(GPIOA, TX_Pin);
		clock();
}

char UARTSoftware_Receive() {
    char c = 0;

    // Doi Start bit
    while (GPIO_ReadInputDataBit(GPIOA, RX_Pin) == 1);

    // Cho mot nua thoi gian bit di vao giua start bit
    delay_us(bit_duration + bit_duration / 2);

    // Doc cac bit du lieu (LSB tru?c)
    for (int i = 0; i < 8; i++) {
				
        if (GPIO_ReadInputDataBit(GPIOA, RX_Pin)) {
            c |= (1 << i);
        }
				clock(); // Doi d?n gi?a bit ti?p theo
    }

    // Doi Stop bit
		while (GPIO_ReadInputDataBit(GPIOA, RX_Pin) == 0){};
    delay_us(bit_duration / 2);

    return c;
}

char data[4] = {'D', 'A', 'N', 'H'};

int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	UARTSoftware_Init();
	while(1){
		for(int i = 0; i < 4; i++){
			UARTSoftware_Transmitt(data[i]);
			delay(1);
		}
		UARTSoftware_Transmitt(UARTSoftware_Receive());
	}
}