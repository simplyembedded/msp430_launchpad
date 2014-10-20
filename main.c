/**
 * @file: main.c
 * @author: Chris Karaplis
 * @brief program entry point
 *
 * Changelog
 * ---------
 * 14-10-06: The LED attached to P1.0 is toggled in an infite loop.
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

    /* Set the MCLK frequency to 1MHz */
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    /* Set all pins to digital IO */
    P1SEL = 0x0;
    P2SEL = 0x0;

    /* Set P1.0 direction to output */
    P1DIR |= 0x01;

    /* Set P1.0 output high */
    P1OUT |= 0x01;

    /* Set P1.3 to input */
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
    /* Calibration is 64 bytes, XOR'd in 2 byte words */
    size_t len = 62 / 2;
    uint16_t *data = (uint16_t *) 0x10c2;
    uint16_t crc = 0;

    while (len-- > 0) {
        crc ^= *(data++);
    }

    return (TLV_CHECKSUM + crc);
}
