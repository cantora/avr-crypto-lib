/* main-skipjack-test.c */
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
 * skipjack test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "skipjack.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


char* algo_name = "Skipjack";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void skipjack_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, 10);
}

void testrun_nessie_skipjack(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   =  80;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = 10;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)skipjack_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)skipjack_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)skipjack_genctx_dummy;
	
	nessie_bc_run();
}


void testrun_performance_skipjack(void){
	uint64_t t;
	char str[16];
	uint8_t key[10], data[8];
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 10);
	memset(data, 0,  8);
	
	startTimer(1);
	skipjack_enc(data, key);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	skipjack_dec(data, key);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	cli_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	cli_putstr("\r\n==testy-encrypt==\r\n key: ");
	cli_hexdump(key,10);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,8);
	skipjack_enc(block,key);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,8);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	cli_putstr("\r\n==testy-decrypt==\r\n key: ");
	cli_hexdump(key,10);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,8);
	skipjack_dec(block,key);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,8);
}

void testrun_skipjack(void){
	uint8_t key[2][10]={
	                     { 0x00, 0x99, 0x88, 0x77, 0x66,
	                       0x55, 0x44, 0x33, 0x22, 0x11 },
	                     { 0x11, 0x22, 0x33, 0x44, 0x55,
	                       0x66, 0x77, 0x88, 0x99, 0x00 }
					   };   
	                 
	uint8_t data[2][8]={
		                 { 0x33, 0x22, 0x11, 0x00, 
		                   0xdd, 0xcc, 0xbb, 0xaa },
		                 { 0xaa, 0xbb, 0xcc, 0xdd,
		                   0x00, 0x11, 0x22, 0x33 }
					   };
	testencrypt(data[0],key[0]);
	testdecrypt(data[0],key[0]);
	testencrypt(data[1],key[1]);
	testdecrypt(data[1],key[1]);	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_skipjack},
	{ test_str,        NULL, testrun_skipjack},
	{ performance_str, NULL, testrun_performance_skipjack},
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
