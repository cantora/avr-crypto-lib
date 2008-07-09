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
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


char* cipher_name = "Skipjack";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void skipjack_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, 10);
}

void testrun_nessie_skipjack(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   =  80;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = 10;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)skipjack_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)skipjack_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)skipjack_genctx_dummy;
	
	nessie_bc_run();
}


void testrun_performance_skipjack(void){
	uint16_t i,c;
	uint64_t t;
	char str[16];
	uint8_t key[10], data[8];
	
	calibrateTimer();
	getOverhead(&c, &i);
	uart_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(c, str, 10);
	uart_putstr_P(PSTR("\r\n\tconst overhead:     "));
	uart_putstr(str);
	utoa(i, str, 10);
	uart_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	uart_putstr(str);
	
	memset(key,  0, 10);
	memset(data, 0,  8);
	
	startTimer(1);
	skipjack_enc(data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	skipjack_dec(data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}

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

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_skipjack, testrun_skipjack, testrun_performance_skipjack};

	while(1){ 
		if (!getnextwordn(str,20)){DEBUG_S("DBG: W1\r\n"); goto error;}
		if(execcommand_d0_P(str, u, v)<0){
			uart_putstr_P(PSTR("\r\nunknown command\r\n"));
		}
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}

