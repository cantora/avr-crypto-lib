/* whirlpool.c */
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

#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "gf256mul.h"
#include "memxor.h"
#include "whirlpool.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

/*
u       0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
E(u)    1 	B 	9 	C 	D 	6 	F 	3 	E 	8 	7 	4 	A 	2 	5 	0
E -1(u) F 	0 	D 	7 	B 	E 	5 	A 	9 	2 	C 	1 	3 	4 	8 	6
*/
static uint8_t eeinv_box[16] PROGMEM = {
	/*	0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
	0x1F, 0xB0, 0x9D, 0xC7, 0xDB, 0x6E, 0xF5, 0x3A,
	0xE9, 0x82, 0x7C, 0x41, 0xA3, 0x24, 0x58, 0x06
};

static uint8_t r_box[16] PROGMEM = {
	0x77, 0xCC, 0xBB, 0xDD, 0xEE, 0x44, 0x99, 0xFF,
	0x66, 0x33, 0x88, 0xAA, 0x22, 0x55, 0x11, 0x00
};

uint8_t whirlpool_sbox(uint8_t a){
	uint8_t b,c,d;
	b = pgm_read_byte(eeinv_box+(a&0x0f));
	d = pgm_read_byte(eeinv_box+(a>>4));
	c = ((d>>4)^b)&0x0f;
	b = (b&0x0f)|(d&0xf0);
	b ^= pgm_read_byte(r_box+c);
	c = pgm_read_byte(eeinv_box+(b&0x0f))&0x0f;
	c |= pgm_read_byte(eeinv_box+(b>>4))&0xf0;
	return c;
}

static void gamma(uint8_t* a){
	uint8_t i;
	for(i=0; i<64; ++i){
		*a = whirlpool_sbox(*a);
		++a;
	}
}

static void pi(uint8_t* a){
	uint8_t b[8];
	uint8_t i,j;
	for(i=1; i<8; ++i){
		for(j=0; j<8; ++j){
			b[j] = a[i+8*((8+j-i)%8)];
		}
		for(j=0; j<8; ++j){
			a[j*8+i] = b[j];
		}
	}
}

static uint8_t theta_matrix[8] PROGMEM = {
	0x1, 0x1, 0x4, 0x1, 0x8, 0x5, 0x2, 0x9
};

#define POLYNOM 0x1D

static void theta(uint8_t* a){
	uint8_t b[8], c, accu;
	uint8_t i,j,k;
	for(i=0; i<8; ++i){
		for(j=0; j<8;++j){
			accu = 0;
			for(k=0; k<8; ++k){
				c = pgm_read_byte(theta_matrix+((8+j-k)%8));
				accu ^= gf256mul(a[8*i+k], c, POLYNOM);
			}
			b[j] = accu;
		}
		memcpy(a+8*i, b, 8);
	}
}

static void w_round(uint8_t* a, const uint8_t* k){
	gamma(a);
#if DEBUG
	cli_putstr_P(PSTR("\r\n pre-pi:"));
	cli_hexdump_block(a, 64, 4, 8);
#endif
	pi(a);
#if DEBUG
	cli_putstr_P(PSTR("\r\n post-pi & pre-theta:"));
	cli_hexdump_block(a, 64, 4, 8);
#endif
	theta(a);
#if DEBUG
	cli_putstr_P(PSTR("\r\n post-theta:"));
	cli_hexdump_block(a, 64, 4, 8);
#endif
	memxor(a, k, 64);
}

static void w_enc(uint8_t *a, const uint8_t* k){
#if DEBUG
	cli_putstr_P(PSTR("\r\n== w_enc ==\r\n w'_00:"));
	cli_hexdump_block(a, 64, 4, 8);
	cli_putstr_P(PSTR("\r\n k_00:"));
	cli_hexdump_block(k, 64, 4, 8);
#endif
	uint8_t rk[64], rc[64];
	uint8_t r,i;
	memxor(a, k, 64);
	memcpy(rk, k, 64);
	memset(rc+8, 0, 56);
	for(r=0; r<10; ++r){
		for(i=0; i<8; ++i){
			rc[i] = whirlpool_sbox(r*8+i);
		}
		w_round(rk, rc);
		w_round(a, rk);
#if DEBUG
		cli_putstr_P(PSTR("\r\n w'_"));
		cli_hexdump_byte(r+1);
		cli_putc(':');
		cli_hexdump_block(a, 64, 4, 8);
		cli_putstr_P(PSTR("\r\n k_"));
		cli_hexdump_byte(r+1);
		cli_putc(':');
		cli_hexdump_block(rk, 64, 4, 8);
#endif
	}
}

void whirlpool_init(whirlpool_ctx_t* ctx){
	memset(ctx->s, 0, 64);
	ctx->blocks = 0;
}

void whirlpool_nextBlock(whirlpool_ctx_t* ctx, const void* block){
	uint8_t state[64];
	ctx->blocks += 1;
	memcpy(state, block, 64);
	w_enc(state, (uint8_t*)(ctx->s));
	memxor(ctx->s, state, 64);
	memxor((ctx->s), block, 64);
}

void whirlpool_lastBlock(whirlpool_ctx_t* ctx, const void* block, uint16_t length_b){
	while(length_b>=512){
		whirlpool_nextBlock(ctx, block);
		block = (uint8_t*)block + 64;
		length_b -= 512;
	}
	uint8_t buffer[64];
	uint8_t i=8;
	uint64_t length;
	length = ctx->blocks*512+length_b;
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b&7);
	if(length_b>255){
		whirlpool_nextBlock(ctx, buffer);
		memset(buffer, 0, 56);
	}
	do{
		buffer[56+(--i)] = length&0xff;
		length >>= 8;
	}while(i);
	whirlpool_nextBlock(ctx, buffer);
}

void whirlpool_ctx2hash(void* dest, const whirlpool_ctx_t* ctx){
	memcpy(dest, (ctx->s), 64);
}
