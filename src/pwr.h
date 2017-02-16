/*
 * pwr.h
 *
 *  Created on: 16/02/2017
 *      Author: Mariano
 */

#ifndef PWR_H_
#define PWR_H_

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)

#define PWR_STOPEntry_WFI               ((uint8_t)0x01)
#define PWR_STOPEntry_WFE               ((uint8_t)0x02)
#define PWR_STOPEntry_SLEEPONEXIT       ((uint8_t)0x03)


#endif /* PWR_H_ */
