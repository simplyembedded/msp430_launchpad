/**
 * \file timer.h
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

#ifndef __TIMER__
#define __TIMER__

#include <stdint.h>

/* Time structure */
struct time
{
    unsigned int sec;
    unsigned int ms;
};

/**
 * \brief Initialize the timer module
 * \return 0 on success, -1 otherwise
 */
int timer_init(void);

/**
 * \brief Create and start a timer
 * \param[in] timeout_ms - the time timeout in ms
 * \param[in] periodic - non-zero for a periodic timer,  0 for single shot
 * \param[in] callback - timer callback function
 * \param[in] arg - callback function private data
 * \return non-negative integer - the timer handle - on success, -1 otherwise
 */
int timer_create(uint16_t timeout_ms, int periodic, void (*callback)(void *), void *arg);

/**
 * \brief Delete a timer
 * \param[in] handle - the timer handle to delete
 * \return 0 if the handle is valid, -1 otherwise
 */
int timer_delete(int handle);

/**
 * \brief Capture the current value of the timer
 * \param[out] time - the time structure to fill with captured time
 * \return 0 on success, -1 otherwise
 */
int timer_capture(struct time *time);

#endif /* __TIMER__ */

