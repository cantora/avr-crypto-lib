/* main-hmac-sha1-test.c */
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
 * HMAC-SHA1 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "sha1.h"
#include "hmac-sha1.h"

#include "nessie_mac_test.h"

#include <stdint.h>
#include <string.h>
#include "cli.h"

char* algo_name = "HMAC-SHA1";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void hmacsha1_next_dummy(void* buffer, void* ctx){
	sha1_nextBlock(ctx, buffer);
}

void hmacsha1_init_dummy(void* key, uint16_t keysize_b, void* ctx){
	hmac_sha1_init(ctx, key, keysize_b);
}

void hmacsha1_last_dummy(void* buffer, uint16_t size_b, void* key, uint16_t keysize_b, void* ctx){
	sha1_lastBlock(ctx, buffer, size_b);
	hmac_sha1_final(ctx, key, keysize_b);
}

void testrun_nessie_hmacsha1(void){
	nessie_mac_ctx.macsize_b   = 160;
	nessie_mac_ctx.keysize_b   = 512;
	nessie_mac_ctx.blocksize_B = 512/8;
	nessie_mac_ctx.ctx_size_B  = sizeof(hmac_sha1_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = (nessie_mac_init_fpt)hmacsha1_init_dummy;
	nessie_mac_ctx.mac_next = (nessie_mac_next_fpt)hmacsha1_next_dummy;
	nessie_mac_ctx.mac_last = (nessie_mac_last_fpt)hmacsha1_last_dummy;
	nessie_mac_ctx.mac_conv = (nessie_mac_conv_fpt)sha1_ctx2hash;
	
	nessie_mac_run();
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(algo_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0");
	void_fpt v[] = {testrun_nessie_hmacsha1, testrun_nessie_hmacsha1};

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

