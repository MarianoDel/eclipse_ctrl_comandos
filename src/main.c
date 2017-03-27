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

#include "flash_program.h"

//#include <stdio.h>
//#include <string.h>


/* Externals variables ---------------------------------------------------------*/
volatile unsigned char timer_1seg = 0;
volatile unsigned short wait_ms_var = 0;
parameters_typedef param_struct;
volatile unsigned short timer_tick = 0;


//--- VARIABLES GLOBALES ---//

// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short timer_for_stop;

// ------- de los switches -------
volatile unsigned short switches_timer;
volatile unsigned char s1, s2;

#define TIM_BIP_SHORT	300
#define TIM_BIP_LONG	800
#define TT_TO_FREE_ERROR	5000

#define TT_RXCODE			6000

#define SAVE_S1		1
#define SAVE_S2		2






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
	unsigned short rxcode = 0;
	unsigned char rxbits = 0;
	unsigned short rxlambda = 0;



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

	//cargo las variables desde la memoria
	param_struct.bits_button_one = ((parameters_typedef *) (unsigned int *) PAGE31)->bits_button_one;
	param_struct.code_button_one = ((parameters_typedef *) (unsigned int *) PAGE31)->code_button_one;
	param_struct.lambda_button_one = ((parameters_typedef *) (unsigned int *) PAGE31)->lambda_button_one;

	param_struct.bits_button_two = ((parameters_typedef *) (unsigned int *) PAGE31)->bits_button_two;
	param_struct.code_button_two = ((parameters_typedef *) (unsigned int *) PAGE31)->code_button_two;
	param_struct.lambda_button_two = ((parameters_typedef *) (unsigned int *) PAGE31)->lambda_button_two;

	timer_for_stop = TIMER_SLEEP;

	RandomGen(timer_tick);

	//--- Main loop ---//
	while(1)
	{
		switch (main_state)
		{
			case CHECK_EVENTS:
				if ((CheckS1() >= S_HALF) && (!CheckS2()))
				{
					repeat = REPEAT_CODES;
					main_state = TX_S;

					rxcode = param_struct.code_button_one;
					rxbits = param_struct.bits_button_one;
					rxlambda = param_struct.lambda_button_one;
				}

				if ((CheckS2() >= S_HALF) && (!CheckS1()))
				{
					repeat = REPEAT_CODES;
					main_state = TX_S;

					rxcode = param_struct.code_button_two;
					rxbits = param_struct.bits_button_two;
					rxlambda = param_struct.lambda_button_two;
				}

				if ((CheckS1()) && (CheckS2()))
				{
					timer_standby = 1000;
					main_state = RX_WAIT_BUTTON;
				}

				if (!timer_for_stop)
					main_state = SLEEPING;

				break;

			case TX_S:
				if (repeat)
				{
					repeat--;
					SendCode16Reset();
					main_state = TX_S_A;
				}
				else
				{
					timer_for_stop = TIMER_SLEEP;
					main_state = CHECK_EVENTS;
				}
				break;

			case TX_S_A:
				if ((rxbits != 0) && (rxbits < 0xFF))
					resp = SendCode16(rxcode, rxbits, rxlambda);
					//resp = SendCode16(rxcode, 12);
				else
					resp = SendCode16(0x0555, 12, DEFAULT_LAMBDA);
					//resp = SendCode16(0x0555, 12);

				if (resp != RESP_CONTINUE)
					main_state = TX_S;

				break;

			case RX_WAIT_BUTTON:
				if (!timer_standby)
				{
					if ((CheckS1()) && (CheckS2()))
					{
						LED_ON;
						timer_standby = 6000;
						main_state = RX_WAIT_BUTTON_B;
					}
					else
						main_state = CHECK_EVENTS;
				}
				break;

			case RX_WAIT_BUTTON_B:
				if (!(CheckS1()) && (!CheckS2()))
				{
					LED_OFF;
					timer_standby = 6000;
					repeat = 0;		//en repeat voy a decir el boton a grabar
					main_state = RX_WAIT_BUTTON_C;
				}

				//agoto timer para grabar
				if (!timer_standby)
				{
					main_state = CHECK_EVENTS;
					LED_OFF;
				}
				break;

			case RX_WAIT_BUTTON_C:
				//en repeat voy a decir el boton a grabar
				if (CheckS1())
					repeat = SAVE_S1;

				if (CheckS2())
					repeat = SAVE_S2;

				//blink de led con timer_for_stop
				if (!timer_for_stop)
				{
					if (LED)
						LED_OFF;
					else
						LED_ON;
					timer_for_stop = 150;
				}

				//agoto timer para grabar
				if (!timer_standby)
				{
					main_state = CHECK_EVENTS;
					LED_OFF;
				}

				if (repeat)		//tengo algo que grabar
				{
					LED_ON;
					timer_standby = 1000;
					main_state = RX_S;
				}
				break;

			case RX_S:
				if (!timer_standby)
				{
					LED_OFF;
					RecvCode16Reset();
					main_state = RX_S_A;
					timer_standby = TT_RXCODE;
				}
				break;

			case RX_S_A:
				resp = RecvCode16(&rxbits);

				if (resp == RESP_OK)
				{
					RecvCode16Reset();
					main_state = RX_S_OK;
				}

				if (resp == RESP_NOK)
				{
					RecvCode16Reset();		//cuantas veces????
					main_state = RX_S_NOK;
				}

				if (!timer_standby)
				{
					RecvCode16Reset();
					main_state = RX_S_TO;
				}
				break;

			case RX_S_OK:
				resp = UpdateTransitions (rxbits, &rxcode, &rxlambda);

				if (resp == RESP_OK)
				{
					for (i = 0; i < 6; i++)
					{
						LED_ON;
						Wait_ms(150);
						LED_OFF;
						Wait_ms(150);
					}

					main_state =  SAVE_PARAMS;
				}
				else
				{
					LED_ON;
					Wait_ms(20);
					LED_OFF;

					main_state = RX_S_A;
				}

				break;

			case RX_S_NOK:
//				LED_ON;
//				Wait_ms(20);
//				LED_OFF;

				main_state = RX_S_A;
				break;

			case RX_S_TO:

				LED_ON;
				Wait_ms(250);
				LED_OFF;

				main_state =  CHECK_EVENTS;
				break;

			case SAVE_PARAMS:
				//grabado
				//hago update de memoria y grabo
				if (repeat == SAVE_S1)
				{
					param_struct.bits_button_one = rxbits;
					param_struct.code_button_one = rxcode;
					param_struct.lambda_button_one = rxlambda;
				}

				if (repeat == SAVE_S2)
				{
					param_struct.bits_button_two = rxbits;
					param_struct.code_button_two = rxcode;
					param_struct.lambda_button_two = rxlambda;
				}

				WriteConfigurations ();

				main_state =  CHECK_EVENTS;
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

	timer_tick++;

}





