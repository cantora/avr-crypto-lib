/* main-tdes-test.c */
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
 * tdes test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "des.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal_tdes.h"
#include "bcal_tdes2.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "TDES";

const bcdesc_t* algolist[] PROGMEM = {
	(bcdesc_t*)&tdes_desc,
	(bcdesc_t*)&tdes2_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void tdes_init_dummy(const void* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, 8*3);
}

void tdes_enc_dummy(void* buffer, void* ctx){
	tdes_enc(buffer, buffer, ctx);
} 

void tdes_dec_dummy(void* buffer, void* ctx){
	tdes_dec(buffer, buffer, ctx);
} 

void testrun_nessie_tdes(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(8*3);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)tdes_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)tdes_dec_dummy;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)tdes_init_dummy;

	nessie_bc_run();
}


void testrun_performance_tdes(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_tdes},
	{ test_str,        NULL, testrun_nessie_tdes},
	{ performance_str, NULL, testrun_performance_tdes},
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
