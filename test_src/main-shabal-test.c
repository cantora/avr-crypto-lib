/* main-shabal-test.c */
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
 * shabal test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "shabal.h"
#include "cli.h"
#include "hfal_shabal.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Shabal";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_stdtest_shabal192(void* msg, uint16_t size_b){
	uint8_t hash[192/8];
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Shabal (192 bits):"));

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(msg, (size_b+7)/8, 4, 16);
	shabal192(hash, msg, size_b);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, 192/8, 4, 16);
}

void testrun_stdtest_shabal224(void* msg, uint16_t size_b){
	uint8_t hash[224/8];
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Shabal (224 bits):"));

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(msg, (size_b+7)/8, 4, 16);
	shabal224(hash, msg, size_b);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, 224/8, 4, 16);
}

void testrun_stdtest_shabal256(void* msg, uint16_t size_b){
	uint8_t hash[256/8];
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Shabal (256 bits):"));

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(msg, (size_b+7)/8, 4, 16);
	shabal256(hash, msg, size_b);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, 256/8, 4, 16);
}

void testrun_stdtest_shabal384(void* msg, uint16_t size_b){
	uint8_t hash[384/8];
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Shabal (384 bits):"));

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(msg, (size_b+7)/8, 4, 16);
	shabal384(hash, msg, size_b);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, 384/8, 4, 16);
}

void testrun_stdtest_shabal512(void* msg, uint16_t size_b){
	uint8_t hash[512/8];
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Shabal (512 bits):"));

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(msg, (size_b+7)/8, 4, 16);
	shabal512(hash, msg, size_b);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, 512/8, 4, 16);
}

void testrun_stdtest_shabal(void){
	uint8_t ma[64];
	char*   mb= "abcdefghijklmnopqrstuvwxyz-"
	            "0123456789-"
			    "ABCDEFGHIJKLMNOPQRSTUVWXYZ-"
                "0123456789-"
				"abcdefghijklmnopqrstuvwxyz";

	memset(ma, 0, 64);
	testrun_stdtest_shabal192(ma, 64*8);
	testrun_stdtest_shabal192(mb, strlen(mb)*8);
	testrun_stdtest_shabal224(ma, 64*8);
	testrun_stdtest_shabal224(mb, strlen(mb)*8);
	testrun_stdtest_shabal256(ma, 64*8);
	testrun_stdtest_shabal256(mb, strlen(mb)*8);
	testrun_stdtest_shabal384(ma, 64*8);
	testrun_stdtest_shabal384(mb, strlen(mb)*8);
	testrun_stdtest_shabal512(ma, 64*8);
	testrun_stdtest_shabal512(mb, strlen(mb)*8);
}

void performance_shabal(void){
	uint64_t t;
	char str[16];
	uint8_t data[64];
	uint8_t hash[512/8];
	shabal_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 64);
	
	startTimer(1);
	shabal192_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (192): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal192_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal192_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal192_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal192_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	startTimer(1);
	shabal_nextBlock(&ctx, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	shabal_lastBlock(&ctx, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal192_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (192): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal224_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal256_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal384_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	shabal512_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	cli_putstr_P(PSTR("\r\n"));

}

void testrun_nessie_shabal(void){
	nessie_hash_ctx.hashsize_b  = 192;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(shabal_ctx_t);
	nessie_hash_ctx.name = "Shabal-192";
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)shabal192_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)shabal_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)shabal_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)shabal192_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 224;
	nessie_hash_ctx.name = "Shabal-224";
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)shabal224_init;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)shabal224_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 256;
	nessie_hash_ctx.name = "Shabal-256";
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)shabal256_init;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)shabal256_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 384;
	nessie_hash_ctx.name = "Shabal-384";
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)shabal384_init;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)shabal384_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 512;
	nessie_hash_ctx.name = "Shabal-512";
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)shabal512_init;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)shabal512_ctx2hash;
	
	nessie_hash_run();
	
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&shabal192_desc,
	(hfdesc_t*)&shabal224_desc,
	(hfdesc_t*)&shabal256_desc,
	(hfdesc_t*)&shabal384_desc,
	(hfdesc_t*)&shabal512_desc,	
	NULL
};

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char ztest_str[]       PROGMEM = "zerotest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_shabal},
	{ test_str,            NULL, testrun_stdtest_shabal},
	{ performance_str,     NULL, performance_shabal},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&shabal256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		
		cmd_interface(cmdlist);
	}
}	
