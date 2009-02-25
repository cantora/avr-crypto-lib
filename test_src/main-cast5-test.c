/* main-cast5-test.c */
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
 * cast5 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include <cast5.h>
#include "nessie_bc_test.h"
#include "performance_test.h"
#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "cast-128 (cast5)";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_cast5(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(cast5_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)cast5_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)cast5_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)cast5_init;
	
	nessie_bc_run();
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void cast5_ctx_dump(cast5_ctx_t *s){
	uint8_t i;
	cli_putstr("\r\n==== cast5_ctx_dump ====\r\n shortkey: ");
	cli_putstr(s->shortkey?"yes":"no");
	for(i=0;i<16;++i){
		uint8_t r;
		cli_putstr("\r\n Km"); cli_hexdump(&i, 1); cli_putc(':');
		cli_hexdump(&(s->mask[i]), 4);
		cli_putstr("\r\n Kr"); cli_hexdump(&i, 1); cli_putc(':');
		r = (s->rotl[i/2]);
		if (i&0x01) r >>= 4;
		r &= 0xf;
		r += (s->roth[i>>3]&(1<<(i&0x7)))?0x10:0x00;
		cli_hexdump(&r, 1);
	}
}


void test_encrypt(uint8_t *block, uint8_t *key, uint8_t keylength, bool print){
	cast5_ctx_t s;
	if (print){
		cli_putstr("\r\nCAST5:\r\n key:\t");
		cli_hexdump(key, keylength/8);
		cli_putstr("\r\n plaintext:\t");
		cli_hexdump(block, 8);
	}
	cast5_init(key, keylength, &s);
	cast5_enc(block, &s);
	if (print){
		cli_putstr("\r\n ciphertext:\t");
		cli_hexdump(block, 8);
	}
} 

void test_decrypt(uint8_t *block, uint8_t *key, uint8_t keylength, bool print){
	cast5_ctx_t s;
	if (print){
		cli_putstr("\r\nCAST5:\r\n key:\t");
		cli_hexdump(key, keylength/8);
		cli_putstr("\r\n ciphertext:\t");
		cli_hexdump(block, 8);
	}
	cast5_init(key, keylength, &s);
	cast5_dec(block, &s);
	if (print){
		cli_putstr("\r\n plaintext:\t");
		cli_hexdump(block, 8);
	}
} 

void testrun_cast5(void){
	uint8_t block[8];
	uint8_t key[16];
	uint8_t *tda = (uint8_t*)"\x01\x23\x45\x67\x89\xAB\xCD\xEF",
	        *tka = (uint8_t*)"\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9A";
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 128, true);
	test_decrypt(block, key, 128, true);
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 80, true);
	test_decrypt(block, key, 80, true);
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 40, true);
	test_decrypt(block, key, 40, true);
	
/**** long test *****/
	cli_putstr("\r\nmaintance-test");
	uint8_t a[16]= {0x01, 0x23, 0x45, 0x67, 0x12,
				    0x34, 0x56, 0x78, 0x23, 0x45, 
				    0x67, 0x89, 0x34, 0x56, 0x78, 
				    0x9A}, 
			b[16]= {0x01, 0x23, 0x45, 0x67, 0x12,
				    0x34, 0x56, 0x78, 0x23, 0x45, 
				    0x67, 0x89, 0x34, 0x56, 0x78, 
				    0x9A};
	uint32_t i;
	for(i=0;i<1000000; ++i){
		test_encrypt(&(a[0]), &(b[0]), 128, false);
		test_encrypt(&(a[8]), &(b[0]), 128, false);
		test_encrypt(&(b[0]), &(a[0]), 128, false);
		test_encrypt(&(b[8]), &(a[0]), 128, false);
		if ((i&0x000000ff) == 0){
			
			cli_hexdump(&i, 4);
		}
	}
	cli_putstr("\r\na = "); cli_hexdump(a, 16);
	cli_putstr("\r\nb = "); cli_hexdump(b, 16);

}

void testrun_performance_cast5(void){
	uint64_t t;
	char str[6];
	uint8_t key[16], data[16];
	cast5_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	cast5_init(key, 128, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	cast5_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	cast5_dec(data, &ctx);
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
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_cast5},
	{ test_str,        NULL, testrun_cast5},
	{ performance_str, NULL, testrun_performance_cast5},
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
