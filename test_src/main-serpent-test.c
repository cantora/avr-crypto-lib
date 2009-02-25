/* main-serpent-test.c */
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
 * serpent test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "serpent.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Serpent";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void serpent_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	serpent_init(key, keysize&0xff, ctx);
}

void testrun_nessie_serpent(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(serpent_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)serpent_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)serpent_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)serpent_genctx_dummy;
	
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 256;
	nessie_bc_run();
}

void testrun_test_serpent(void){
	uint8_t key[32];
	serpent_ctx_t ctx;
	uint8_t i;
	memset(key, 0, 16);
	serpent_init(key, 128, &ctx);
	for(i=0; i<33; ++i){
		cli_putstr_P(PSTR("\r\n subkekey "));	
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR(" : "));	
		cli_hexdump(ctx.k[i], 16);
	}
}

void testrun_performance_serpent(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	serpent_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 32);
	memset(data, 0, 16);
	
	startTimer(1);
	serpent_init(key, 0, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	serpent_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	serpent_dec(data, &ctx);
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
	{ nessie_str,      NULL, testrun_nessie_serpent},
	{ test_str,        NULL, testrun_test_serpent},
	{ performance_str, NULL, testrun_performance_serpent},
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
