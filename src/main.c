/**
 * \file main.c
 * \author Chris Karaplis
 * \brief program entry point
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

#include "board.h"
#include "watchdog.h"
#include "timer.h"
#include <stddef.h>
#include <msp430.h>

static volatile int _blink_enable = 0;

static void blink_led(void *arg);

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    if (board_init() == 0) {
        int timer_handle = -1;

        while (1) {
            watchdog_pet();
            
            /**
             * If blinking is enabled and the timer handle is 
             * negative (invalid) create a periodic timer with
             * a timeout of 500ms
             */
            if (_blink_enable != 0 ) {
                if (timer_handle < 0) {
                    timer_handle = timer_create(500, 1, blink_led, NULL); 
                }
            } else {
                if (timer_handle != -1) {
                    timer_delete(timer_handle);
                    timer_handle = -1;
                }
            }
        }
    }

    return 0;
}

static void blink_led(void *arg)
{
    (void) arg;

    /* Toggle P1.0 output */
    P1OUT ^= 0x01;
}

__attribute__((interrupt(PORT1_VECTOR))) void port1_isr(void)
{
    if (P1IFG & 0x8) {
        /* Clear the interrupt flag */
        P1IFG &= ~0x8;

        /* Toggle the blink enable */
        _blink_enable ^= 1;
    }
}

