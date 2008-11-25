/* nessie_hash_test.c */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for hashes
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_hash_test.h"
#include "nessie_common.h"
#include "uart.h"

nessie_hash_ctx_t nessie_hash_ctx;

static
void ascii_hash(char* data, char* desc){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint16_t sl;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr(desc);
	nessie_hash_ctx.hash_init(ctx);
	sl = strlen(data);
	while(sl>=nessie_hash_ctx.blocksize_B){
		nessie_hash_ctx.hash_next(data, ctx);
		data += nessie_hash_ctx.blocksize_B;
		sl   -= nessie_hash_ctx.blocksize_B;
	}
	nessie_hash_ctx.hash_last(data, sl*8, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

// message=1 million times "a"

static
void amillion_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	uint32_t n=1000000LL;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr_P(PSTR("1 million times \"a\""));
	memset(block, 'a', nessie_hash_ctx.blocksize_B);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B){
		nessie_hash_ctx.hash_next(block, ctx);
		n    -= nessie_hash_ctx.blocksize_B;
	}
	nessie_hash_ctx.hash_last(block, n*8, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}


static
void zero_hash(uint16_t n){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	
	uart_putstr_P(PSTR("\r\n                       message="));
	if(n>=10000)
		uart_putc('0'+n/10000);
	if(n>=1000)
		uart_putc('0'+(n/1000)%10);
	if(n>=100)
		uart_putc('0'+(n/100)%10);
	if(n>=10)
		uart_putc('0'+(n/10)%10);
	uart_putc('0'+n%10);
	uart_putstr_P(PSTR(" zero bits"));
	
	memset(block, 0, nessie_hash_ctx.blocksize_B); 
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

static
void one_in512_hash(uint16_t pos){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	uint16_t n=512;
	char* tab[8]={"80", "40", "20", "10", 
	              "08", "04", "02", "01" };

	pos&=511;
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr_P(PSTR("512-bit string: "));
	if((pos/8) >=10){
		uart_putc('0'+(pos/8/10)%10);
	} else {
		uart_putc(' ');
	}
	uart_putc('0'+(pos/8)%10);
	uart_putstr_P(PSTR("*00,"));
	uart_putstr(tab[pos&7]);
	uart_putc(',');
	if(63-(pos/8) >=10){
		uart_putc('0'+((63-pos/8)/10)%10);
	} else {
		uart_putc(' ');
	}
	uart_putc('0'+(63-pos/8)%10);
	uart_putstr_P(PSTR("*00"));
	
	/* now the real stuff */
	memset(block, 0, 512/8);
	block[pos>>3] = 0x80>>(pos&0x7);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

static
void tv4_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.hashsize_b/8];
	uint16_t n=nessie_hash_ctx.hashsize_b;
	uint32_t i;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	if(nessie_hash_ctx.hashsize_b>=10000)
		uart_putc('0' + (nessie_hash_ctx.hashsize_b/10000)%10);
	if(nessie_hash_ctx.hashsize_b>=1000)
		uart_putc('0' + (nessie_hash_ctx.hashsize_b/1000)%10);
	if(nessie_hash_ctx.hashsize_b>=100)
		uart_putc('0' + (nessie_hash_ctx.hashsize_b/100)%10);
	if(nessie_hash_ctx.hashsize_b>=10)
		uart_putc('0' + (nessie_hash_ctx.hashsize_b/10)%10);
	uart_putc('0' + nessie_hash_ctx.hashsize_b%10);

	uart_putstr_P(PSTR(" zero bits"));
	memset(block, 0, 256/8);
	
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n    -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
	for(i=1; i<100000L; ++i){ /* this assumes BLOCKSIZE >= HASHSIZE */
		nessie_hash_ctx.hash_init(ctx);
		nessie_hash_ctx.hash_last(hash, nessie_hash_ctx.hashsize_b, ctx);
		nessie_hash_ctx.hash_conv(hash, ctx);
	}
	nessie_print_item("iterated 100000 times", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

/*
   "" (empty string)
   message="a"
   message="abc"
   message="message digest"
   message="abcdefghijklmnopqrstuvwxyz"
   message="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
   message="A...Za...z0...9"
   message=8 times "1234567890"
*/


void nessie_hash_run(void){
	uint16_t i;
	uint8_t set;
	
	nessie_print_header(nessie_hash_ctx.name, 0, 0, nessie_hash_ctx.hashsize_b, 0, 0);
	/* test set 1 */
	char* challange[8][2]= {
		{"", "\"\" (empty string)"},
		{"a", "\"a\""},
		{"abc", "\"abc\""},
		{"message digest", "\"message digest\""},
		{"abcdefghijklmnopqrstuvwxyz","\"abcdefghijklmnopqrstuvwxyz\""},
		{"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
			"\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\""},
		{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		 "abcdefghijklmnopqrstuvwxyz"
		 "0123456789"	, "\"A...Za...z0...9\""},
		{"1234567890" "1234567890" "1234567890" "1234567890" 
		 "1234567890" "1234567890" "1234567890" "1234567890",
		 "8 times \"1234567890\""} 	
	};
	set=1;
	nessie_print_setheader(set);
	for(i=0; i<8; ++i){
		nessie_print_set_vector(set, i);
		ascii_hash(challange[i][0], challange[i][1]);
	}
	nessie_print_set_vector(set, i);
	amillion_hash();
	/* test set 2 */
	set=2;
	nessie_print_setheader(set);
	for(i=0; i<1024; ++i){
		nessie_print_set_vector(set, i);
		zero_hash(i);
	}
	/* test set 3 */
	set=3;
	nessie_print_setheader(set);
	for(i=0; i<512; ++i){
		nessie_print_set_vector(set, i);
		one_in512_hash(i);
	}
	/* test set 4 */
	set=4;
	nessie_print_setheader(set);
	nessie_print_set_vector(set, 0);
	tv4_hash();

	nessie_print_footer();
}
