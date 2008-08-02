/* main-trivium-test.c */
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
 * Mickey128 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"
#include "cli.h"

#include "trivium.h"
#include "nessie_stream_test.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* cipher_name = "Trivium";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void trivium_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	uint32_t iv=0;
	trivium_init(key, 80, &iv, 32, ctx);
}

uint8_t trivium_getbyte_dummy(trivium_ctx_t* ctx){
	uint8_t i,ret=0;
	for(i=0; i<8; ++i){
		ret<<=1;
		ret |= trivium_enc(ctx);
	}
	return ret;
}

void testrun_nessie_trivium(void){
	nessie_stream_ctx.outsize_b =   8; /* actually unused */
	nessie_stream_ctx.keysize_b =  80; /* this is the one we have refrence vectors for */
	nessie_stream_ctx.ivsize_b  =  32;
	nessie_stream_ctx.name = cipher_name;
	nessie_stream_ctx.ctx_size_B = sizeof(trivium_ctx_t);
	nessie_stream_ctx.cipher_genctx = (nessie_stream_genctx_fpt)trivium_genctx_dummy;
	nessie_stream_ctx.cipher_enc = (nessie_stream_genenc_fpt)trivium_getbyte_dummy;
	
	nessie_stream_run();	
}

void testrun_performance_trivium(void){
	uint64_t t;
	char str[16];
	uint8_t key[10], iv[10];
	trivium_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 10);
	memset(iv,  0, 10);
	
	startTimer(1);
	trivium_init(key, 80, iv, 80, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	startTimer(1);
	trivium_enc(&ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	uart_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_trivium, testrun_nessie_trivium, testrun_performance_trivium};

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
