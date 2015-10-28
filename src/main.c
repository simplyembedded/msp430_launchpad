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
#include "menu.h"
#include "uart.h"
#include "defines.h"
#include <stddef.h>
#include <string.h>
#include <msp430.h>

static volatile int _blink_enable = 0;
static uint16_t _timer_ms = 0;

static void blink_led(void *arg);
static int set_blink_freq(void);
static int stopwatch(void);

static const struct menu_item main_menu[] = 
{
    {"Set LED blinking frequency", set_blink_freq},
    {"Stopwatch", stopwatch},
};

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    if (board_init() == 0) {
        int timer_handle = -1;

        uart_puts("\n**********************************************");
        uart_puts("\nSimply Embedded tutorials for MSP430 Launchpad");
        uart_puts("\nsimplyembedded.org");
        uart_puts("\nVersion: 0.11");
        uart_puts("\n"__DATE__);
        uart_puts("\n**********************************************");

        menu_init(main_menu, ARRAY_SIZE(main_menu));
        
        while (1) {
            watchdog_pet();
            menu_run();
      
            /**
             * If blinking is enabled and the timer handle is 
             * negative (invalid) create a periodic timer
             */
            if (_blink_enable != 0 ) {
                if (timer_handle < 0) {
                    timer_handle = timer_create(_timer_ms, 1, blink_led, NULL); 
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

static int set_blink_freq(void)
{
    const unsigned int value = menu_read_uint("Enter the LED blinking frequency (Hz): ");

    if (value > 0) {
        _timer_ms = 1000 / (2 * value);
    }

    return (value > 0) ? 0 : -1;
}

static int stopwatch(void)
{
    struct time start_time;
    struct time end_time;
    
    uart_puts("\nPress any key to start/stop the stopwatch: ");
    
    /* Wait to start */
    while (uart_getchar() == -1) {watchdog_pet();}
    
    if (timer_capture(&start_time) == 0) {
        uart_puts("\nRunning...");

        /* Wait to stop */
        while (uart_getchar() == -1) {watchdog_pet();}
        
        if (timer_capture(&end_time) == 0) {
            size_t i;
            char time_str[] = "00000:000";
            unsigned int sec = end_time.sec - start_time.sec;
            unsigned int ms = end_time.ms - start_time.ms;

            /* Convert the seconds to a string */
            for (i = 4; (i > 0) && (sec > 0); i--) {
                time_str[i] = sec % 10 + '0';
                sec /= 10;
            }
   
            /* Convert the milliseconds to a string */
            for (i = 8; (i > 5) && (ms > 0); i--) {
                time_str[i] = ms % 10 + '0';
                ms /= 10;
            }

            /* Display the result */
            time_str[sizeof(time_str) - 1] = '\0';
            uart_puts("\nTime: ");
            uart_puts(time_str);
        }
    }

    return 0;
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

