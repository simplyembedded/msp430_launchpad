/**
 * @file: main.c
 * @author: Chris Karaplis
 * @brief program entry point
 *
 * Changelog
 * ---------
 * 14-10-06: The LED attached to P1.0 is toggled in an infite loop.
 * 14-10-19: Configure clock module for MCLK = 1MHz and the LED to blink at 2Hz.
 *           Configure P1.3 as digital input and wait for button press before blinking
 *
 *
 * Copyright (c) 2014, simplyembedded.org
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

#include <msp430.h>
#include <stdint.h>
#include <stddef.h>

/* LED blinking frequency */
#define LED_BLINK_FREQ_HZ   2

/* Number of cycles to delay based on 1MHz MCLK */
#define LED_DELAY_CYCLES    (1000000 / LED_BLINK_FREQ_HZ)

static int  _verify_cal_data(void);

int main(int argc, char *argv[])
{
    /* Hold the watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    if (_verify_cal_data() != 0) {
        /*  Calibration data is corrupted...hang */
        while(1);
    }
    
    /* Configure the clock module - MCLK = 1MHz */
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    /* Configure P1.0 as digital output */
    P1SEL &= ~0x01;
    P1DIR |= 0x01;

    /* Set P1.0 output high */
    P1OUT |= 0x01;

    /* Configure P1.3 to digital input */
    P1SEL &= ~0x08;
    P1DIR &= ~0x08;

    /* Wait forever until the button is pressed */
    while (P1IN & 0x08);

    /* Now start blinking */
    while (1) {
        /* Wait for LED_DELAY_CYCLES cycles */
        __delay_cycles(LED_DELAY_CYCLES);
        
        /* Toggle P1.0 output */
        P1OUT ^= 0x01;
    }
}

static int _verify_cal_data(void)
{
    size_t len = 62 / 2;
    uint16_t *data = (uint16_t *) 0x10c2;
    uint16_t crc = 0;

    while (len-- > 0) {
        crc ^= *(data++);
    }

    return (TLV_CHECKSUM + crc);
}
