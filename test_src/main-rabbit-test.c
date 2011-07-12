/* main-rabbit-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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


#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "rabbit.h"
#include "cli.h"
#include "performance_test.h"

#include "scal_rabbit.h"
#include "scal-basic.h"
#include "scal-nessie.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "Rabbit";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void test_vector(void* key, void* iv){
	rabbit_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n testing with key: "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n              iv:  "));
	if(iv){
		cli_hexdump(iv, 8);
	}else{
		cli_putstr_P(PSTR("[no iv]"));
	}
	rabbit_init(key, 128, iv, &ctx);
	cli_putstr_P(PSTR("\r\n S[0]: "));
	rabbit_gen(&ctx);
	cli_hexdump(ctx.buffer, 16);
	cli_putstr_P(PSTR("\r\n S[1]: "));
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
	cli_hexdump(ctx.buffer, 16);
	cli_putstr_P(PSTR("\r\n S[2]: "));
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
	cli_hexdump(ctx.buffer, 16);
//	dump_ctx(&ctx);
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
//	dump_ctx(&ctx);
	cli_putstr_P(PSTR("\r\n S[3]: "));
	cli_hexdump(ctx.buffer, 16);
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
	cli_putstr_P(PSTR("\r\n S[4]: "));
	cli_hexdump(ctx.buffer, 16);
	cli_putstr_P(PSTR("\r\n"));
}

void nessie_first(void){
	uint8_t key[16];
    uint8_t iv[8];

	memset(iv, 0, 8);
	memset(key, 0, 16);
	key[0] = 0x80;
	test_vector(key, iv);
	key[0] = 0x00;

	key[15] = 0x80;
	test_vector(key, iv);

}


const uint8_t spec_key1[] PROGMEM = {
	0x91, 0x28, 0x13, 0x29, 0x2E, /* 0xED */ 0x3D, 0x36, 0xFE,
	0x3B, 0xFC, 0x62, 0xF1, 0xDC, 0x51, 0xC3, 0xAC
};

const uint8_t spec_key2[] PROGMEM = {
	0x83, 0x95, 0x74, 0x15, 0x87, 0xE0, 0xC7, 0x33,
	0xE9, 0xE9, 0xAB, 0x01, 0xC0, 0x9B, 0x00, 0x43
};

const uint8_t spec_iv1[] PROGMEM = {
	0xC3, 0x73, 0xF5, 0x75, 0xC1, 0x26, 0x7E, 0x59
};

const uint8_t spec_iv2[] PROGMEM = {
	0xA6, 0xEB, 0x56, 0x1A, 0xD2, 0xF4, 0x17, 0x27
};

void spec_test(void){
	uint8_t key[16];
    uint8_t iv[8];

	memset(key, 0, 16);
	test_vector(key, NULL);
	memcpy_P(key, spec_key1, 16);
	test_vector(key, NULL);
	memcpy_P(key, spec_key2, 16);
	test_vector(key, NULL);


	memset(key, 0, 16);
	memset(iv, 0, 8);
	test_vector(key, iv);
	memcpy_P(iv,  spec_iv1,   8);
	test_vector(key, iv);
	memcpy_P(iv,  spec_iv2,   8);
	test_vector(key, iv);
}


void testrun_nessie_rabbit(void){
	scal_nessie_set_estream(1);
	scal_nessie_run(&rabbit_desc);
}



void testrun_performance_rabbit(void){
	uint64_t t;
	char str[16];
	uint8_t key[16];
	rabbit_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 16);

	startTimer(1);
	rabbit_init(key, 128, NULL, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	rabbit_gen(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char first_str[]       PROGMEM = "first";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_rabbit},
	{ performance_str, NULL, testrun_performance_rabbit},
	{ first_str,       NULL, nessie_first},
	{ test_str,        NULL, spec_test},
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

