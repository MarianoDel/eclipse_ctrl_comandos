/**
  ******************************************************************************
  * @file    Template_2/stm32f0_gpio.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   GPIO functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure global inputs/outputs.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "hard.h"
#include "stm32f0xx.h"

//#include "stm32f0xx_exti.h"


//--- VARIABLES EXTERNAS ---//
//volatile unsigned char led;


//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
	unsigned long temp;

	//--- MODER ---//
	//00: Input mode (reset state)
	//01: General purpose output mode
	//10: Alternate function mode
	//11: Analog mode

	//--- OTYPER ---//
	//These bits are written by software to configure the I/O output type.
	//0: Output push-pull (reset state)
	//1: Output open-drain

	//--- ORSPEEDR ---//
	//These bits are written by software to configure the I/O output speed.
	//x0: Low speed.
	//01: Medium speed.
	//11: High speed.
	//Note: Refer to the device datasheet for the frequency.

	//--- PUPDR ---//
	//These bits are written by software to configure the I/O pull-up or pull-down
	//00: No pull-up, pull-down
	//01: Pull-up
	//10: Pull-down
	//11: Reserved

#ifdef VER_1_0
#ifdef GPIOA_ENABLE

	//--- GPIO A ---//
	if (!GPIOA_CLK)
		GPIOA_CLK_ON;

	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0xFFFFC030;		//PA0 input; PA1 out; PA3 out; PA4 input; PA5 input; PA6 out
	temp |= 0x00001044;
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFFFF33;
	temp |= 0x00000000;		//PA1 PA3 low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0xFFFFFCFC;		//PA0 PA4 pull up
	temp |= 0x00000101;
	GPIOA->PUPDR = temp;


#endif

#ifdef GPIOB_ENABLE

	//--- GPIO B ---//
	if (!GPIOB_CLK)
		GPIOB_CLK_ON;

	temp = GPIOB->MODER;	//2 bits por pin
	temp &= 0xFFFFFFF3;		//PB1 (analog input)
	temp |= 0x0000000C;
	GPIOB->MODER = temp;

	temp = GPIOB->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOB->OTYPER = temp;

	temp = GPIOB->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOB->OSPEEDR = temp;

	temp = GPIOB->PUPDR;	//2 bits por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOB->PUPDR = temp;


#endif
#endif


#ifdef GPIOF_ENABLE

	//--- GPIO F ---//
	if (!GPIOF_CLK)
		GPIOF_CLK_ON;

	temp = GPIOF->MODER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->MODER = temp;

	temp = GPIOF->OTYPER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OTYPER = temp;

	temp = GPIOF->OSPEEDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OSPEEDR = temp;

	temp = GPIOF->PUPDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->PUPDR = temp;

#endif

#ifdef WITH_EXTI
	//Interrupt en PB8
	if (!SYSCFG_CLK)
		SYSCFG_CLK_ON;

	SYSCFG->EXTICR[0] = 0x00000000; //Select Port A & Pin 0 external interrupt
	SYSCFG->EXTICR[1] = 0x00000000; //Select Port A & Pin 4 external interrupt
	EXTI->IMR |= 0x0011; 			//Corresponding mask bit for interrupts PA4 & PA0
	EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
	EXTI->RTSR |= 0x0000; 			//Pin Interrupt line on rising edge
	EXTI->FTSR |= 0x0011; 			//Pin Interrupt line on falling edge PA4 & PA0

	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn, 6);

	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 6);

#endif

}

#ifdef WITH_EXTI
void EXTI0_1_IRQHandler(void)
{
	if(EXTI->PR & 0x0001)	//Line0
	{
		EXTI->PR |= 0x0001;
		EXTIOff();
		//led = 1;
	}
}

void EXTI4_15_IRQHandler(void)
{
	if(EXTI->PR & 0x0010)	//Line4
	{
		EXTI->PR |= 0x0010;
		EXTIOff();
		//led = 2;
	}
}
#endif

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000011;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000011;
}

void Gpio5PullUpOn (void)
{
	unsigned long temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0xFFFFF3FF;		//PA5 pull up
	temp |= 0x00000400;
	GPIOA->PUPDR = temp;
}

void Gpio5PullUpOff (void)
{
//	unsigned long temp;
//	temp = GPIOA->PUPDR;	//2 bits por pin
//	temp &= 0xFFFFCFFF;		//PA6 not pull up
//	temp |= 0x00000000;
//	GPIOA->PUPDR = temp;
	GPIOA->PUPDR &= 0xFFFFF3FF;
}

//--- end of file ---//
