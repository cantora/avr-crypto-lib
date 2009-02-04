/* main-sha256-test.c */
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
 * SHA-256 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "sha256.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "shavs.h"
#include "hfal_sha256.h"
#include "dump.h"

char* algo_name = "SHA-256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void sha256_next_dummy(void* buffer, void* ctx){
	sha256_nextBlock(ctx, buffer);
}

void sha256_last_dummy(void* buffer, uint16_t size_b, void* ctx){
	sha256_lastBlock(ctx, buffer, size_b);
}

void testrun_nessie_sha256(void){
	nessie_hash_ctx.hashsize_b  = 256;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(sha256_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)sha256_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)sha256_next_dummy;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)sha256_last_dummy;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)sha256_ctx2hash;
	
	nessie_hash_run();
}

void testrun_performance_sha256(void){
	uint64_t t;
	char str[16];
	uint8_t data[32];
	sha256_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 32);
	
	startTimer(1);
	sha256_init(&ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	sha256_nextBlock(&ctx, data);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tone-block time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	sha256_lastBlock(&ctx, data, 0);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tlast block time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char dump_str[]        PROGMEM = "dump";

const hfdesc_t* algo_list[] PROGMEM = {
	(hfdesc_t*)&sha256_desc,
	NULL
};

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_sha256},
	{ test_str,            NULL, testrun_nessie_sha256},
	{ performance_str,     NULL, testrun_performance_sha256},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ dump_str,        (void*)1, (void_fpt)dump},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	uart_putstr("\r\n");
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	algolist=algo_list;
	for(;;){
		uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		uart_putstr(algo_name);
		uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
