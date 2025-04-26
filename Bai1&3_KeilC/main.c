//typedef struct
//{
//  unsigned int CRL;
//  unsigned int CRH;
//  unsigned int IDR;
//  unsigned int ODR;
//  unsigned int BSRR;
//  unsigned int BRR;
//  unsigned int LCKR;
//} GPIO_TypeDef;

//typedef struct
//{
//  unsigned int CR;
//  unsigned int CFGR;
//  unsigned int CIR;
//  unsigned int APB2RSTR;
//  unsigned int APB1RSTR;
//  unsigned int AHBENR;
//  unsigned int APB2ENR;
//  unsigned int APB1ENR;
//  unsigned int BDCR;
//  unsigned int CSR;
//} RCC_TypeDef;

//#define GPIOC ((GPIO_TypeDef *)0x40011000)
//#define RCC ((RCC_TypeDef *) 0x40021000)
//#define GPIOA ((GPIO_TypeDef *)0x40010800)

//void delay(volatile unsigned int time){
//	for ( unsigned int i = 0; i < time ; i++);
//}

//void WritePin(GPIO_TypeDef *GPIO_Port, unsigned int Pin, unsigned int state)
//{
//	if(state == 1)
//	GPIO_Port->ODR |= (1 << Pin);
//	else
//	GPIO_Port->ODR &= ~(1 << Pin);
//}

//void GPIO_Config(void){			
//  RCC->APB2ENR |= (1 << 4) | (1 << 2);
//	//PC13
//	GPIOC->CRH &= ~(0xF << 20);
//	GPIOC->CRH |= (0x3 << 20);
//	//PA0
//	GPIOA->CRL |= (1 << 3);
//	GPIOA->CRL &= (7 << 0);
//	GPIOA->ODR |= (1 << 0);
//}


//int main(void) {
//	GPIO_Config();
//	while(1){
//		/*
//		WritePin(GPIOC, 13, 1); 
//		delay(10000000);
//		WritePin(GPIOC, 13, 0);
//		delay(10000000);
//		*/
//		
//		if((GPIOA->IDR & (1 << 0)) == 1){
//			WritePin(GPIOC, 13, 1);			
//		}
//		else WritePin(GPIOC, 13, 0);
//	}
//}


#define TIM2_BASE			0x40000000
#define TIM2_CR1			(*(volatile unsigned int *)(TIM2_BASE +	0x00))
#define TIM2_DIER			(*(volatile unsigned int *)(TIM2_BASE +	0x0C))
#define TIM2_SR				(*(volatile unsigned int *)(TIM2_BASE +	0x10))
#define TIM2_PSC			(*(volatile unsigned int *)(TIM2_BASE +	0x28))
#define TIM2_ARR			(*(volatile unsigned int *)(TIM2_BASE +	0x2C))
#define TIM2_CNT			(*(volatile unsigned int *)(TIM2_BASE +	0x24))
	
#define RCC_BASE			0x40021000
#define RCC_APB1ENR	(*(volatile unsigned int *)(RCC_BASE + 0x1C))
#define RCC_APB2ENR	(*(volatile unsigned int *)(RCC_BASE + 0x18))
	
#define GPIOC_BASE		0x40011000
#define GPIOC_CRH			(*(volatile unsigned int *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR			(*(volatile unsigned int *)(GPIOC_BASE + 0x0C))

#define NVIC_ISER0		(*(volatile unsigned int *)0xE000E100)
#define SCB_VTOR			(*(volatile unsigned int *)0xE000ED08)
#define ADJUST				(*(volatile unsigned int *)0x200000B0)
	
__attribute__((aligned(256))) void (*VectorTable[256])();


	
void TIM2_IRQHandler_Custom(void){
	if (TIM2_SR & 1){ //Thanh ghi cua TIM2_SR, khi Timer tran bao ngat len CPU thi bit cuoi cua thanh len 1
		TIM2_SR &= ~1;
		
		GPIOC_ODR ^= (1 << 13);
	}
}

void vector_table_init(void){
	unsigned int i;
	for (i = 0; i < 256; i++) {
		VectorTable[i] = (void (*)())(*((unsigned int *)(0x08000000 + (i * 4))));
	}
	
	ADJUST = (unsigned int)TIM2_IRQHandler_Custom;//Thay gia tri bien Adjust thanh dia chi cua ham TIM2_IRQHandler_Custom
	
	SCB_VTOR = (unsigned int)VectorTable;//Cap nhat dia chi ve cai VectorTable
	
}

void RCC_Config(){
	RCC_APB2ENR |= (1 << 4);
	RCC_APB1ENR |= 1;
}

void GPIO_Config(){
	GPIOC_CRH &= ~(0xF << 20);
	GPIOC_CRH |= (0x3 << 20);
}

void TIM_Config(){
	
	//so can thiet phai tru cho 1
	
	TIM2_PSC = 7200 - 1; // prescaler (1 xung clock bang 72MHz ==> nhan voi prescaler = 7200 ==> 0,1ms dem 1 lan)
	TIM2_ARR = 10000 - 1; // Dem toi bnh thi tran (Period: 0,1 ms dem 1 lan ==> dem 10000 lan duoc 1s) auto reload
	
	TIM2_DIER |= (1 << 0); // Bat Timer cho Interrupt DMA/Interrupt Enable Register: Khi timer tran se bao ngat
	TIM2_CR1 |= (1 << 0);	// Counter enable
	
	NVIC_ISER0 |= (1 << 28); // Bat de ngat hoat dong 
}

int main(void) {
	vector_table_init();
	GPIO_Config();
	RCC_Config();
	TIM_Config();
	while(1){
		//Do nothing
	}
}