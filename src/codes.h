/*
 * codes.h
 *
 *  Created on: 17/02/2017
 *      Author: Mariano
 */

#ifndef CODES_H_
#define CODES_H_

//Code States
enum {
	C_INIT = 0,
	C_SENDING,
	C_FINISH

} typedef CodeState;

//--- Funciones del Modulo ---
unsigned char SendCode16 (unsigned short, unsigned char);
void SendCode16Reset (void);

#endif /* CODES_H_ */
