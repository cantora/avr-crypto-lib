/* uart.h */
/*
    This file is part of the This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UART_H
#define UART_H

/**
 * UART Library
 *
 * #define F_CPU 16000000         // Oszillator-Frequenz in Hz
 * #define UART_INTERRUPT 1
 * #define UART_BAUD_RATE 19200
 * #define UART_RXBUFSIZE 16
 * #define UART_TXBUFSIZE 16
 * #define UART_LINE_BUFFER_SIZE 40
 * #define UART_LEDS             // LED1 and LED2 toggle on tx and rx interrupt
 *
 */


#include "config.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

void uart_init(void);

void uart_putc(char c);
void uart_putstr(char * str);
void uart_putstr_P(PGM_P str);
void uart_hexdump(const void* buf, int len);

char uart_getc(void);
char uart_getc_nb(char *c);		/* returns 1 on success */

/*
  get one Cariage return terminated line
  echo charakters back on Uart
  returns buffer with zero terminated line on success, 0 pointer otherwise
*/
char * uart_getline_nb(void);

#endif
