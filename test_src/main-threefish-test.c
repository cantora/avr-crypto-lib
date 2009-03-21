/* main-threefish-test.c */
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
 * threefish test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "threefish.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Threefish";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void threefish256_dummy_init(const uint8_t* key, uint16_t keysize_b, void* ctx){
	uint8_t null[16];
	memset(null, 0, 16);
	threefish256_init(key, null, ctx);
}

void testrun_nessie_threefish(void){
	nessie_bc_ctx.keysize_b = 256;
	nessie_bc_ctx.blocksize_B = 32;
	nessie_bc_ctx.ctx_size_B = sizeof(threefish256_ctx_t);
	nessie_bc_ctx.name = "Threefish256";
	nessie_bc_ctx.cipher_genctx = threefish256_dummy_init;
	nessie_bc_ctx.cipher_enc = (nessie_bc_enc_fpt)threefish256_enc;
	nessie_bc_ctx.cipher_dec = (nessie_bc_dec_fpt)threefish256_dec;
	nessie_bc_ctx.cipher_free = NULL;
	
	nessie_bc_run();
}

void testrun_stdtest_threefish256(void){
	uint8_t key[32], data[32];
	uint8_t tweak[16];
	uint8_t i;
	threefish256_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (256 bits):"));
	memset(key,  0, 32);
	memset(data, 0, 32);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	threefish256_init(key, tweak, &ctx);
	threefish256_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
	/* 
	cli_hexdump_rev(data, 8);
	cli_putc(' ');
	cli_hexdump_rev(data+8, 8);
	cli_putc(' ');
	cli_hexdump_rev(data+16, 8);
	cli_putc(' ');
	cli_hexdump_rev(data+24, 8);
	cli_putc(' ');
	*/
	/* second test */
	for(i=0; i<32; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\n\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	threefish256_init(key, tweak, &ctx);
	threefish256_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
}

void testrun_stdtest_threefish512(void){
	uint8_t key[64], data[64];
	uint8_t tweak[16];
	uint8_t i;
	threefish512_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (512 bits) :"));
	memset(key,  0, 64);
	memset(data, 0, 64);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+32, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	threefish512_init(key, tweak, &ctx);
	threefish512_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	
	for(i=0; i<64; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\n\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+32, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	threefish512_init(key, tweak, &ctx);
	threefish512_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
}

void testrun_stdtest_threefish1024(void){
	uint8_t key[128], data[128];
	uint8_t tweak[16];
	uint8_t i;
	threefish1024_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (1024 bits) :"));
	memset(key,  0, 128);
	memset(data, 0, 128);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+96, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+96, 32);
	threefish1024_init(key, tweak, &ctx);
	threefish1024_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+96, 32);
	
	for(i=0; i<128; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\n\r\nkey:    "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(key+96, 32);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump(tweak, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+96, 32);
	threefish1024_init(key, tweak, &ctx);
	threefish1024_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+32, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+64, 32);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump(data+96, 32);
	}


void testrun_stdtest_threefish(void){
	testrun_stdtest_threefish256();
	testrun_stdtest_threefish512();
	testrun_stdtest_threefish1024();
}

void testrun_performance_threefish256(void){
	uint64_t t;
	char str[16];
	uint8_t key[THREEFISH256_BLOCKSIZE_B];
	uint8_t data[THREEFISH256_BLOCKSIZE_B];
	uint8_t tweak[16];
	threefish256_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\nThreefish-256 performance:"));
	
	calibrateTimer();
	print_overhead();	
	
//	memset(key,  0, THREEFISH256_BLOCKSIZE_B);
//	memset(tweak, 0, 16);
	
	startTimer(1);
	threefish256_init(key, tweak, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	threefish256_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	cli_putstr_P(PSTR("\r\n"));	
}

void testrun_performance_threefish512(void){
	uint64_t t;
	char str[16];
	uint8_t key[THREEFISH512_BLOCKSIZE_B];
	uint8_t data[THREEFISH512_BLOCKSIZE_B];
	uint8_t tweak[16];
	threefish512_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\nThreefish-512 performance:"));
	
	calibrateTimer();
	print_overhead();	
	
//	memset(key,  0, THREEFISH512_BLOCKSIZE_B);
//	memset(tweak, 0, 16);
	
	startTimer(1);
	threefish512_init(key, tweak, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	threefish512_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	cli_putstr_P(PSTR("\r\n"));	
}

void testrun_performance_threefish1024(void){
	uint64_t t;
	char str[16];
	uint8_t key[THREEFISH1024_BLOCKSIZE_B];
	uint8_t data[THREEFISH1024_BLOCKSIZE_B];
	uint8_t tweak[16];
	threefish1024_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\nThreefish-1024 performance:"));
	
	calibrateTimer();
	print_overhead();	
	
//	memset(key,  0, THREEFISH1024_BLOCKSIZE_B);
//	memset(tweak, 0, 16);
	
	startTimer(1);
	threefish1024_init(key, tweak, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	threefish1024_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	cli_putstr_P(PSTR("\r\n"));	
}

void testrun_performance_threefish(void){
	testrun_performance_threefish256();
	testrun_performance_threefish512();
	testrun_performance_threefish1024();
}

void init_test(void){
	threefish256_ctx_t ctx;
	uint8_t key[32], tweak[16];
	memset(key, 0,32);
	memset(tweak, 0,16);
	threefish256_init(key, tweak, &ctx);
	cli_putstr_P(PSTR("\r\n ctx: \r\n\tk:"));
	cli_hexdump(ctx.k, 5*8);
	cli_putstr_P(PSTR("\r\n\tt:"));
	cli_hexdump(ctx.t, 3*8);
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char inittest_str[]    PROGMEM = "inittest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_threefish},
	{ test_str,        NULL, testrun_stdtest_threefish},
	{ inittest_str,    NULL, init_test},
	{ performance_str, NULL, testrun_performance_threefish},
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
