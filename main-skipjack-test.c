/* main-skipjack-test.c */
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
/*
 * skipjack test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "skipjack.h"

#include <stdint.h>
#include <string.h>


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	uart_putstr("\r\n==testy-encrypt==\r\n key: ");
	uart_hexdump(key,10);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,8);
	skipjack_enc(block,key);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,8);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	uart_putstr("\r\n==testy-decrypt==\r\n key: ");
	uart_hexdump(key,10);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,8);
	skipjack_dec(block,key);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,8);
}

void testrun_skipjack(void){
	uint8_t key[]={ 0x00, 0x99, 0x88, 0x77, 0x66,
					0x55, 0x44, 0x33, 0x22, 0x11};
	uint8_t data[]={ 0x33, 0x22, 0x11, 0x00, 0xdd, 0xcc, 0xbb, 0xaa};
	testencrypt(data,key);
	testdecrypt(data,key);	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (skipjack)\r\nloaded and running\r\n");

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "test")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_skipjack();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
	
}

