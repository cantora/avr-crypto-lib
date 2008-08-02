/* main-shacal2_enc-test.c */
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
 * Shacal2 encryption only test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "shacal2_enc.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* cipher_name = "Shacal2 encryption only";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void shacal2_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, (keysize_b+7)/8);
}

void shacal2_enc_dummy(void* buffer, void* ctx){
	shacal2_enc(buffer, ctx, SHACAL2_KEYSIZE);
}

void testrun_nessie_shacal2enc(void){
	nessie_bc_ctx.blocksize_B = SHACAL2_BLOCKSIZE_B;
	nessie_bc_ctx.keysize_b   = SHACAL2_KEYSIZE;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = SHACAL2_KEYSIZE_B;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)shacal2_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)NULL;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)shacal2_genctx_dummy;
	
	nessie_bc_run();	
}

void testrun_performance_shacal2enc(void){
	uint64_t t;
	uint8_t key[SHACAL2_KEYSIZE_B], data[SHACAL2_BLOCKSIZE_B];
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, SHACAL2_KEYSIZE_B);
	memset(data, 0, SHACAL2_BLOCKSIZE_B);
	
	
	startTimer(1);
	shacal2_enc(data, key, SHACAL2_KEYSIZE);
	t = stopTimer();
	print_time_P(PSTR("\tencrypt time: "), t);
	
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
	void_fpt v[] = {testrun_nessie_shacal2enc, 
		            testrun_nessie_shacal2enc,
		            testrun_performance_shacal2enc};

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
