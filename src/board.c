/**
 * \file board.c
 * \author Chris Karaplis
 * \brief Board initialization API 
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
#include "tlv.h"
#include "timer.h"
#include "uart.h"
#include <msp430.h>

/**
 * \brief Initialize all board dependant functionality
 * \return 0 on success, -1 otherwise
 */
int board_init(void)
{
    uart_config_t config;

    watchdog_disable();

    /* Check calibration data */
    if (tlv_verify() != 0) {
        /*  Calibration data is corrupted...hang */
        while(1);
    }
    
    /* Configure the clock module - MCLK = 1MHz */
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    /* Configure ACLK to to be sourced from VLO = ~12KHz */
    BCSCTL3 |= LFXT1S_2;

    /* Initialize the timer module */
    if (timer_init() != 0) {
        /* Timers could not be initialized...hang */
        while (1);
    }

    /* Configure P1.0 as digital output */
    P1SEL &= ~0x01;
    P1DIR |= 0x01;

    /* Set P1.0 output high */
    P1OUT |= 0x01;

    /* Configure P1.3 to digital input */
    P1SEL &= ~0x08;
    P1SEL2 &= ~0x08;
    P1DIR &= ~0x08;

    /* Pull-up required for rev 1.5 Launchpad */
    P1REN |= 0x08;
    P1OUT |= 0x08;
    
    /* Set P1.3 interrupt to active-low edge  */
    P1IES |= 0x08;

    /* Enable interrupt on P1.3 */
    P1IE |= 0x08;

    /* Configure P1.1 and P1.2 for UART (USCI_A0) */
    P1SEL |= 0x6;
    P1SEL2 |= 0x6;

    /* Global interrupt enable */
    __enable_interrupt();
 
    watchdog_enable();
    
    /* Initialize UART to 9600 baud */
    config.baud = 9600;

    if (uart_init(&config) != 0) {
        while (1);
    }

    return 0;
}

