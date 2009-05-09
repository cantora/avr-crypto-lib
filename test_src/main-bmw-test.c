/* main-bmw-test.c */
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
 * BlueMidnightWish test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "bmw_small.h"
#include "bmw_large.h"
#include "cli.h"
#include "hfal_bmw_small.h"
#include "hfal_bmw_large.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "BlueMidnightWish";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void performance_bmw(void){
	uint64_t t;
	char str[16];
	uint8_t data[128];
	uint8_t hash[512/8];
	bmw_small_ctx_t ctx1;
	bmw_large_ctx_t ctx2;

	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 64);
	
	startTimer(1);
	bmw224_init(&ctx1);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw256_init(&ctx1);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw384_init(&ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw512_init(&ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	startTimer(1);
	bmw_small_nextBlock(&ctx1, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block (small) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw_large_nextBlock(&ctx2, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block (large) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw_small_lastBlock(&ctx1, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block (small) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw_large_lastBlock(&ctx2, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block (large) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw224_ctx2hash(hash, &ctx1);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw256_ctx2hash(hash, &ctx1);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw384_ctx2hash(hash, &ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	bmw512_ctx2hash(hash, &ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_nessie_bmw(void){
	nessie_hash_ctx.hashsize_b  = 224;
	nessie_hash_ctx.name = "BlueMidnigthWish-224";
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(bmw224_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)bmw224_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)bmw224_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)bmw224_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)bmw224_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 256;
	nessie_hash_ctx.name = "BlueMidnigthWish-256";
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(bmw256_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)bmw256_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)bmw256_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)bmw256_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)bmw256_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 384;
	nessie_hash_ctx.name = "BlueMidnigthWish-384";
	nessie_hash_ctx.blocksize_B = 1024/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(bmw384_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)bmw384_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)bmw384_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)bmw384_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)bmw384_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 512;
	nessie_hash_ctx.name = "BlueMidnigthWish-512";
	nessie_hash_ctx.blocksize_B = 1024/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(bmw512_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)bmw512_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)bmw512_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)bmw512_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)bmw512_ctx2hash;
	
	nessie_hash_run();
}
void bmw224_test(void* msg, uint32_t length_b){
	uint8_t diggest[224/8];
	cli_putstr_P(PSTR("\r\n=== BMW224 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	bmw224(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 224/8, 4, 16);
}

void bmw256_test(void* msg, uint32_t length_b){
	uint8_t diggest[256/8];
	cli_putstr_P(PSTR("\r\n=== BMW256 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	bmw256(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 256/8, 4, 16);
}

void bmw384_test(void* msg, uint32_t length_b){
	uint8_t diggest[384/8];
	cli_putstr_P(PSTR("\r\n=== BMW384 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	bmw384(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 384/8, 4, 16);
}

void bmw512_test(void* msg, uint32_t length_b){
	uint8_t diggest[512/8];
	cli_putstr_P(PSTR("\r\n=== BMW512 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	bmw512(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 512/8, 4, 16);
}

void testrun_stdtest_bmw(void){
	char* msg0 = "abc";
	char* msg1 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"; 
	bmw224_test(msg0, strlen(msg0)*8);
	bmw224_test(msg1, strlen(msg1)*8);
	bmw256_test(msg0, strlen(msg0)*8);
	bmw256_test(msg1, strlen(msg1)*8);
	bmw384_test(msg0, strlen(msg0)*8);
	bmw384_test(msg1, strlen(msg1)*8);
	bmw512_test(msg0, strlen(msg0)*8);
	bmw512_test(msg1, strlen(msg1)*8);
}

void testshort(void){
	char* msg0 = "abc";
	bmw224_test(msg0, strlen(msg0)*8);
}

void testlshort(void){
	char* msg0 = "abc";
	bmw384_test(msg0, strlen(msg0)*8);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&bmw224_desc,
	(hfdesc_t*)&bmw256_desc,
	(hfdesc_t*)&bmw384_desc,
	(hfdesc_t*)&bmw512_desc,
	NULL
};

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testshort_str[]   PROGMEM = "short";
const char testlshort_str[]  PROGMEM = "lshort";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_bmw},
	{ test_str,            NULL, testrun_stdtest_bmw},
	{ testshort_str,       NULL, testshort},
	{ testlshort_str,      NULL, testlshort},
	{ performance_str,     NULL, performance_bmw},
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
	shavs_algo=(hfdesc_t*)&bmw256_desc;
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
