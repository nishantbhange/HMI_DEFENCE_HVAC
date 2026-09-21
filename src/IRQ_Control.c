/*
 * IRQ_Control.c
 *
 *  Created on: Sep 21, 2026
 *      Author: Nishant Bhange
 */
#include"IRQ_Control.h"


  uint32_t Get_PRIMASK(void)
{
    uint32_t result;

    __asm volatile ("MRS %0, PRIMASK" : "=r" (result));

    return result;
}

  void Disable_IRQ(void)
{
    __asm volatile ("cpsid i" : : : "memory");
}

  void Enable_IRQ(void)
{
    __asm volatile ("cpsie i" : : : "memory");
}

