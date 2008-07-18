/* main-noekeon-test.c */
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
 * serpent test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "noekeon.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* cipher_name = "Noekeon";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void noekeon_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	noekeon_init(key, ctx);
}

void testrun_nessie_noekeon_indirect(void){
	char str[strlen(cipher_name)+10];
	strcpy(str, cipher_name);
	strcat(str, "-indirect");
	
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = str;
	nessie_bc_ctx.ctx_size_B  = sizeof(noekeon_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)noekeon_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)noekeon_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)noekeon_genctx_dummy;
	
	nessie_bc_run();
}

void noekeon_genctx_dummy_direct(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, 16);
}

void testrun_nessie_noekeon_direct(void){
	char str[strlen(cipher_name)+10];
	strcpy(str, cipher_name);
	strcat(str, "-Direct");
	
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = str;
	nessie_bc_ctx.ctx_size_B  = sizeof(noekeon_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)noekeon_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)noekeon_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)noekeon_genctx_dummy_direct;
	
	nessie_bc_run();
}

void testrun_nessie_noekeon(void){
	testrun_nessie_noekeon_direct();
	testrun_nessie_noekeon_indirect();
}


void testrun_stdtest_rundirect(void* data, void* key){
	uart_putstr_P(PSTR("\r\n                     "));
	uart_putstr_P(PSTR("k = "));
	uart_hexdump(key,16);
	
	uart_putstr_P(PSTR("\r\n                     "));
	uart_putstr_P(PSTR("a = "));
	uart_hexdump(data,16);
	
	noekeon_enc(data, key);
	uart_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	uart_hexdump(data,16);
	
	noekeon_dec(data, key);
	uart_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	uart_hexdump(data,16);
	uart_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_runindirect(void* data, void* key){
	noekeon_ctx_t ctx;
	uart_putstr_P(PSTR("\r\n                     "));
	uart_putstr_P(PSTR("k = "));
	uart_hexdump(key,16);
	
	uart_putstr_P(PSTR("\r\n                     "));
	uart_putstr_P(PSTR("a = "));
	uart_hexdump(data,16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	uart_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	uart_hexdump(data,16);
	
	noekeon_dec(data, &ctx);
	uart_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	uart_hexdump(data,16);
	uart_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_noekeon(void){
	uint8_t key[16], data[16];
	uint8_t key3[16];
	noekeon_ctx_t ctx;
	
	uart_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Indirect-Key Mode:\r\n"));
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	testrun_stdtest_runindirect(data, key);
	
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	testrun_stdtest_runindirect(data, key);
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	memcpy(key3, data, 16);
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	testrun_stdtest_runindirect(data, key3);
	
	uart_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Direct-Key Mode:\r\n"));
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	testrun_stdtest_rundirect(data, key);
	
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	testrun_stdtest_rundirect(data, key);
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	noekeon_enc(data, key);
	memcpy(key3, data, 16);
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	noekeon_enc(data, key);
	testrun_stdtest_rundirect(data, key3);
	
}

void testrun_performance_noekeon(void){
	uint64_t t;
	char str[16];
	uint8_t key[16], data[16];
	noekeon_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	noekeon_init(key, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	startTimer(1);
	noekeon_enc(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	startTimer(1);
	noekeon_dec(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
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

	PGM_P    u   = PSTR("nessie\0test\0direct\0indirect\0performance\0");
	void_fpt v[] = {testrun_nessie_noekeon, 
		            testrun_stdtest_noekeon,
		            testrun_nessie_noekeon_direct, 
		            testrun_nessie_noekeon_indirect,  
		            testrun_performance_noekeon};

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

