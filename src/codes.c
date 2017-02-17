/*
 * codes.c
 *
 *  Created on: 17/02/2017
 *      Author: Mariano
 */

#include "codes.h"
#include "hard.h"
#include "tim.h"

//--- External Variables -----

//--- Global Variables -------
unsigned char send_state = 0;

//--- Module Functions -------

//Envia el codigo de hasta 2 bytes (16bits), c es codigo, bits la cantidad a enviar
//contesta RESP_CONTINUE si falta o RESP_OK si termino
unsigned char SendCode16 (unsigned short c, unsigned char bits)
{
	RspMessages resp = RESP_CONTINUE;

	switch (send_state)
	{
		case C_INIT:
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

