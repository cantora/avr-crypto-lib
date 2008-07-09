/* main-md5-test.c */
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
 * md5 test suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "md5.h"
#include "nessie_hash_test.h"

#include <stdint.h>
#include <string.h>
#include "cli.h"

char* algo_name = "MD5";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void md5_next_dummy(void* buffer, void* ctx){
	md5_nextBlock(ctx, buffer);
}

void md5_last_dummy(void* buffer, uint16_t size_b, void* ctx){
	md5_lastBlock(ctx, buffer, size_b);
}

void md5_ctx2hash_dummy(void* buffer, void* ctx){
	memcpy(buffer, ctx, 16);
}


void testrun_nessie_md5(void){
	nessie_hash_ctx.hashsize_b  = 128;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(md5_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)md5_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)md5_next_dummy;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)md5_last_dummy;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)md5_ctx2hash_dummy;
	
	nessie_hash_run();
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

/*
 * MD5 test suite:
 * MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
 * MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
 * MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
 * MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 * d174ab98d277d9f5a5611c2c9f419d9f
 * MD5 ("123456789012345678901234567890123456789012345678901234567890123456
 * 78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 */

void testrun_md5(void){
	md5_ctx_t s;
	char* testv[]={"", "a", "abc", "message digest", "abcdefghijklmnopqrstuvwxyz", 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
	uint8_t i;
	
	uart_putstr("\r\n=== MD5 test suit ===");
	for(i=0; i<7; ++i){
		uart_putstr("\r\n MD5 (\"");
		uart_putstr(testv[i]);
		uart_putstr("\") = \r\n");
		md5_init(&s);
		md5_lastBlock(&s, testv[i], strlen(testv[i])*8);
		uart_hexdump(&s.a[0], 16);
	}
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (MD5)\r\nloaded and running\r\n");
	PGM_P    u   = PSTR("nessie\0test\0");
	void_fpt v[] = {testrun_nessie_md5, testrun_md5};

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

