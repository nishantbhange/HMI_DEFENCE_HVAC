/*
 * Delay.c
 *
 *  Created on: Jun 27, 2026
 *      Author: Nishant Bhange
 */
#include "Delay.h"
#include "HMI.h"

uint16_t Tick_Count=0;
static void Load_counter(uint32_t Load_Value);

void LPIT_Init(void){
	IP_LPIT0->MCR=(1<<LPIT0_MCR_BIT_DBG_EN)|
			      (1<<LPIT0_MCR_BIT_M_CEN);
	//IP_LPIT0->MIER=(1<<LPIT0_MIER_BIT_TIE0);
	IP_LPIT0->TMR[0].TCTRL =(1<<LPIT0_TCTRL0_BIT_TSOI)|
							 (1<<LPIT0_TCTRL0_BIT_TRG_SRC);
}

static void Load_counter(uint32_t Load_Value){
	IP_LPIT0->MSR =(1<<LPIT0_MSR_BIT_TIF0);
	IP_LPIT0->TMR[0].TVAL=Load_Value;
	IP_LPIT0->SETTEN=(1<<LPIT0_SETTEN_BIT_SETEN0);
}

void DelayUs(uint32_t us){

if(us> LPIT_MAX_DELAY_US || us==0){
	return ;
}
uint32_t Ticks = (uint32_t)(((uint64_t)SCG_SIRC_DIV2_clock * us) / 1000000U);
   if(Ticks == 0){
       Ticks = 1;
   }

   uint32_t Load_Value = Ticks - 1;

	Load_counter(Load_Value);
	while(!(IP_LPIT0->MSR & 0x01)){

	}
	IP_LPIT0->MSR =(1<<LPIT0_MSR_BIT_TIF0);

}
void DelayMs(uint32_t ms){

	 //uint32_t Load_Value=(((CoreClockHz / 1000U)*ms) - 1) ;
	 //Systick_Init(Load_Value,SysTick_CTRL_CLKSOURCE_PROCESSOR_CLK,SysTick_EXCEPTION_DI);
	 //while(!(S32_SysTick->CSR & (1<<SYST_CSR_BIT_COUNTFLAG))){}
if(ms>SYSTICK_MAX_DELAY_MS || ms==0){
	return ;
}
	Tick_Count=Global_Tick_Count;
	while((Global_Tick_Count-Tick_Count)<ms){

	}

}
void Systick_Init(uint32_t Load_Value , bool CLKSOURCE , bool SysTick_EXCEPTION )
{
	if(Load_Value>0xFFFFFF)
	{
		return ;
	}
    //  Disable SysTick
	S32_SysTick->CSRr= 0;
	// Set reload value (24-bit maximum)
	S32_SysTick->RVR= Load_Value ;
    // Clear current counter and COUNTFLAG
	S32_SysTick->CVR  = 0;
	 // Enable SysTick: CLKSOURCE=Processor Clock , SysTick_EXCEPTION=Enable Interrupt ,SysTick_EN= Start Counter

    S32_SysTick->CSRr |= (CLKSOURCE<<SYST_CSR_BIT_CLKSRC) |
    		(SysTick_EXCEPTION<<SYST_CSR_BIT_TICKINT) |
					(SysTick_EN<<SYST_CSR_BIT_EN);

}
