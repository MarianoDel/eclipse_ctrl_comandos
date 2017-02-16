/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "core_cm0.h"
#include "stm32f0x_gpio.h"
#include "stm32f0x_tim.h"
#include "stm32f0xx_it.h"
#include "dsp.h"

//#include <stdio.h>
//#include <string.h>


//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

volatile unsigned short timer_led_comm = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned char seq_ready = 0;


//--- VARIABLES GLOBALES ---//

// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned char filter_timer;
static __IO uint32_t TimingDelay;

volatile unsigned char door_filter;
volatile unsigned char take_sample;
volatile unsigned char move_relay;

volatile unsigned char secs = 0;
volatile unsigned short minutes = 0;

volatile unsigned short timer_led_error = 0;
#define TIM_BIP_SHORT	300
#define TIM_BIP_LONG	800
#define TT_TO_FREE_ERROR	5000






//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
void UpdateSwitches (void);
unsigned char CheckS1 (void);
unsigned char CheckS2 (void);

// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i;

	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();

	//ACTIVAR SYSTICK TIMER
	//if (SysTick_Config(48000))		//del core_cm0.h
	if (SysTick_Config(8000))		//del core_cm0.h
	{
		while (1)	/* Capture error */
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			for (i = 0; i < 255; i++)
			{
				asm (	"nop \n\t"
						"nop \n\t"
						"nop \n\t" );
			}
		}
	}


	//TIM Configuration.
#if ((defined BOOST_WITH_CONTROL) || (defined BOOST_CONVENCIONAL))
	TIM_3_Init();
#endif
#ifdef BUCK_BOOST_WITH_CONTROL
	TIM_1_Init();
	TIM_3_Init();
#endif

	//--- COMIENZO PROGRAMA DE PRODUCCION


	while(1)
	{
//		if (LED)
//		{
//			LED_OFF;
//			TX_CODE_OFF;
//		}
//		else
//		{
//			LED_ON;
//			TX_CODE_ON;
//		}
//
//		Wait_ms(3);
		if (S1)
		{
			TX_CODE_ON;
			LED_ON;
		}
		else
		{
			TX_CODE_OFF;
			LED_OFF;
		}

		if (S2)
		{
			LED_ON;
			for (i = 0; i < 10; i++)
			{
				TX_CODE_ON;
				Wait_ms(1);
				TX_CODE_OFF;
				Wait_ms(1);
			}
			LED_OFF;
			Wait_ms(20);
		}
	}


	//--- Main loop ---//
	while(1)
	{
		//PROGRAMA DE PRODUCCION



	}	//termina while(1)

	return 0;
}


//--- End of Main ---//
//unsigned char CheckS1 (void)	//cada check tiene 10ms
//{
//	if (s1 > SWITCHES_THRESHOLD_FULL)
//		return S_FULL;
//
//	if (s1 > SWITCHES_THRESHOLD_HALF)
//		return S_HALF;
//
//	if (s1 > SWITCHES_THRESHOLD_MIN)
//		return S_MIN;
//
//	return S_NO;
//}
//
//unsigned char CheckS2 (void)	//cada check tiene 10ms
//{
//	if (s1 > SWITCHES_THRESHOLD_FULL)
//		return S_FULL;
//
//	if (s1 > SWITCHES_THRESHOLD_HALF)
//		return S_HALF;
//
//	if (s1 > SWITCHES_THRESHOLD_MIN)
//		return S_MIN;
//
//	return S_NO;
//}
//
//void UpdateSwitches (void)
//{
//	//revisa los switches cada 10ms
//	if (!switches_timer)
//	{
//		if (S1_INPUT)
//		{
//			if (s1 < SWITCHES_ROOF)
//				s1++;
//		}
//		else if (s1 > 50)
//			s1 -= 50;
//		else if (s1 > 10)
//			s1 -= 5;
//		else if (s1)
//			s1--;
//
//		switches_timer = SWITCHES_TIMER_RELOAD;
//	}
//}



void EXTI4_15_IRQHandler(void)		//nueva detecta el primer 0 en usart Consola PHILIPS
{


	if(EXTI->PR & 0x0100)	//Line8
	{
		EXTI->PR |= 0x0100;
	}
}


void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}

	if (wait_ms_var)
		wait_ms_var--;

	if (timer_standby)
		timer_standby--;

	if (take_sample)
		take_sample--;

	if (filter_timer)
		filter_timer--;

	if (timer_led_error)
		timer_led_error--;
	/*
	//cuenta 1 segundo
	if (button_timer_internal)
		button_timer_internal--;
	else
	{
		if (button_timer)
		{
			button_timer--;
			button_timer_internal = 1000;
		}
	}
	*/
}





