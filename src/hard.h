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
#define S1 ((GPIOB->IDR & 0x0001) == 0)

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
#define S2 ((GPIOB->IDR & 0x0010) == 0)

//GPIOA pin5
//GPIOA pin6
//GPIOA pin7

//GPIOA pin9
//GPIOA pin10
//GPIOA pin13
//GPIOA pin14

//GPIOB pin1
#endif


//ESTADOS DEL PROGRAMA PRINCIPAL
enum {
	ERROR_INIT = 0,
	ERROR_LOW_VIN,
	ERROR_HIGH_VIN,
	ERROR_WRONG_VIN,
	ERROR_RUN,
	ERROR_RUN_A,
	ERROR_RUN_B,
	ERROR_OK

} typedef ErrorState;

#define SWITCHES_TIMER_RELOAD	10

#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TIMER_SLEEP			6000	//6 segundos

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

#endif /* HARD_H_ */
