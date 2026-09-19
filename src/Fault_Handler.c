
//overrides the weak definitions in exceptions.c
 /* in a .noinit section */
#include<stdint.h>
#include "S32K144.h"

volatile uint32_t Fault_Record_Type
    __attribute__((section(".noinit")));

volatile uint32_t Fault_Record_PC
    __attribute__((section(".noinit")));

volatile uint32_t Fault_Record_CFSR
    __attribute__((section(".noinit")));

static void Fault_Enter(uint32_t type)
{
    //Plant safe, immediately and unconditionally.
    IP_PTE->PCOR = (1UL << 7);                       /* Compressor  */
    IP_PTA->PCOR = (1UL << 6) | (1UL << 7);          /* Condenser, Blower */
    IP_PTC->PCOR = (1UL << 8) | (1UL << 9);          /* Reserve, Heater   */
    IP_PTB->PCOR = (1UL << 0) | (1UL << 1);          /* Heater2, Solenoid */
    //__DSB();
    __asm("DSB");
    // Record for post-mortem (survives reset in .noinit)
    Fault_Record_Type = type;
    Fault_Record_CFSR = S32_SCB->CFSR;

    //let the watchdog reset
    for(;;) { }
}

void HardFault_Handler(void)  { Fault_Enter(0x01U); }
void BusFault_Handler(void)   { Fault_Enter(0x02U); }
void UsageFault_Handler(void) { Fault_Enter(0x03U); }
void MemManage_Handler(void)  { Fault_Enter(0x04U); }
void NMI_Handler(void)        { Fault_Enter(0x05U); }
void undefined_handler(void)  { Fault_Enter(0x06U); }
