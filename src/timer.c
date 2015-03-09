/**
 * \file timer.c
 * \author Chris Karaplis
 * \brief Timer API
 *
 * Copyright (c) 2015, simplyembedded.org
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "timer.h"
#include <string.h>
#include <msp430.h>

#define MAX_TIMERS  10
#define TIMER_RESOLUTION_MS    100

#define SR_ALLOC() uint16_t __sr
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
#define EXIT_CRITICAL() __set_interrupt_state(__sr)

struct timer
{
    uint16_t expiry;
    uint16_t periodic;
    void (*callback)(void *);
    void *arg;
};

static struct timer _timer[MAX_TIMERS];
static volatile uint16_t _timer_tick = 0;

/**
 * \brief Initialize the timer module
 * \return 0 on success, -1 othewise
 */
int timer_init(void)
{
    /* Clear the timer structure */
    memset(_timer, 0, sizeof(_timer));

    /* Set timer to use SMCLK, clock divider 2, up-mode */
    TA1CTL = TASSEL1 | ID0 | MC0;

    /* TA1CCR0 set to the interval for the desires resolution based on 1MHz SMCLK */
    TA1CCR0 = (((1000000 / 2) / 1000) * TIMER_RESOLUTION_MS) - 1;

    /* Enable CCIE interupt */
    TA1CCTL0 = CCIE;

    return 0;
}

/**
 * \brief Create and start a timer
 * \param[in] timeout_ms - the time timeout in ms
 * \param[in] periodic - non-zero for a periodic timer,  0 for single shot
 * \param[in] callback - timer callback function
 * \param[in] arg - callback function private data
 * \return non-negative integer - the timer handle - on success, -1 otherwise
 */
int timer_create(uint16_t timeout_ms, int periodic, void (*callback)(void *), void *arg)
{
    int handle = -1;
    size_t i;

    /* Find a free timer */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (_timer[i].callback == NULL) {
            break;
        }
    }

    /* Make sure a valid timer is found */
    if (i < MAX_TIMERS) {
        SR_ALLOC();
        ENTER_CRITICAL();

        /* Set up the timer */
        if (periodic != 0) {
            _timer[i].periodic = (timeout_ms < 100) ? 1 : (timeout_ms / TIMER_RESOLUTION_MS);
        } else {
            _timer[i].periodic = 0;
        }

        _timer[i].callback = callback;
        _timer[i].arg = arg;
        _timer[i].expiry = _timer_tick + _timer[i].periodic; 

        EXIT_CRITICAL();
        handle = i;
    } 

    return handle;
}

/**
 * \brief Delete a timer
 * \param[in] handle - the timer handle to delete
 * \return 0 if the handle is valid, -1 otherwise
 */
int timer_delete(int handle)
{
    int status = -1;

    if (handle < MAX_TIMERS) {
        SR_ALLOC();
        ENTER_CRITICAL();

        /* Clear the callback to delete the timer */
        _timer[handle].callback = NULL;

        EXIT_CRITICAL();
        status = 0;
    }

    return status;
}

__attribute__((interrupt(TIMER1_A0_VECTOR))) void timer1_isr(void)
{
    size_t i;

    /* Clear the interrupt flag */
    TA1CCTL0 &= ~CCIFG;

    /* Increment the timer tick */ 
    _timer_tick++;

    for (i = 0; i < MAX_TIMERS; i++) {
        /* If the timer is enabled and expired, invoke the callback */
        if ((_timer[i].callback != NULL) && (_timer[i].expiry == _timer_tick)) {
            _timer[i].callback(_timer[i].arg);

            if (_timer[i].periodic > 0) {
                /* Timer is periodic, calculate next expiration */
                _timer[i].expiry += _timer[i].periodic;
            } else {
                /* If timer is not periodic, clear the callback to disable */
                _timer[i].callback = NULL;
            }
        }
    }
}
