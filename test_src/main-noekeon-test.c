/* main-noekeon-test.c */
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
/*
 * noekeon test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart_i.h"
#include "debug.h"

#include <noekeon.h>
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Noekeon";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void noekeon_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	noekeon_init(key, ctx);
}

void testrun_nessie_noekeon_indirect(void){
	char str[strlen(algo_name)+10];
	strcpy(str, algo_name);
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
	char str[strlen(algo_name)+10];
	strcpy(str, algo_name);
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
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("k = "));
	cli_hexdump(key,16);
	
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("a = "));
	cli_hexdump(data,16);
	
	noekeon_enc(data, key);
	cli_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	cli_hexdump(data,16);
	
	noekeon_dec(data, key);
	cli_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	cli_hexdump(data,16);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_runindirect(void* data, void* key){
	noekeon_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("k = "));
	cli_hexdump(key,16);
	
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("a = "));
	cli_hexdump(data,16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	cli_hexdump(data,16);
	
	noekeon_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	cli_hexdump(data,16);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_noekeon(void){
	uint8_t key[16], data[16];
	uint8_t key3[16];
	noekeon_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Indirect-Key Mode:\r\n"));
	
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
	
	cli_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Direct-Key Mode:\r\n"));
	
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
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	noekeon_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	noekeon_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	cli_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char direct_str[]      PROGMEM = "direct";
const char indirect_str[]    PROGMEM = "indirect";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_noekeon},
	{ test_str,        NULL, testrun_stdtest_noekeon},
	{ direct_str,      NULL, testrun_nessie_noekeon_direct},
	{ indirect_str,    NULL, testrun_nessie_noekeon_indirect},
	{ performance_str, NULL, testrun_performance_noekeon},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
