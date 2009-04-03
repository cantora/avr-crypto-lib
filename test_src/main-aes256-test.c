/* main-aes256-test.c */
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
 * AES-256 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "aes.h"
#include "aes256_enc.h"
#include "aes256_dec.h"
#include "aes_keyschedule.h"

#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "AES-256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_aes(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 256;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(aes256_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)aes256_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)aes256_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)aes_init;
	nessie_bc_run();
}

void testrun_testkey_aes256(void){
	uint8_t key[32] = { 0x60, 0x3d, 0xeb, 0x10, 
	                    0x15, 0xca, 0x71, 0xbe, 
	                    0x2b, 0x73, 0xae, 0xf0, 
	                    0x85, 0x7d, 0x77, 0x81, 
	                    0x1f, 0x35, 0x2c, 0x07, 
	                    0x3b, 0x61, 0x08, 0xd7, 
	                    0x2d, 0x98, 0x10, 0xa3, 
	                    0x09, 0x14, 0xdf, 0xf4};
	aes256_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes256_ctx_t));
	aes256_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 32);
	for(i=0; i<15; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes(void){
	testrun_testkey_aes256();
}
/*****************************************************************************/

void testrun_performance_aes256(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	aes256_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 32);
	memset(data, 0, 16);
	
	startTimer(1);
	aes256_init(key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	aes256_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	aes256_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_performance_aes(void){
	cli_putstr_P(PSTR("\r\n -=AES Performance Test=-\r\n"));
	cli_putstr_P(PSTR("\r\n       AES-256\r\n"));
	testrun_performance_aes256();
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testkey_str[]     PROGMEM = "testkey";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_aes },
	{ test_str,        NULL, testrun_nessie_aes},
	{ testkey_str,     NULL, testrun_testkey_aes},
	{ performance_str, NULL, testrun_performance_aes},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}


