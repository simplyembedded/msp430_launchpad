/**
 * \file i2c.h
 * \author Chris Karaplis
 * \brief I2C master driver
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

#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stddef.h>

/* I2C slave device structure */
struct i2c_device
{
    uint8_t address;
};

/* I2C transaction data */
struct i2c_data
{
    const void *tx_buf;
    size_t tx_len;
    void *rx_buf;
    size_t rx_len;
};

/**
 * \brief Initialize the I2C peripheral
 * \return 0 on success, -1 otherwise
 */
int i2c_init(void);

/**
 * \brief Perform an I2C transfer
 * \param[in] dev - the I2C slave device
 * \param[in/out] data - data structure containing the buffers
 * \return 0 on success, -1 otherwise
 */
int i2c_transfer(const struct i2c_device *dev, struct i2c_data *data);

#endif /* __I2C_H__ */ 
