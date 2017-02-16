/*
 * pwr.c
 *
 *  Created on: 16/02/2017
 *      Author: Mariano
 */

#include "pwr.h"
#include "stm32f0xx.h"
#include "core_cm0.h"

void PwrInit (void)
{
	if (!RCC_PWR_CLK)
		RCC_PWR_CLK_ON;
}


/**
  * @brief  Enters STOP mode.
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event,
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional
  *         startup delay is incurred when waking up from Stop mode.
  *         By keeping the internal regulator ON during Stop mode, the consumption
  *         is higher although the startup time is reduced.
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *         This parameter can be one of the following values:
  *             @arg PWR_Regulator_ON: STOP mode with regulator ON
  *             @arg PWR_Regulator_LowPower: STOP mode with regulator in low power mode
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *         This parameter can be one of the following values:
  *             @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *             @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
                @arg PWR_STOPEntry_SLEEPONEXIT: enter STOP mode with SLEEPONEXIT instruction
  * @retval None
  */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;

  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDSR bits */
  tmpreg &= CR_DS_MASK;

  /* Set LPDSR bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;

  /* Store the new value */
  PWR->CR = tmpreg;

  /* Set SLEEPDEEP bit of Cortex-M0 System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI();
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  }
  else if (PWR_STOPEntry == PWR_STOPEntry_WFE)
  {
    /* Request Wait For Event */
    __WFE();
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  }
  else
  {
    /* Set SLEEP on exit bit of Cortex-M0 System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
  }
}

