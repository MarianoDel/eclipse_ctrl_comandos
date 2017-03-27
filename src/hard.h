/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_

//-------- Board Configuration -----------------------//
#define VER_1_0

//--- Tipo de control o ninguno con WITHOUT_POTE
//#define WITH_POTE
//#define WITH_1_TO_10
//#define WITHOUT_POTE

//#ifdef VER_1_0
//#define BOOST_CONVENCIONAL	//placa anterior del tamaño de la F12V5A ultimo prog 13-07-16
//#endif

#ifdef VER_1_0
//GPIOA pin0
#define S1 ((GPIOA->IDR & 0x0001) == 0)

//GPIOA pin1
#define LED ((GPIOA->ODR & 0x0002) != 0)
#define LED_ON	GPIOA->BSRR = 0x00000002
#define LED_OFF GPIOA->BSRR = 0x00020000

//GPIOA pin2

//GPIOA pin3
#define TX_CODE ((GPIOA->ODR & 0x0008) != 0)
#define TX_CODE_ON	GPIOA->BSRR = 0x00000008
#define TX_CODE_OFF GPIOA->BSRR = 0x00080000

//GPIOA pin4
#define S2 ((GPIOA->IDR & 0x0010) == 0)

//GPIOA pin5
#define RX_CODE ((GPIOA->IDR & 0x0020) != 0)

//GPIOA pin6
#define RX_EN 		((GPIOA->ODR & 0x0040) != 0)
#define RX_EN_ON	GPIOA->BSRR = 0x00000040
#define RX_EN_OFF 	GPIOA->BSRR = 0x00400000

//GPIOA pin7

//GPIOA pin9
//GPIOA pin10
//GPIOA pin13
//GPIOA pin14

//GPIOB pin1
#endif

//-------- Respuestas de Funciones -------------------------//
enum
{
	RESP_CONTINUE = 0,
	RESP_SELECTED,
	RESP_CHANGE,
	RESP_CHANGE_ALL_UP,
	RESP_WORKING,
	RESP_FINISH,
	RESP_YES,
	RESP_NO,
	RESP_NO_CHANGE,
	RESP_OK,
	RESP_NOK,
	RESP_NO_ANSWER,
	RESP_TIMEOUT,
	RESP_READY
} typedef RspMessages;

//ESTADOS DEL PROGRAMA PRINCIPAL
enum {
	CHECK_EVENTS = 0,
	TX_S,
	TX_S_A,
	RX_WAIT_BUTTON,
	RX_WAIT_BUTTON_B,
	RX_WAIT_BUTTON_C,
	RX_S,
	RX_S_A,
	RX_S_OK,
	RX_S_TO,
	RX_S_NOK,
	SAVE_PARAMS,
	SLEEPING

} typedef MainStates;

#define REPEAT_CODES	3
#define SWITCHES_TIMER_RELOAD	10

#define SWITCHES_ROOF			400
#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	10		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TIMER_SLEEP			200		//200ms
#define TIMER_FOR_SAVE		4000	//4 segundos

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

#endif /* HARD_H_ */
