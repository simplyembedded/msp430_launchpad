/**
 * \file i2c.c
 * \author Chris Karaplis
 * \brief I2C master driver
 *
 * Copyright (c) 2017, simplyembedded.org
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
 *
 */

#include "i2c.h"
#include "defines.h"
#include <msp430.h>

static int _transmit(const struct i2c_device *dev, const uint8_t *buf, size_t nbytes);
static int _receive(const struct i2c_device *dev, uint8_t *buf, size_t nbytes);
static int _check_ack(const struct i2c_device *dev);

/**
 * \brief Initialize the I2C peripheral
 * \return 0 on success, -1 otherwise
 */
int i2c_init(void)
{
    /* Ensure USCI_B0 is in reset before configuring */
    UCB0CTL1 = UCSWRST;

    /* Set USCI_B0 to master mode I2C mode */
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    /**
     * Configure the baud rate registers for 100kHz when sourcing from SMCLK
     * where SMCLK = 1MHz
     */
    UCB0BR0 = 10;
    UCB0BR1 = 0;
    
    /* Take USCI_B0 out of reset and source clock from SMCLK */
    UCB0CTL1 = UCSSEL_2;
    
    return 0;
}

/**
 * \brief Perform an I2C transfer
 * \param[in] dev - the I2C slave device
 * \param[in/out] data - data structure containing the buffers
 * \return 0 on success, -1 otherwise
 */
int i2c_transfer(const struct i2c_device *dev, struct i2c_data *data)
{
    int err = 0;

    /* Set the slave device address */
    UCB0I2CSA = dev->address;
 
    /* Transmit data is there is any */
    if (data->tx_len > 0) {
        err = _transmit(dev, (const uint8_t *) data->tx_buf, data->tx_len);
    }

    /* Receive data is there is any */
    if ((err == 0) && (data->rx_len > 0)) {
        err = _receive(dev, (uint8_t *) data->rx_buf, data->rx_len);
    } else {
        /* No bytes to receive send the stop condition */
        UCB0CTL1 |= UCTXSTP; 
    }
            
    return err;
}

/**
 * \brief Check for ACK/NACK and handle NACK condition if occured
 * \param[in] dev - the I2C slave device
 * \return 0 if slave ACK'd, -1 if slave NACK'd
 */
static int _check_ack(const struct i2c_device *dev)
{
    int err = 0;
    IGNORE(dev);
 
    /* Check for ACK */
    if (UCB0STAT & UCNACKIFG) {    
        /* Stop the I2C transmission */
        UCB0CTL1 |= UCTXSTP;

        /* Clear the interrupt flag */
        UCB0STAT &= ~UCNACKIFG;

        /* Set the error code */
        err = -1;
    }

    return err;
}

/**
 * \brief Transmit data to the slave device
 * \param[in] dev - the I2C slave device
 * \param[in] buf - the buffer of data to transmit
 * \param[in] nbytes - the number of bytes to transmit
 * \return 0 on success, -1 otherwise
 */
static int _transmit(const struct i2c_device *dev, const uint8_t *buf, size_t nbytes)
{
    int err = 0;
    IGNORE(dev);

    /* Send the start condition */
    UCB0CTL1 |= UCTR | UCTXSTT;        

    /* Wait for the start condition to be sent and ready to transmit interrupt */
    while ((UCB0CTL1 & UCTXSTT) && ((IFG2 & UCB0TXIFG) == 0));

    /* Check for ACK */
    err = _check_ack(dev);
    
    /* If no error and bytes left to send, transmit the data */
    while ((err == 0) && (nbytes > 0)) {
        UCB0TXBUF = *buf;
        while ((IFG2 & UCB0TXIFG) == 0) {
            err = _check_ack(dev);
            if (err < 0) {
                break;
            }
        }

        buf++;
        nbytes--;
    }
    
    return err;
}

/**
 * \brief Receive data from the slave device
 * \param[in] dev - the I2C slave device
 * \param[in] buf - the buffer to store the received data
 * \param[in] nbytes - the number of bytes to receive
 * \return 0 on success, -1 otherwise
 */
static int _receive(const struct i2c_device *dev, uint8_t *buf, size_t nbytes)
{
    int err = 0;
    IGNORE(dev);

    /* Send the start and wait */
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;

    /* Wait for the start condition to be sent */
    while (UCB0CTL1 & UCTXSTT);
    
    /* 
     * If there is only one byte to receive, then set the stop
     * bit as soon as start condition has been sent
     */
    if (nbytes == 1) {
        UCB0CTL1 |= UCTXSTP;
    }

    /* Check for ACK */
    err = _check_ack(dev);

    /* If no error and bytes left to receive, receive the data */
    while ((err == 0) && (nbytes > 0)) {
        /* Wait for the data */
        while ((IFG2 & UCB0RXIFG) == 0);

        *buf = UCB0RXBUF;
        buf++;
        nbytes--;

        /* 
         * If there is only one byte left to receive
         * send the stop condition
         */
        if (nbytes == 1) {
            UCB0CTL1 |= UCTXSTP; 
        }
    }

    return err;
}

