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
#include "gpio.h"
#include "tim.h"
#include "stm32f0xx_it.h"
#include "dsp.h"
#include "pwr.h"
#include "codes.h"

//#include <stdio.h>
//#include <string.h>


//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

volatile unsigned short timer_led_comm = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned char seq_ready = 0;

//para pruebas int
//volatile unsigned char led = 0;

//--- VARIABLES GLOBALES ---//

// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned char filter_timer;
volatile unsigned short timer_for_stop;

// ------- de los switches -------
volatile unsigned short switches_timer;
volatile unsigned char s1, s2;

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



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i;
	RspMessages resp = RESP_CONTINUE;
	MainStates main_state = CHECK_EVENTS;
	unsigned char repeat;

	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();
	EXTIOff();

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
	TIM_16_Init();

	//habilito power modes
	PwrInit ();	//se va a stop con o sin esto

	//--- COMIENZO PROGRAMA DE PRODUCCION
	for (i = 0; i < 6; i++)
	{
		if (LED)
			LED_OFF;
		else
			LED_ON;

		Wait_ms(300);
	}

	timer_for_stop = TIMER_SLEEP;

	//--- Main loop ---//
	while(1)
	{
		switch (main_state)
		{
			case CHECK_EVENTS:
				if (CheckS1())
				{
					repeat = REPEAT_CODES;
					main_state = TX_S1;
				}

				if (CheckS2())
				{
					repeat = REPEAT_CODES;
					main_state = TX_S2;
				}

				if (!timer_for_stop)
					main_state = SLEEPING;

				break;

			case TX_S1:
				if (repeat)
				{
					repeat--;
					SendCode16Reset();
					main_state = TX_S1_A;
				}
				else
				{
					timer_for_stop = TIMER_SLEEP;
					main_state = CHECK_EVENTS;
				}
				break;

			case TX_S1_A:
				resp = SendCode16(0x0550, 12);

				if (resp != RESP_CONTINUE)
					main_state = TX_S1;

				break;

			case TX_S2:
				if (repeat)
				{
					repeat--;
					SendCode16Reset();
					main_state = TX_S2_A;
				}
				else
				{
					timer_for_stop = TIMER_SLEEP;
					main_state = CHECK_EVENTS;
				}
				break;

			case TX_S2_A:
				resp = SendCode16(0x0551, 12);

				if (resp != RESP_CONTINUE)
					main_state = TX_S2;

				break;

			case SLEEPING:
				if (!timer_for_stop)
				{
					EXTIOn();

					if (GPIOA_CLK)
						GPIOA_CLK_OFF;

					//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);		//0.04mA
					PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);		//0.02mA

					if (!GPIOA_CLK)
						GPIOA_CLK_ON;

					timer_for_stop = TIMER_SLEEP;
				}
				else
					main_state = CHECK_EVENTS;

				break;

			default:
				main_state = CHECK_EVENTS;
				break;
		}

		UpdateSwitches ();

	}	//termina while(1)

	return 0;
}


//--- End of Main ---//
unsigned char CheckS1 (void)	//cada check tiene 10ms
{
	if (s1 > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (s1 > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (s1 > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}

unsigned char CheckS2 (void)	//cada check tiene 10ms
{
	if (s2 > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (s2 > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (s2 > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}


//revisa los switches cada 10ms
void UpdateSwitches (void)
{
	if (!switches_timer)
	{
		if (S1)
		{
			if (s1 < SWITCHES_ROOF)
				s1++;
		}
		else if (s1 > 50)
			s1 -= 50;
		else if (s1 > 10)
			s1 -= 5;
		else if (s1)
			s1--;

		if (S2)
		{
			if (s2 < SWITCHES_ROOF)
				s2++;
		}
		else if (s2 > 50)
			s2 -= 50;
		else if (s2 > 10)
			s2 -= 5;
		else if (s2)
			s2--;

		switches_timer = SWITCHES_TIMER_RELOAD;
	}
}





void TimingDelay_Decrement(void)
{
	if (wait_ms_var)
		wait_ms_var--;

	if (timer_standby)
		timer_standby--;

	if (switches_timer)
		switches_timer--;

	if (timer_for_stop)
		timer_for_stop--;

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





