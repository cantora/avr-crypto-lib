/* main-aes-test.c */
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
 * AES test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "aes/aes.h"

#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"
#include "dump.h"

#include "bcal_aes128.h"
#include "bcal_aes192.h"
#include "bcal_aes256.h"
#include "bcal-cbc.h"
#include "bcal-cfb_byte.h"
#include "bcal-cfb_bit.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

char* algo_name = "AES";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_aes(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(aes128_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)aes128_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)aes128_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)aes_init;
	nessie_bc_run();

	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_ctx.ctx_size_B  = sizeof(aes192_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)aes192_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)aes192_dec;
	nessie_bc_run();

	nessie_bc_ctx.keysize_b   = 256;
	nessie_bc_ctx.ctx_size_B  = sizeof(aes256_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)aes256_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)aes256_dec;
	nessie_bc_run();
}

void testrun_test_aes(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c };
	uint8_t data[16] = { 0x32, 0x43, 0xf6, 0xa8,
	                     0x88, 0x5a, 0x30, 0x8d,
	                     0x31, 0x31, 0x98, 0xa2,
	                     0xe0, 0x37, 0x07, 0x34 };
	aes128_ctx_t ctx;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n cipher test (FIPS 197):\r\n key:        "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	aes128_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n ciphertext: "));
	cli_hexdump(data, 16);
	aes128_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	cli_putstr(PSTR("\r\n testing bcal:"));
	bcgen_ctx_t bcal_ctx;
	uint8_t r;
	r = bcal_cipher_init(&aes128_desc, key, 128, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n init = 0x"));
	cli_hexdump(&r, 1);

	bcal_cipher_enc(data, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n ciphertext: "));
	cli_hexdump(data, 16);
	bcal_cipher_dec(data, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	bcal_cipher_free(&bcal_ctx);
}

void testrun_testkey_aes128(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c};
	aes128_ctx_t ctx;
	uint8_t i;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 16);
	for(i=0; i<11; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes192(void){
	uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7,
	                    0xda, 0x0e, 0x64, 0x52,
	                    0xc8, 0x10, 0xf3, 0x2b,
	                    0x80, 0x90, 0x79, 0xe5,
	                    0x62, 0xf8, 0xea, 0xd2,
	                    0x52, 0x2c, 0x6b, 0x7b};
	aes192_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes192_ctx_t));
	aes192_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 24);
	for(i=0; i<13; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}


void testrun_testkey_aes256(void){
	uint8_t key[32] = { 0x60, 0x3d, 0xeb, 0x10,
	                    0x15, 0xca, 0x71, 0xbe,
	                    0x2b, 0x73, 0xae, 0xf0,
	                    0x85, 0x7d, 0x77, 0x81,
	                    0x1f, 0x35, 0x2c, 0x07,
	                    0x3b, 0x61, 0x08, 0xd7,
	                    0x2d, 0x98, 0x10, 0xa3,
	                    0x09, 0x14, 0xdf, 0xf4};
	aes256_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes256_ctx_t));
	aes256_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 32);
	for(i=0; i<15; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes(void){
	testrun_testkey_aes128();
	testrun_testkey_aes192();
	testrun_testkey_aes256();
}

uint8_t modes_key[]   PROGMEM={ 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		                        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                              };
uint8_t modes_iv[]    PROGMEM={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                              };
uint8_t modes_plain[] PROGMEM={ 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		                        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		                        /* --- */
		                        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		                        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		                        /* --- */
		                        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
		                        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		                        /* --- */
		                        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
		                        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
                              };

void testrun_aes128_cbc(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cbc_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CBC-TEST **"));
	r = bcal_cbc_init(&aes128_desc, key, 128, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 8);
	if(r)
		return;
	bcal_cbc_encMsg(iv, plain, 4, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 4*128/8, 4, 8);
	bcal_cbc_decMsg(iv, plain, 4, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 4*128/8, 4, 8);
	bcal_cbc_free(&ctx);
}

void testrun_aes128_cfb8(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cfb_B_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CFB8-TEST **"));
	r = bcal_cfb_B_init(&aes128_desc, key, 128, 8, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 8);
	if(r)
		return;
	bcal_cfb_B_encMsg(iv, plain, 64, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 64, 4, 8);

	bcal_cfb_B_decMsg(iv, plain, 64, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 64, 4, 8);

	bcal_cfb_B_free(&ctx);

}

void testrun_aes128_cfb1(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cfb_b_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CFB1-TEST **"));
	r = bcal_cfb_b_init(&aes128_desc, key, 128, 1, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 2, 4, 8);
	if(r)
		return;
	uint8_t i, bit_offset, byte_offset;
	bcal_cfb_b_loadIV(iv, &ctx);
	for(i=0; i<16; ++i){
		byte_offset = i/8;
		bit_offset = i&7;
		cli_putstr_P(PSTR("\r\n  plain bit:   "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
		bcal_cfb_b_encNext(plain+byte_offset, bit_offset, &ctx);
		cli_putstr_P(PSTR("\r\n  cipher bit:  "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
	}
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 2, 4, 8);

	bcal_cfb_b_loadIV(iv, &ctx);
	for(i=0; i<16; ++i){
		byte_offset = i/8;
		bit_offset = i&7;
		cli_putstr_P(PSTR("\r\n  plain bit:   "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
		bcal_cfb_b_decNext(plain+byte_offset, bit_offset, &ctx);
		cli_putstr_P(PSTR("\r\n  cipher bit:  "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
	}
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 2, 4, 8);


	bcal_cfb_b_encMsg(iv, plain, 0, 64*8, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 64, 4, 8);

	bcal_cfb_b_decMsg(iv, plain, 0, 64*8, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 64, 4, 8);

	bcal_cfb_b_free(&ctx);

}
/*****************************************************************************/

void testrun_performance_aes128(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	aes128_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 32);
	memset(data, 0, 16);

	startTimer(1);
	aes128_init(key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes128_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes128_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}


void testrun_performance_aes192(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	aes192_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 32);
	memset(data, 0, 16);

	startTimer(1);
	aes192_init(key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes192_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes192_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}


void testrun_performance_aes256(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	aes256_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 32);
	memset(data, 0, 16);

	startTimer(1);
	aes256_init(key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes256_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);


	startTimer(1);
	aes256_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}

void testrun_performance_aes(void){
	cli_putstr_P(PSTR("\r\n -=AES Performance Test=-\r\n"));
	cli_putstr_P(PSTR("\r\n       AES-128\r\n"));
	testrun_performance_aes128();
	cli_putstr_P(PSTR("\r\n       AES-192\r\n"));
	testrun_performance_aes192();
	cli_putstr_P(PSTR("\r\n       AES-256\r\n"));
	testrun_performance_aes256();
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testkey_str[]     PROGMEM = "testkey";
const char testcbc_str[]     PROGMEM = "testcbc";
const char testcfb8_str[]    PROGMEM = "testcfb8";
const char testcfb1_str[]    PROGMEM = "testcfb1";
const char performance_str[] PROGMEM = "performance";
const char dump_str[]        PROGMEM = "dump";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_aes },
	{ test_str,        NULL, testrun_test_aes},
	{ testkey_str,     NULL, testrun_testkey_aes},
	{ testcbc_str,     NULL, testrun_aes128_cbc},
	{ testcfb8_str,    NULL, testrun_aes128_cfb8},
	{ testcfb1_str,    NULL, testrun_aes128_cfb1},
	{ performance_str, NULL, testrun_performance_aes},
	{ dump_str,    (void*)1, (void_fpt)dump},
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

