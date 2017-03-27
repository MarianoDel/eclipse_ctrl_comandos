/*
 * codes.h
 *
 *  Created on: 17/02/2017
 *      Author: Mariano
 */

#ifndef CODES_H_
#define CODES_H_

#include "gpio.h"

#define RX_CODE_PLLUP_ON Gpio5PullUpOn()
#define RX_CODE_PLLUP_OFF Gpio5PullUpOff()

#define SIZEOF_BUFF_TRANS	64



#define		DEFAULT_LAMBDA		540		//en us
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

} typedef CodeStateTX;

enum {
	C_RXINIT = 0,
	C_RXINIT_PULLUP,
	C_RXWAIT_PILOT_A,
	C_RXWAIT_PILOT_B,
	C_RXWAIT_BITS_B,
	C_RXWAIT_BITS_C,
	C_RXERROR,
	C_RXOK

} typedef CodeStateRX;

//--- Funciones del Modulo ---
unsigned char SendCode16 (unsigned short, unsigned char, unsigned short);
void SendCode16Reset (void);
unsigned char RecvCode16 (unsigned char *);
void RecvCode16Reset (void);
unsigned char UpdateTransitions (unsigned char, unsigned short *, unsigned short *);

#endif /* CODES_H_ */
