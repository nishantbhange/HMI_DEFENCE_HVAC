/*
 * Event_Queue.c
 *
 *  Created on: Sep 21, 2026
 *      Author: Nishant Bhange
 */

#include "Event_Queue.h"
#include"IRQ_Control.h"
static volatile HMI_EventMsg_t EvQ[EVQ_CAPACITY];
static volatile uint32_t       EvQ_Head = 0U;   /* next slot to write - producer-owned */
static volatile uint32_t       EvQ_Tail = 0U;   /* next slot to read  - consumer-owned */
static volatile uint32_t       EvQ_Overflow = 0U;

/*
 * PRIMASK-based critical section: masks ALL interrupts for exactly the
 * instructions it takes to copy one 8-byte message in or out - on a
 * 48 MHz Cortex-M4 that is on the order of 10-15 cycles (~300 ns).
 * Negligible next to the 1 ms SysTick period or the few-microsecond ADC
 * ISR, and it is the only mechanism on this core that actually prevents
 * preemption of a read-modify-write, regardless of interrupt priority.
 *
 * PRIMASK is saved/restored (not unconditionally re-enabled) so this is
 * safe even if it is ever called from inside another critical section.
 */
void Event_Post(HMI_Event_t ev, ErrorCode_t code)
{
    uint32_t primask =Get_PRIMASK();
    Disable_IRQ();

    uint32_t head = EvQ_Head;
    uint32_t next = (head + 1U) & (EVQ_CAPACITY - 1U);

    if (next != EvQ_Tail)                 /* room available */
    {
        EvQ[head].ev   = ev;
        EvQ[head].code = code;
        EvQ_Head = next;                  /* publish last */
    }
    else
    {
        /* Queue full: only possible if the consumer has been starved for
         * EVQ_CAPACITY consecutive posts, i.e. the main loop has stalled
         * for an extended period (e.g. stuck in a blocking EEPROM wait -
         * see the earlier review's S-7). Drop the newest event rather than
         * overwrite an older undelivered one, and count it so it is
         * visible on a display/log instead of silently vanishing. */
        EvQ_Overflow++;
    }

    if (primask == 0U) { Enable_IRQ(); }
}

bool Event_Get(HMI_EventMsg_t *out)
{
    /* Single reader (main loop only), but Head/Tail are still touched by
     * Event_Post from any interrupt context, so this still needs the
     * critical section. */
    uint32_t primask =Get_PRIMASK();
    Disable_IRQ();

    bool have_one = (EvQ_Tail != EvQ_Head);
    if (have_one)
    {
        out->ev   = EvQ[EvQ_Tail].ev;
        out->code = EvQ[EvQ_Tail].code;
        EvQ_Tail  = (EvQ_Tail + 1U) & (EVQ_CAPACITY - 1U);
    }

    if (primask == 0U) { Enable_IRQ(); }
    return have_one;
}

uint32_t Event_Queue_Overflow_Count(void) { return EvQ_Overflow; }
