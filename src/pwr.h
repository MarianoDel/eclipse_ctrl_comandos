/*
 * pwr.h
 *
 *  Created on: 16/02/2017
 *      Author: Mariano
 */

#ifndef PWR_H_
#define PWR_H_

#include <stdint.h>

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)

#define PWR_STOPEntry_WFI               ((uint8_t)0x01)
#define PWR_STOPEntry_WFE               ((uint8_t)0x02)
#define PWR_STOPEntry_SLEEPONEXIT       ((uint8_t)0x03)

#define PWR_Regulator_ON                ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower          PWR_CR_LPSDSR


#define RCC_PWR_CLK 		(RCC->APB1ENR & 0x10000000)
#define RCC_PWR_CLK_ON 		RCC->APB1ENR |= 0x10000000
#define RCC_PWR_CLK_OFF 	RCC->APB1ENR &= ~0x10000000


//Funciones del modulo
void PwrInit (void);
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);

#endif /* PWR_H_ */
