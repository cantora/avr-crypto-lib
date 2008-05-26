/* config.h */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
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
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <avr/io.h>

#define F_CPU 16000000         /* Oszillator-Frequenz in Hz */

#define DEBUG uart

/* uart.[ch] defines */
#define UART_INTERRUPT 1
#define UART_BAUD_RATE 9600
#define UART_RXBUFSIZE 16
#define UART_TXBUFSIZE 16
#define UART_LINE_BUFFER_SIZE 40
#undef UART_LEDS
/*
#define UART_HWFLOWCONTROL
#define UART_RTS_PORT PORTA
#define UART_RTS_DDR DDRA
#define UART_CTS_PIN PINA
#define UART_CTS_DDR DDRA
#define UART_RTS_BIT 0
#define UART_CTS_BIT 1
*/

//#define ATMEGA644 /* this is now done by make */

#endif

