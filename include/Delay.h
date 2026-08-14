/*
 * Delay.h
 *
 *  Created on: Jun 27, 2026
 *      Author: Nishant Bhange
 */

#ifndef DELAY_H_
#define DELAY_H_
#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>
#include<S32K144.h>

#define CoreClockHz                           48000000U
#define SCG_SIRC_DIV2_clock                   8000000U
#define SysTick_CTRL_CLKSOURCE_PROCESSOR_CLK  1
#define SysTick_CTRL_CLKSOURCE_EXT_CLK        0
#define SysTick_EXCEPTION_EN                  1
#define SysTick_EXCEPTION_DI                  0
#define SysTick_EN                            1
#define SysTick_DI                            0

#define SYST_CSR_BIT_EN                       0
#define SYST_CSR_BIT_TICKINT                  1
#define SYST_CSR_BIT_CLKSRC                   2
#define SYST_CSR_BIT_COUNTFLAG                16

#define LPIT0_MCR_BIT_M_CEN                   0
#define LPIT0_MCR_BIT_DBG_EN                  3
#define LPIT0_MIER_BIT_TIE0                   0
#define LPIT0_TCTRL0_BIT_TSOI                 17
#define LPIT0_TCTRL0_BIT_TRG_SRC              23

#define LPIT0_MSR_BIT_TIF0                    0
#define LPIT0_SETTEN_BIT_SETEN0               0


#define COMPRESSOR_DELAY_MS                   180000U
#define LPIT_MAX_DELAY_US  ((uint32_t)(((uint64_t)0xFFFFFFFFULL * 1000000ULL) / SCG_SIRC_DIV2_clock))
#define SYSTICK_MAX_DELAY_MS  ((uint32_t)(((uint64_t)0xFFFFFFFFULL * 1000000ULL) / CoreClockHz))


void LPIT_Init(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);
void Systick_Init(uint32_t Load_Value , bool CLKSOURCE , bool SysTick_EXCEPTION );

extern volatile bool LPIT_Timeout_Flag;
extern volatile uint32_t LPIT_Timeout_Count;

#endif /* DELAY_H_ */
