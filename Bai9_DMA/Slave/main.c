#include "main.h"

TIM_OCInitTypeDef

void SPI_Config(void) {
    // C?p clock cho GPIOA và SPI1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    // C?u hình GPIO cho SPI1 (PA5: SCK, PA6: MISO, PA7: MOSI)
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Ch?n ch?c nang SPI cho các chân
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);  // SCK
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);  // MISO
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);  // MOSI
    
    // C?u hình SPI1
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // Giao ti?p hai chi?u
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       // Ch? d? Master
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   // D? li?u 8-bit
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          // Clock polarity th?p
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        // Clock phase ? c?nh th? nh?t
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           // NSS di?u khi?n b?ng ph?n m?m
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // T?c d? truy?n
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  // Truy?n MSB tru?c
    SPI_Init(SPI1, &SPI_InitStructure);

    // Kích ho?t SPI1
    SPI_Cmd(SPI1, ENABLE);
}

void SPI_Send1Data(uint8_t data) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    // Ð?i cho d?n khi b? d?m truy?n (TXE) tr?ng
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    // G?i byte qua SPI
    SPI_I2S_SendData(SPI1, data);

    // Ð?i cho d?n khi quá trình truy?n k?t thúc (Truy?n xong c? byte)
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void delay(uint32_t time){
	for(int i = 0; i < time; i++){}
}

uint8_t dataToSend[] = "Hello from STM32";

int main(void) {
    // C?u hình SPI
    SPI_Config();

    // D? li?u c?n truy?n
    
    // Truy?n t?ng byte qua SPI
    

    while (1) {
			for (int i = 0; i < sizeof(dataToSend) - 1; i++) {
        SPI_Send1Data(dataToSend[i]);
				delay(1000000);
    }
    }
}

