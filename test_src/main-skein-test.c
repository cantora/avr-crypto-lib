/* main-skein-test.c */
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

#include "skein.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Skein";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_stdtest_skein256(uint16_t outsize_b){
	uint8_t message[64];
	uint8_t hash[(outsize_b+7)/8];
	uint8_t i;
	skein256_ctx_t ctx;
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (256 bits):"));
	for(i=0; i<64; ++i)
		message[i] = 0xFF-i;
	
	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein256_init(&ctx, outsize_b);
	skein256_lastBlock(&ctx, message, 8);
	skein256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 32, 4, 16);
	skein256_init(&ctx, outsize_b);
	skein256_lastBlock(&ctx, message, 32*8);
	skein256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 64, 4, 16);
	skein256_init(&ctx, outsize_b);
	skein256_lastBlock(&ctx, message, 64*8);
	skein256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein512(uint16_t outsize_b){
	uint8_t message[128];
	uint8_t hash[(outsize_b+7)/8];
	uint8_t i;
	skein512_ctx_t ctx;
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (512 bits):"));
	for(i=0; i<128; ++i)
		message[i] = 0xFF-i;
	
	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein512_init(&ctx, outsize_b);
	skein512_lastBlock(&ctx, message, 8);
	skein512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 64, 4, 16);
	skein512_init(&ctx, outsize_b);
	skein512_lastBlock(&ctx, message, 64*8);
	skein512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 128, 4, 16);
	skein512_init(&ctx, outsize_b);
	skein512_lastBlock(&ctx, message, 128*8);
	skein512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein1024(uint16_t outsize_b){
	uint8_t message[256];
	uint8_t hash[(outsize_b+7)/8];
	uint16_t i;
	skein1024_ctx_t ctx;
		
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (1024 bits):"));
	for(i=0; i<256; ++i)
		message[i] = 0xFF-i;
	
	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein1024_init(&ctx, outsize_b);
	skein1024_lastBlock(&ctx, message, 8);
	skein1024_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 128, 4, 16);
	skein1024_init(&ctx, outsize_b);
	skein1024_lastBlock(&ctx, message, 128*8);
	skein1024_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
	
	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 256, 4, 16);
	skein1024_init(&ctx, outsize_b);
	skein1024_lastBlock(&ctx, message, 256*8);
	skein1024_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein(void){
	testrun_stdtest_skein256(256);
	testrun_stdtest_skein512(512);
	testrun_stdtest_skein1024(1024);
}
/*
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
*/
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
//	{ nessie_str,      NULL, testrun_nessie_noekeon},
	{ test_str,        NULL, testrun_stdtest_skein},
//	{ performance_str, NULL, testrun_performance_threefish},
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
