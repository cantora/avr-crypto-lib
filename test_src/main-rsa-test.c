/* main-rsa-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * RSA test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon/noekeon.h"
#include "noekeon/noekeon_prng.h"
#include "bigint/bigint.h"
#include "bigint/bigint_io.h"
#include "rsa.h"
#include "rsa_key_blob.h"

#include "cli.h"
#include "performance_test.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "RSA";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

rsa_ctx_t rsa_ctx;

void load_fix_rsa(void){
	load_rsa_key_blob(&rsa_ctx);
}

void rsa_print_item(bigint_t* a, PGM_P pstr){
	uint8_t *p;
	cli_putstr_P(PSTR("\r\n"));
	cli_putstr_P(pstr);
	cli_putstr_P(PSTR(": "));
	uint16_t i;
	p = a->wordv + a->length_B -1;
	for(i=0; i<a->length_B-1; ++i){
		if(i%16==0){
			cli_putstr_P(PSTR("\r\n    "));
		}
		cli_hexdump(p, 1);
		cli_putc(':');
		--p;
	}
	if(i%16==0){
		cli_putstr_P(PSTR("\r\n    "));
	}
	cli_hexdump(p, 1);
}

void print_rsa_ctx(rsa_ctx_t* ctx){
	cli_putstr_P(PSTR("\r\n === RSA context ==="));
	rsa_print_item(&(ctx->modulus), PSTR("modulus"));
	rsa_print_item(&(ctx->pubexp),  PSTR("public exponent"));
	rsa_print_item(&(ctx->privexp), PSTR("private exponent"));
}

uint8_t testmsg[128] PROGMEM = {
		0x25, 0x11, 0x1e, 0x08, 0x1c, 0x72, 0x57, 0xbf,
		0x46, 0xbb, 0x56, 0xe8, 0x0b, 0x24, 0x0d, 0x72,
		0x77, 0x11, 0x12, 0x10, 0xec, 0xb9, 0xe4, 0x6f,
		0xe0, 0x6c, 0x83, 0x65, 0xe2, 0xe5, 0x1a, 0x3c,
		0xdb, 0x6c, 0x51, 0x8d, 0xbc, 0xd0, 0x30, 0xb6,
		0xca, 0x01, 0xb4, 0x03, 0x03, 0xe9, 0x86, 0x86,
		0xfe, 0x76, 0x4e, 0xb7, 0x47, 0xb5, 0x06, 0x15,
		0x92, 0x38, 0xc2, 0x0d, 0x3c, 0xfa, 0x53, 0xe3,
		0xe3, 0x57, 0xeb, 0x34, 0xb8, 0xda, 0x7b, 0x0e,
		0x06, 0x28, 0x1d, 0x4a, 0x14, 0xcc, 0x56, 0x8d,
		0xf3, 0x0f, 0x40, 0x2e, 0x23, 0x45, 0xe7, 0xcd,
		0xc5, 0x83, 0x8f, 0xaa, 0x55, 0x55, 0x7d, 0xbd,
		0x19, 0x63, 0xbd, 0x17, 0xda, 0xfd, 0x18, 0xd1,
		0x62, 0x43, 0xc7, 0x33, 0x39, 0x2c, 0xda, 0x64,
		0x27, 0x96, 0xa0, 0x83, 0xa0, 0xf9, 0x3b, 0x1e,
		0x17, 0x71, 0x59, 0xaa, 0x43, 0x3b, 0xeb, 0x80
};

void quick_test(void){
	load_fix_rsa();

	bigint_t m,c,m_;
	uint8_t mw[rsa_ctx.modulus.length_B], cw[rsa_ctx.modulus.length_B], m_w[rsa_ctx.modulus.length_B];
	uint8_t i;

	print_rsa_ctx(&rsa_ctx);
	m.wordv = mw;
	c.wordv = cw;
	m_.wordv = m_w;
	m.length_B = rsa_ctx.modulus.length_B;
	m.info = 0;
	cli_putstr_P(PSTR("\r\ngenerating random message ..."));
/*	do{
		for(i=0; i<rsa_ctx.modulus.length_B/16; i+=1){
			random_block(&(mw[i*16]));
		}
		bigint_adjust(&m);
	}while(bigint_cmp_s(&m, &(rsa_ctx.modulus))==1);
*/
	memcpy_P(mw, testmsg, 128);
	bigint_changeendianess(&m);
	bigint_adjust(&m);
	rsa_print_item(&m,  PSTR("plain "));

	cli_putstr_P(PSTR("\r\nencrypting ..."));
	rsa_enc_bigint(&c, &m, &rsa_ctx);
	rsa_print_item(&c,  PSTR("cipher"));

	cli_putstr_P(PSTR("\r\ndecrypting ..."));
	rsa_dec_bigint(&m_, &c, &rsa_ctx);

	rsa_print_item(&m_, PSTR("plain'"));
	cli_putstr_P(PSTR("\r\n plain == plain': "));
	if(bigint_cmp_s(&m, &m_)){
		cli_putstr_P(PSTR("false"));
	}else{
		cli_putstr_P(PSTR("true"));
	}
}

void reset_prng(void){
	uint8_t buf[16];
	memset(buf, 0, 16);
	random_seed(buf);
	cli_putstr_P(PSTR("\r\nPRNG reset"));
}

void testrun_performance_bigint(void){

}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]        PROGMEM = "echo-test";
const char reset_prng_str[]       PROGMEM = "reset-prng";
const char quick_test_str[]       PROGMEM = "quick-test";
const char performance_str[]      PROGMEM = "performance";
const char echo_str[]             PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ reset_prng_str,       NULL, reset_prng                    },
	{ quick_test_str,       NULL, quick_test                    },
	{ performance_str,      NULL, testrun_performance_bigint    },
	{ echo_str,         (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                 NULL, NULL                          }
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
