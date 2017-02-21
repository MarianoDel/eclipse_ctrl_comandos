/*
 * codes.c
 *
 *  Created on: 17/02/2017
 *      Author: Mariano
 */

#include "codes.h"
#include "hard.h"
#include "tim.h"
#include "stm32f0xx.h"



//--- External Variables -----

//--- Global Variables -------
unsigned char send_state = 0;
unsigned char recv_state = 0;
unsigned short bitmask;
unsigned short lambda;

//--- Module Functions -------

//Envia el codigo de hasta 2 bytes (16bits), c es codigo, bits a enviar
//contesta RESP_CONTINUE si falta o RESP_OK si termino RESP_NOK en error
unsigned char SendCode16 (unsigned short code, unsigned char bits)
{
	RspMessages resp = RESP_CONTINUE;


	switch (send_state)
	{
		case C_INIT:
			if ((bits > 16) || (bits == 0))
				resp = RESP_NOK;
			else
			{
				bits = bits - 1;	//quito offset
				bitmask = 1;
				bitmask <<= bits;

				lambda = DEFAULT_LAMBDA;
				send_state = C_SEND_PILOT_A;
				TIM16->CNT = 0;
				TIM16Enable();
				LED_OFF;
				TX_CODE_OFF;
			}
			break;

		case C_SEND_PILOT_A:
			if (TIM16->CNT > (36*lambda))
			{
				TIM16->CNT = 0;
				LED_ON;
				TX_CODE_ON;			//bit de pilot
				send_state = C_SEND_PILOT_B;
			}
			break;

		case C_SEND_PILOT_B:
			//if (TIM16->CNT > (lambda))
			if (TIM16->CNT > 900)		//el algoritmo es mas corto cuando entra por S1 q cuando entra por S2
			{
				//TIM16->CNT = 0;
				LED_OFF;
				TX_CODE_OFF;			//apago, siempre despues de pilot
				send_state = C_SENDING;
			}
			break;

		case C_SENDING:
			if (bitmask)
			{
				if (code & bitmask)
					send_state = C_SEND_ONE_A;
				else
					send_state = C_SEND_ZERO_A;

				bitmask >>= 1;
			}
			else
			{
				TIM16Disable();
				resp = RESP_OK;		//termine de enviar
			}
			break;

		case C_SEND_ONE_A:
			LED_OFF;
			TX_CODE_OFF;
			TIM16->CNT = 0;
			send_state = C_SEND_ONE_B;
			break;

		case C_SEND_ONE_B:
			if (TIM16->CNT > (2*lambda))
			{
				TIM16->CNT = 0;
				LED_ON;
				TX_CODE_ON;
				send_state = C_SEND_ONE_C;
			}
			break;

		case C_SEND_ONE_C:
			if (TIM16->CNT > (lambda))
			{
				TIM16->CNT = 0;
				LED_OFF;
				TX_CODE_OFF;
				send_state = C_SENDING;
			}
			break;

		case C_SEND_ZERO_A:
			LED_OFF;
			TX_CODE_OFF;
			TIM16->CNT = 0;
			send_state = C_SEND_ZERO_B;
			break;

		case C_SEND_ZERO_B:
			//if (TIM16->CNT > (lambda))
			if (TIM16->CNT > (360))
			{
				TIM16->CNT = 0;
				LED_ON;
				TX_CODE_ON;
				send_state = C_SEND_ZERO_C;
			}
			break;

		case C_SEND_ZERO_C:
			//if (TIM16->CNT > (2*lambda))
			if (TIM16->CNT > (1280))
			{
				TIM16->CNT = 0;
				LED_OFF;
				TX_CODE_OFF;
				send_state = C_SENDING;
			}
			break;

		default:
			send_state = C_INIT;
			break;
	}
	return resp;
}

//resetea la SM de SendCode
inline void SendCode16Reset (void)
{
	send_state = C_INIT;
}

//resetea la SM de RecvCode
inline void RecvCode16Reset (void)
{
	recv_state = C_INIT;
}

//Recibe el codigo de hasta 2 bytes (16bits), c es codigo, bits a enviar
//contesta RESP_CONTINUE si falta o RESP_OK si termino RESP_NOK en error
unsigned char RecvCode16 (unsigned short * code, unsigned char * bits)
{
	RspMessages resp = RESP_CONTINUE;


	switch (recv_state)
	{
		case C_RXINIT:
			recv_state = C_RXWAIT_PILOT_A;
			TIM16->CNT = 0;
			TIM16Enable();
			RX_CODE_PLLUP_ON;
			RX_EN_ON;
			break;

		case C_RXWAIT_PILOT_A:
			break;


		default:
			send_state = C_RXINIT;
			break;
	}
	return resp;
}
