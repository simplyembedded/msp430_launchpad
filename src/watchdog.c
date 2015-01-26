/**
 * \file watchdog.c
 * \author Chris Karaplis
 * \brief Watchdog API
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

#include "watchdog.h"
#include <msp430.h>

/**
 * \brief Disable the watchdog timer module
 */
void watchdog_disable(void)
{
    /* Hold the watchdog */
    WDTCTL = WDTPW + WDTHOLD;
}

/**
 * \brief Enable the watchdog timer module
 * The watchdog timeout is set to an interval of 32768 cycles 
 */
void watchdog_enable(void)
{
    /* Read the watchdog interrupt flag */
    if (IFG1 & WDTIFG) {
        /* Clear if set */
        IFG1 &= ~WDTIFG; 
    }

    watchdog_pet();
}

/**
 * \brief Pet the watchdog
 */
void watchdog_pet(void)
{
    /**
     * Enable the watchdog with following settings
     *   - sourced by ACLK
     *   - interval = 32786 / 12000 = 2.73s
     */ 
    WDTCTL = WDTPW + (WDTSSEL | WDTCNTCL);
}

