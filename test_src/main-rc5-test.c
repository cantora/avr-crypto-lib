/* main-rc5-test.c */
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
 * rc5 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart_i.h"
#include "debug.h"

#include "rc5.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define RC5_ROUNDS 12
char* algo_name = "RC5-32/12/16";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void rc5_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	rc5_init(key, keysize_b, RC5_ROUNDS, ctx);
}

void testrun_nessie_rc5(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(rc5_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)rc5_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)rc5_dec;
	nessie_bc_ctx.cipher_free = (nessie_bc_free_fpt)rc5_free;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)rc5_genctx_dummy;
	
	nessie_bc_run();
}


void testrun_performance_rc5(void){
	uint64_t t;
	char str[16];
	uint8_t key[16], data[16];
	rc5_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	rc5_init(key, 128, RC5_ROUNDS, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	rc5_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	rc5_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	rc5_free(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tfree time: "));
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
	{ nessie_str,      NULL, testrun_nessie_rc5 },
	{ test_str,        NULL, testrun_nessie_rc5},
	{ performance_str, NULL, testrun_performance_rc5},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
