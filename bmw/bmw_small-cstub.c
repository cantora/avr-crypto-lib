/* bmw_small.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file    bmw_small.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 *
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "bmw_small.h"


#define SHL32(a,n) ((a)<<(n))
#define SHR32(a,n) ((a)>>(n))
#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))
#define ROTR32(a,n) (((a)>>(n))|((a)<<(32-(n))))

#define DEBUG 0


#if DEBUG
 #include "cli.h"

 void ctx_dump(const bmw_small_ctx_t* ctx){
 	uint8_t i;
	cli_putstr_P(PSTR("\r\n==== ctx dump ===="));
	for(i=0; i<16;++i){
		cli_putstr_P(PSTR("\r\n h["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(ctx->h[i]), 4);
	}
	cli_putstr_P(PSTR("\r\n counter = "));
	cli_hexdump(&(ctx->counter), 4);
 }

 void dump_x(const uint32_t* q, uint8_t elements, char x){
	uint8_t i;
 	cli_putstr_P(PSTR("\r\n==== "));
	cli_putc(x);
	cli_putstr_P(PSTR(" dump ===="));
	for(i=0; i<elements;++i){
		cli_putstr_P(PSTR("\r\n "));
		cli_putc(x);
		cli_putstr_P(PSTR("["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(q[i]), 4);
	}
 }
#else
 #define ctx_dump(x)
 #define dump_x(a,b,c)
#endif

void bmw_small_f1(uint32_t* q, const void* m, const void* h);
void bmw_small_f0(uint32_t* h, const void* m, uint32_t* q);
void bmw_small_f2(uint32_t* h, uint32_t* q, const void* m);
void bmw_small_nextBlock(bmw_small_ctx_t* ctx, const void* block);

/*
void bmw_small_nextBlock(bmw_small_ctx_t* ctx, const void* block){
	uint32_t q[32];
	dump_x(block, 16, 'M');
	bmw_small_f0(ctx->h, block, q);
	dump_x(q, 16, 'Q');
	bmw_small_f1(q, block, ctx->h);
	dump_x(q, 32, 'Q');
	bmw_small_f2(ctx->h, q, block);
	ctx->counter += 1;
	ctx_dump(ctx);
}
*/

void bmw_small_lastBlock(bmw_small_ctx_t* ctx, const void* block, uint16_t length_b){
	struct {
		uint8_t  buffer[64];
		uint32_t ctr;
	} pctx;
	while(length_b >= BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(ctx, block);
		length_b -= BMW_SMALL_BLOCKSIZE;
		block = (uint8_t*)block + BMW_SMALL_BLOCKSIZE_B;
	}
	memset(pctx.buffer, 0, 64);
	memcpy(pctx.buffer, block, (length_b+7)/8);
	pctx.buffer[length_b>>3] |= 0x80 >> (length_b&0x07);
	if(length_b+1>64*8-64){
		bmw_small_nextBlock(ctx, pctx.buffer);
		memset(pctx.buffer, 0, 64-8);
		ctx->counter -= 1;
	}
	*((uint64_t*)&(pctx.buffer[64-8])) = (uint64_t)(ctx->counter*512LL)+(uint64_t)length_b;
	bmw_small_nextBlock(ctx, pctx.buffer);
	uint8_t i;
	memset(pctx.buffer, 0xaa, 64);
	for(i=0; i<16;++i){
		pctx.buffer[i*4] = i+0xa0;
	}
	bmw_small_nextBlock((bmw_small_ctx_t*)&pctx, ctx->h);
	memcpy(ctx->h, pctx.buffer, 64);
}

void bmw224_init(bmw224_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x00010203;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x04040404;
	}
	ctx->counter=0;
//	ctx_dump(ctx);
}

void bmw256_init(bmw256_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x40414243;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x04040404;
	}
	ctx->counter=0;
//	ctx_dump(ctx);
}

void bmw224_nextBlock(bmw224_ctx_t* ctx, const void* block){
	bmw_small_nextBlock(ctx, block);
}

void bmw256_nextBlock(bmw256_ctx_t* ctx, const void* block){
	bmw_small_nextBlock(ctx, block);
}

void bmw224_lastBlock(bmw224_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_small_lastBlock(ctx, block, length_b);
}

void bmw256_lastBlock(bmw256_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_small_lastBlock(ctx, block, length_b);
}

void bmw224_ctx2hash(void* dest, const bmw224_ctx_t* ctx){
	memcpy(dest, &(ctx->h[9]), 224/8);
}

void bmw256_ctx2hash(void* dest, const bmw256_ctx_t* ctx){
	memcpy(dest, &(ctx->h[8]), 256/8);
}

void bmw224(void* dest, const void* msg, uint32_t length_b){
	bmw_small_ctx_t ctx;
	bmw224_init(&ctx);
	while(length_b>=BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(&ctx, msg);
		length_b -= BMW_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
	}
	bmw_small_lastBlock(&ctx, msg, length_b);
	bmw224_ctx2hash(dest, &ctx);
}

void bmw256(void* dest, const void* msg, uint32_t length_b){
	bmw_small_ctx_t ctx;
	bmw256_init(&ctx);
	while(length_b>=BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(&ctx, msg);
		length_b -= BMW_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
	}
	bmw_small_lastBlock(&ctx, msg, length_b);
	bmw256_ctx2hash(dest, &ctx);
}


