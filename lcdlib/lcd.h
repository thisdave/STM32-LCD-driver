#ifndef LCD_H_
#define LCD_H_

#include <stm32_lib/inc/stm32f0xx_gpio.h>
#include <cmsis_boot/stm32f0xx.h>

#ifdef JHONNY
	#define KISKUTYA
#else
	#define	KISKUTYA extern
#endif

KISKUTYA volatile uint32_t TickValue;


//Idoegyseg keslelteteshez:
#define COIN 0.04

#define RS GPIO_Pin_13 /* RS is named as PC13 */
#define RW GPIO_Pin_14 /* RW is named as PC14 */
#define EN GPIO_Pin_15 /* EN is named as PC15 */

#define SET_EN()   GPIOC->BSRR=EN
#define CLEAR_EN()   GPIOC->BRR=EN
#define SET_RS2()   GPIOC->BSRR=RS
#define CLEAR_RS()   GPIOC->BRR=RS
#define SET_RW()   GPIOC->BSRR=RW
#define CLEAR_RW()   GPIOC->BRR=RW

#define delay1per3us()			\
	{   volatile uint32_t i;	\
		for(i=0;i<1;i++);		\
	}

#define delay_5us()				\
{	volatile uint32_t i;		\
	for(i=0;i<14;i++);			\
}
#define delay_us_100()			\
{								\
	volatile uint32_t i;		\
		for(i=0;i<14*20;i++);	\
}



/* SysTick interrupt each 1 MHz with HCLK equal to 48MHz*/ 
/* - 30 to compensate the overhead of this sub routine	*/
#define delay_ms(n_ms)           								\
{																\
	if (SysTick_Config (SystemCoreClock/1000 - 30)==1)			\
	{															\
	}															\
																\
	TickValue = (n_ms);											\
	while(TickValue == (n_ms));									\
	while(TickValue != 0)										\
		{;}														\
}

#define s_latch() \
{	delay_ms(10);	\
	GPIOC->BSRR=EN;	\
	delay_ms(10);	\
	GPIOC->BRR=EN;	\
	delay_ms(10);	\
}


#define shup12(input) ((input) << 12)

#define LCD_DATA(input) GPIOB->ODR = shup12( (input) )  //LCD DATA register

void SysTick_Handler(void);

void LCD_init();
void LCDstring(const char *msg);
void LCDint(int val, unsigned int field_length);
void LCDpos(uint8_t x,uint8_t y);
void LCDshow(char *a,char *b);
void LCDclear();

#endif /* LCD_H_ */
