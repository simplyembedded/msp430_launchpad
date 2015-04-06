/**
 * \file menu.c
 * \author Chris Karaplis
 * \brief A simple menu implementation
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

#include "menu.h"
#include "uart.h"
#include "watchdog.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static const struct menu_item *_current_menu = NULL;
static size_t _current_menu_size = 0;

static void display_menu(void);

/**
 * \brief Initialize and display the current menu
 * \param[in] menu - array of menu items
 * \param[in] count - number of menu items (max 9)
 */
void menu_init(const struct menu_item *menu, size_t count)
{
    /* Limit menu size to 9 options */
    if (count > 9) {
        count = 9;
    }

    _current_menu = menu;
    _current_menu_size = count;

    display_menu();
}

/**
 * \brief Read user input and execute menu selection
 */
void menu_run(void)
{
    static unsigned int value = 0;
    int c = uart_getchar();

    if ((c >= '0') && (c <= '9')) {
        value *= 10;
        value += c - '0';
        uart_putchar(c);
    } else if ((c == '\n') || (c == '\r')) {
        if ((value > 0) && (value <= _current_menu_size)) {
            /* Invoke the callback */
            if (_current_menu[value - 1].handler != NULL) {
                uart_puts("\n");
                if (_current_menu[value - 1].handler() != 0) {
                    uart_puts("\nError\n");
                }
            }
        } else {    
            uart_puts("\nInvalid selection\n");
        }

        display_menu();
        value = 0;
    } else {
       /* Not a valid character */ 
    }
}


/**
 * \brief Read an unsigned integer from the menu prompt
 * \param[in] prompt - the text to display
 * \return the unsigned integer read 
 */
unsigned int menu_read_uint(const char *prompt)
{
    unsigned int value = 0;
 
    uart_puts(prompt);
    
    while (1) {
        int c = uart_getchar();

        watchdog_pet();
     
        if ((c >= '0') && (c <= '9')) {
            value *= 10;
            value += c - '0';
            uart_putchar(c);
        } else if ((c == '\n') || (c == '\r')) {
            uart_puts("\n");
            break;
        } else {
            /* Not a valid character */
        }
    }

    return value;
}

static void display_menu(void)
{
    size_t index = 0;
    const struct menu_item *menu = _current_menu;
    size_t count = _current_menu_size;

    uart_puts("\nMenu selection:");

    while (count--) {
        static char option[] = "\n0. ";
        option[1] = '1' + index++;
        uart_puts(option);
        uart_puts(menu->text);
        menu++;
    };

    /* Print prompt */
    uart_puts("\n> ");
}
