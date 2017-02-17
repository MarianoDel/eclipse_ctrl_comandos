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
				send_state = C_SEND_PILOT;
				TIM16->CNT = 0;
				TIM16Enable();
				LED_ON;
				TX_CODE_ON;
			}
			break;

		case C_SEND_PILOT:
			if (TIM16->CNT > (lambda))
			{
				TIM16->CNT = 0;
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
			if (TIM16->CNT > (lambda))
			{
				TIM16->CNT = 0;
				LED_ON;
				TX_CODE_ON;
				send_state = C_SEND_ZERO_C;
			}
			break;

		case C_SEND_ZERO_C:
			if (TIM16->CNT > (2*lambda))
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

