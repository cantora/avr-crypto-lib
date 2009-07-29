/* uart_defs.h */
/*
    This file is part of the AVR-uart_ni.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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

#ifndef UART_DEFS_H_
#define UART_DEFS_H_

#define UART_PARATY_NONE 0 /** \def UART_PARATY_NONE define no paraty */
#define UART_PARATY_EVEN 2 /** \def UART_PARATY_EVEN define even paraty */
#define UART_PARATY_ODD  3 /** \def UART_PARATY_ODD define odd paraty */
#define UART_STOPBITS_1 0  /** \def UART_STOPBITS_1 define 1 stop bit */
#define UART_STOPBITS_2 1  /** \def UART_STOPBITS_2 define 2 stop bits */
#define UART_DATABITS_5 0  /** \def UART_DATABITS_5 define 5 data bits */
#define UART_DATABITS_6 1  /** \def UART_DATABITS_6 define 6 data bits */
#define UART_DATABITS_7 2  /** \def UART_DATABITS_7 define 7 data bits */
#define UART_DATABITS_8 3  /** \def UART_DATABITS_8 define 8 data bits */
#define UART_DATABITS_9 7  /** \def UART_DATABITS_9 define 9 data bits */

#endif /* UART_DEFS_H_ */
