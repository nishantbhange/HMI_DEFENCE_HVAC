/*
 * Event_Queue.h
 *
 *  Created on: Sep 21, 2026
 *      Author: Nishant Bhange
 */

/*
 * Event_Queue.h
 *
 * Interrupt-safe event delivery between ISRs and the main loop.
 *
 * Replaces the racy pair of globals:
 *     volatile HMI_Event_t  Event;
 *     volatile ErrorCode_t  Current_Error;
 * which were written from multiple interrupt contexts (SysTick_Handler,
 * ADC0_IRQHandler) and one foreground context (Service_ADC), and read
 * across THREE separate, non-atomic statements in foreground code
 * (Process_Pending_Event, Error_Handler, HMI_Process_Event). Any interrupt
 * landing between those reads could substitute a different fault code, and
 * a single-slot "mailbox" could never hold more than one pending event
 * regardless of locking.
 */
#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include "HMI.h"      /* HMI_Event_t, ErrorCode_t */

/* One event and its fault code, always moved together so they can never be
 * observed as a mismatched pair. */
typedef struct {
    HMI_Event_t  ev;
    ErrorCode_t  code;   /* meaningful only for Event_Error / Event_Error_Clear;
                            pass Error_None for every other event. */
} HMI_EventMsg_t;

/* Power of two. SysTick can post at most one event per 1 ms tick; the main
 * loop drains on every pass (worst case a few ms with LCD activity). 16
 * gives an order of magnitude of headroom over anything this design
 * produces, at a cost of 16 * 8 bytes = 128 bytes RAM. */
#define EVQ_CAPACITY   16U

/* Callable from ANY context - ISR or foreground. */
void Event_Post(HMI_Event_t ev, ErrorCode_t code);

/* Foreground-only. Non-blocking: returns false if the queue is empty. */
bool Event_Get(HMI_EventMsg_t *out);

/* Diagnostic: counts posts that were dropped because the queue was full.
 * Should read 0 forever in a healthy system - see section 6. */
uint32_t Event_Queue_Overflow_Count(void);

#endif /* EVENT_QUEUE_H_ */


