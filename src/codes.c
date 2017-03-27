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

unsigned short bits_t [SIZEOF_BUFF_TRANS];
unsigned char bits_c;

//unsigned char last_c;

//--- Module Functions -------

//Envia el codigo de hasta 2 bytes (16bits), c es codigo, bits a enviar
//contesta RESP_CONTINUE si falta o RESP_OK si termino RESP_NOK en error
unsigned char SendCode16 (unsigned short code, unsigned char bits, unsigned short def_lambda)
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

				if (!def_lambda)
					resp = RESP_NOK;
				else
					lambda = def_lambda;

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
			if (TIM16->CNT > (lambda))
			//if (TIM16->CNT > (360))
			{
				TIM16->CNT = 0;
				LED_ON;
				TX_CODE_ON;
				send_state = C_SEND_ZERO_C;
			}
			break;

		case C_SEND_ZERO_C:
			if (TIM16->CNT > (2*lambda))
			//if (TIM16->CNT > (1280))
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
	RX_CODE_PLLUP_OFF;
	RX_EN_OFF;
	TIM16Disable();

	recv_state = C_RXINIT;
}

//Recibe puntero a cantidad de bits; por ahora sale ok solo con 12 BITS
//contesta RESP_CONTINUE si falta o RESP_OK si termino RESP_NOK en error
//contesta RESP_TIMEOUT si nunca le llego nada
unsigned char RecvCode16 (unsigned char * bits)
{
	RspMessages resp = RESP_CONTINUE;

	switch (recv_state)
	{
		case C_RXINIT:
			recv_state = C_RXINIT_PULLUP;
			TIM16->CNT = 0;
			TIM16Enable();
			RX_CODE_PLLUP_ON;
			RX_EN_ON;
			bits_c = 0;
			break;

		case C_RXINIT_PULLUP:
			//espera transciciones
			if (TIM16->CNT > 200)	//200us de espera para activacion pullup
			{
				recv_state = C_RXWAIT_PILOT_A;
			}
			break;

		case C_RXWAIT_PILOT_A:
			//espera transciciones

			if (!RX_CODE)	//tengo transicion inicio pilot
			{
				recv_state = C_RXWAIT_PILOT_B;
				TIM16->CNT = 0;
				bits_c = 0;
			}
			break;

		case C_RXWAIT_PILOT_B:
			if (TIM16->CNT > 3000)	//pasaron 3mseg sin nada
				recv_state = C_RXERROR;

			if (RX_CODE)	//tengo pilot y primera transicion bits
			{
				bits_t[bits_c] = TIM16->CNT;
				TIM16->CNT = 0;
				bits_c++;
				recv_state = C_RXWAIT_BITS_B;	//salto la primera transicion
			}
			break;

		case C_RXWAIT_BITS_B:
			//segunda transcicion de bit
			if (TIM16->CNT > 3000)	//pasaron 3mseg sin nada, puede ser el final del codigo
			{
				bits_c -= 1;	//ajusto pilot
				bits_c >>= 1;	//2 transciciones 1 bit
				if (bits_c == 12)
					recv_state = C_RXOK;
				else
					recv_state = C_RXERROR;

			}

			if (!RX_CODE)	//tengo segunda transcicion bit
			{
				bits_t[bits_c] = TIM16->CNT;
				TIM16->CNT = 0;

				if (bits_c < SIZEOF_BUFF_TRANS)
				{
					bits_c++;
					recv_state = C_RXWAIT_BITS_C;
				}
				else
					recv_state = C_RXERROR;

			}
			break;

		case C_RXWAIT_BITS_C:
			//tercera transcicion de bit, primera del proximo
			if (TIM16->CNT > 3000)	//pasaron 3mseg sin nada
				recv_state = C_RXERROR;

			if (RX_CODE)	//tengo segunda transcicion bit y primera del proximo
			{
				bits_t[bits_c] = TIM16->CNT;
				TIM16->CNT = 0;

				if (bits_c < SIZEOF_BUFF_TRANS)
				{
					bits_c++;
					recv_state = C_RXWAIT_BITS_B;
				}
				else
					recv_state = C_RXERROR;

			}
			break;

		case C_RXERROR:
			//termine recepcion con error
			resp = RESP_NOK;
			*bits = 0;
			break;

		case C_RXOK:
			resp = RESP_OK;
			*bits = bits_c;
			break;

		default:
			recv_state = C_RXINIT;
			break;
	}
	return resp;
}

//Recibe cantidad de bits
//contesta con punteros a codigo, lambda rx
//contesta con RESP_OK o RESP_NOK cuando valida el codigo
unsigned char UpdateTransitions (unsigned char bits, unsigned short * rxcode, unsigned short * lambda)
{
	RspMessages resp = RESP_OK;
	unsigned char i;
	unsigned char transitions;
	unsigned char tot_lambda;
	unsigned int tot_time = 0;
	unsigned short lambda15;

	*rxcode = 0;
	//tengo 2 transiciones por bit
	transitions = bits * 2;
	for (i = 0; i < transitions; i++)
		tot_time += bits_t[i + 1];		//todas las transciones sin el pilot

	tot_lambda = bits * 3;
	*lambda = tot_time / tot_lambda;
	lambda15 = *lambda * 3;
	lambda15 >>= 1;

	//compenso offset de bits
	bits -= 1;
	for (i = 0; i < transitions; i += 2)
	{
		//veo si es 0
		if ((bits_t[i + 1] < lambda15) && (bits_t[i + 2] > lambda15))
		{
			*rxcode &= 0xFFFE;
			if (bits)
				*rxcode <<= 1;
			bits--;
		}
		//veo si es 1
		else if ((bits_t[i + 1] > lambda15) && (bits_t[i + 2] < lambda15))
		{
			*rxcode |= 1;
			if (bits)
				*rxcode <<= 1;
			bits--;
		}
		//es un error
		else
		{
			i = transitions;
			resp = RESP_NOK;
		}
	}

	return resp;
}

