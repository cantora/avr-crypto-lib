/* serial-tools.c */
/*
    This file is part of the AVR-Crypto-Lib.
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
/**
 * 
 * Author:	Daniel Otte
 * Date:		16.05.2006
 * 
 * This tools should help to parse some input. 
 * 
 */

#include "config.h"
#include "uart.h"
#include <string.h>
#include <stdint.h>

int getnextwordn(char *s, int n){ /* words are seperated by spaces, lf or cr */
	char c = ' ';
	do{
		c=uart_getc(); 
	}while(c==' ' || c=='\r' || c=='\n');
	*s++ = c;
	do{ 
	  *s++ = c = uart_getc();
	}while(c!=' ' && c!='\r' && c!='\n' && --n);
	*(s-1) = '\0';
	return n;
}


void readhex2buffer(void* buffer, int n){
	char c;
	uint8_t i;
	
//	DEBUG_S("\r\nDBG: n="); DEBUG_B(n&0xff); DEBUG_S("\r\n");
	for(i=0; i<n; ++i){
		c = uart_getc();
		if ('0'<= c && '9'>=c){
			((uint8_t*)buffer)[i] = c - '0';
		} else {
			c &= ~('A' ^ 'a'); /* make all uppercase */ 
			if ('A'<= c && 'F'>=c){
				((uint8_t*)buffer)[i] = c - 'A' + 10;
			} else {
				/* oh shit, wrong char */
			}
		}
		
		((uint8_t*)buffer)[i] <<= 4;
		
		c = uart_getc();
		if ('0'<= c && '9'>=c){
			((uint8_t*)buffer)[i] |= c - '0';
		} else {
			c &= ~('A' ^ 'a'); /* make all uppercase */ 
			if ('A'<= c && 'F'>=c){
				((uint8_t*)buffer)[i] |= c - 'A' + 10;
			} else {
				/* oh shit, wrong char */
			}
		}
	} /* for i=0 .. n */
}

void uart_putptr(void* p){
	uart_hexdump((void*) &p,2);
}

