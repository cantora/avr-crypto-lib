/* main-xtea-test.c */
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
/*
 * XTEA test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "xtea.h"
#include "nessie_bc_test.h"
#include "performance_test.h"
#include "cli.h"

#include <stdint.h>
#include <string.h>

char* cipher_name = "XTEA";

void xtea_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, (keysize+7)/8);
}

void xtea_enc_dummy(uint8_t* buffer, void* ctx){
	xtea_enc((uint32_t*)buffer, (uint32_t*)buffer, ctx);
}

void xtea_dec_dummy(uint8_t* buffer, void* ctx){
	xtea_dec((uint32_t*)buffer, (uint32_t*)buffer, ctx);
}

void testrun_nessie_xtea(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = 128/8;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)xtea_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)xtea_dec_dummy;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)xtea_genctx_dummy;
	
	nessie_bc_run();	
}

void testrun_performance_xtea(void){
	uint64_t t;
	uint8_t key[16], data[8];
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0,  8);
	
	startTimer(1);
	xtea_enc(data, data, key);
	t = stopTimer();
	print_time_P(PSTR("\tencrypt time: "), t);
	
	startTimer(1);
	xtea_dec(data, data, key);
	t = stopTimer();
	print_time_P(PSTR("\tdecrypt time: "), t);
	
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
	void_fpt v[] = {testrun_nessie_xtea, testrun_nessie_xtea, testrun_performance_xtea};

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
