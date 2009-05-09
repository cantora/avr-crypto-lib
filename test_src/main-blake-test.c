/* main-blake-test.c */
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
 * blake test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "blake_small.h"
#include "blake_large.h"
#include "hfal_blake_small.h"
#include "hfal_blake_large.h"
#include "shavs.h"
#include "cli.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Blake";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_blake(void){
	nessie_hash_ctx.hashsize_b  = 224;
	nessie_hash_ctx.name = "Blake28";
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(blake28_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)blake28_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)blake28_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)blake28_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)blake28_ctx2hash;
	
	nessie_hash_run();
	
	nessie_hash_ctx.hashsize_b  = 256;
	nessie_hash_ctx.name = "Blake32";
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(blake32_ctx_t);
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)blake32_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)blake32_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)blake32_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)blake32_ctx2hash;
	
	nessie_hash_run();
}
void blake28_test(void* msg, uint32_t length_b){
	uint8_t diggest[224/8];
	cli_putstr_P(PSTR("\r\n=== Blake28 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	blake28(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 224/8, 4, 16);
}

void blake32_test(void* msg, uint32_t length_b){
	uint8_t diggest[256/8];
	cli_putstr_P(PSTR("\r\n=== Blake32 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	blake32(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 256/8, 4, 16);
}

void blake48_test(void* msg, uint32_t length_b){
	uint8_t diggest[384/8];
	cli_putstr_P(PSTR("\r\n=== Blake48 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	blake48(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 384/8, 4, 16);
}

void blake64_test(void* msg, uint32_t length_b){
	uint8_t diggest[512/8];
	cli_putstr_P(PSTR("\r\n=== Blake48 test ===\r\n message:\r\n"));
	cli_hexdump_block(msg, (length_b+7)/8, 4, 16);
	blake64(diggest, msg, length_b);
	cli_putstr_P(PSTR("\r\n diggest:\r\n"));
	cli_hexdump_block(diggest, 512/8, 4, 16);
}
void testrun_stdtest_blake(void){
	uint8_t msg1[144]; 
	memset(msg1, 0, 144);
	blake28_test("", 8);
	blake28_test(msg1, 576);
	blake32_test("", 8);
	blake32_test(msg1, 576);
	blake48_test("", 8);
	blake48_test(msg1, 1152);
	blake64_test("", 8);
	blake64_test(msg1, 1152);
}

void testshort(void){
	blake32_test("", 8);
}

void testlshort(void){
	blake64_test("", 8);
}

void performance_blake(void){
	uint64_t t;
	char str[16];
	uint8_t data[64];
	uint8_t hash[512/8];
	blake_small_ctx_t ctx;
	blake_large_ctx_t ctx2;
	
	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 64);
	
	
	startTimer(1);
	blake28_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake32_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	blake48_init(&ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake64_init(&ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	startTimer(1);
	blake_small_nextBlock(&ctx, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block (small) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake_large_nextBlock(&ctx2, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block (large) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	startTimer(1);
	blake_small_lastBlock(&ctx, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block (small) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake_large_lastBlock(&ctx2, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block (large) time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake28_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (224): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake32_ctx2hash(hash, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (256): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	blake48_ctx2hash(hash, &ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (384): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	blake64_ctx2hash(hash, &ctx2);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time (512): "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
		
	cli_putstr_P(PSTR("\r\n"));

}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&blake28_desc,
	(hfdesc_t*)&blake32_desc,
	(hfdesc_t*)&blake48_desc,
	(hfdesc_t*)&blake64_desc,
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
	{ nessie_str,          NULL, testrun_nessie_blake},
	{ test_str,            NULL, testrun_stdtest_blake},
	{ testshort_str,       NULL, testshort},
	{ testlshort_str,      NULL, testlshort},
	{ performance_str,     NULL, performance_blake},
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
	shavs_algo=(hfdesc_t*)&blake32_desc;
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
