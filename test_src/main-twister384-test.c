/* main-twister384-test.c */
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
 * twister test suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "twister-big.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

char* algo_name = "TWISTER-384";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void twister384_init_dummy(void* ctx){
	twister_big_init(ctx, 384);
}

void twister384_ctx2hash_dummy(void* buffer, void* ctx){
	twister_big_ctx2hash(buffer, ctx, 384);
}


void testrun_nessie_twister384(void){
	nessie_hash_ctx.hashsize_b  = 384;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(twister_big_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)twister384_init_dummy;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)twister_big_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)twister_big_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)twister384_ctx2hash_dummy;
	
	nessie_hash_run();
}

/******************************************************************************   
 * selftests
 ******************************************************************************/

void print_hash(void* hash){
	cli_hexdump(hash, 256/8);
	cli_putstr_P(PSTR("\r\n\t"));
	cli_hexdump((uint8_t*)hash+256/8, 128/8);
	
}

void testrun_twister384(void){
	twister384_hash_t hash;
	char* testv[]={
		"", 
		"a", 
		"abc", 
		"message digest", 
		"abcdefghijklmnopqrstuvwxyz", 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
	uint32_t i;
	
	cli_putstr_P(PSTR("\r\n=== TWISTER-384 test suit (MD5 test values) ==="));
	for(i=0; i<7; ++i){
		cli_putstr_P(PSTR("\r\n TWISTER-384 (\""));
		cli_putstr(testv[i]);
		cli_putstr_P(PSTR("\") = \r\n\t"));
		twister384(&hash, testv[i], strlen(testv[i])*8);
		print_hash(hash);
	//	return;
	}
	
	cli_putstr_P(PSTR("\r\n\r\n=== TWISTER-384 test suit (short test values) ==="));
	uint8_t stestv[]= {0x00, 0x00, 0xC0, 0xC0, 0x80, 0x48, 0x50};
	uint8_t stestl[]= {   0,    1,    2,    3,    4,    5,    6};	
	for(i=0; i<7; ++i){
		cli_putstr_P(PSTR("\r\n TWISTER-384 (\""));
		cli_hexdump(&(stestv[i]), 1);
		cli_putstr_P(PSTR("\") = \r\n\t"));
		twister384(hash, &(stestv[i]), stestl[i]);
		print_hash(hash);
	}
	
#ifdef TWISTER_LONGTEST
	cli_putstr_P(PSTR("\r\n\r\n=== TWISTER-384 test suit (long test) ==="));
	char* ltest= "abcdefghbcdefghicdefghijdefghijk"
                           "efghijklfghijklmghijklmnhijklmno";	
	twister384_ctx_t ctx;
	twister384_init(&ctx);	
	cli_putstr_P(PSTR("\r\n TWISTER-384 ( 16777216 x \""));
	cli_putstr(ltest);	
	cli_putstr_P(PSTR("\") = \r\n\t"));
	for(i=0; i<16777216; ++i){
		twister384_nextBlock(&ctx, ltest);
	}
	twister384_ctx2hash(hash, &ctx);
	print_hash(hash);
#endif
}


void testrun_performance_twister384(void){
	uint64_t t;
	char str[16];
	uint8_t data[64];
	twister_big_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 64);
	
	startTimer(1);
	twister_big_init(&ctx, 384);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	twister_big_nextBlock(&ctx, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	twister_big_lastBlock(&ctx, data, 0);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	twister_big_ctx2hash(data, &ctx, 384);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx2hash time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}


/*****************************************************************************
 * main
 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_twister384},
	{ test_str,        NULL, testrun_twister384},
	{ performance_str, NULL, testrun_performance_twister384},
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
