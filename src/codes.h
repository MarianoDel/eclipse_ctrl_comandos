/*
 * codes.h
 *
 *  Created on: 17/02/2017
 *      Author: Mariano
 */

#ifndef CODES_H_
#define CODES_H_


#define		DEFAULT_LAMBDA		560		//en us
//Code States
enum {
	C_INIT = 0,
	C_SEND_PILOT_A,
	C_SEND_PILOT_B,
	C_SENDING,
	C_SEND_ONE_A,
	C_SEND_ONE_B,
	C_SEND_ONE_C,
	C_SEND_ZERO_A,
	C_SEND_ZERO_B,
	C_SEND_ZERO_C,
	C_FINISH

} typedef CodeState;

//--- Funciones del Modulo ---
unsigned char SendCode16 (unsigned short, unsigned char);
void SendCode16Reset (void);

#endif /* CODES_H_ */
