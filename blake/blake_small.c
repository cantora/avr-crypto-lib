/* blake_small.c */
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
 * \file    blake_small.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-04
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "memxor.h"
#include "blake_small.h"
#include "blake_common.h"

#define BUG_1 1 /* bug compatibility for zero length message */
#define BUG_2 1 /* bug compatibility for messages of length%512=505...511 */


uint32_t blake_c[] PROGMEM = {
   0x243F6A88, 0x85A308D3,
   0x13198A2E, 0x03707344,
   0xA4093822, 0x299F31D0,
   0x082EFA98, 0xEC4E6C89,
   0x452821E6, 0x38D01377,
   0xBE5466CF, 0x34E90C6C,
   0xC0AC29B7, 0xC97C50DD,
   0x3F84D5B5, 0xB5470917
};

#define ROTL32(a, n) (((a)<<(n))|((a)>>(32-(n))))
#define ROTR32(a, n) (((a)>>(n))|((a)<<(32-(n))))
#define CHANGE_ENDIAN32(a) (((a)<<24)| \
                            ((0x0000ff00&(a))<<8)| \
						    ((0x00ff0000&(a))>>8)| \
						    (a)>>24 )

void blake_small_expand(uint32_t* v, const blake_small_ctx_t* ctx){
	uint8_t i;
	memcpy(v, ctx->h, 8*4);
	for(i=0; i<8; ++i){
		v[8+i] = pgm_read_dword(&(blake_c[i]));
	}
	memxor((uint8_t*)v+8, ctx->s, 4*4);
	
}

void blake_small_changeendian(void* dest, const void* src){
	uint8_t i;
	for(i=0; i<16; ++i){
		((uint32_t*)dest)[i] = CHANGE_ENDIAN32(((uint32_t*)src)[i]);
	}
}

void blake_small_compress(uint32_t* v,const void* m){
	uint8_t r,i;
	uint8_t a,b,c,d, s0, s1;
	uint32_t lv[4];
	for(r=0; r<10; ++r){
		for(i=0; i<8; ++i){
	//		blake_small_g(r, i, v, (uint32_t*)m);
			a = pgm_read_byte(blake_index_lut+4*i+0);
			b = pgm_read_byte(blake_index_lut+4*i+1);
			c = pgm_read_byte(blake_index_lut+4*i+2);
			d = pgm_read_byte(blake_index_lut+4*i+3);
			s0 = pgm_read_byte(blake_sigma+16*r+2*i+0);
			s1 = pgm_read_byte(blake_sigma+16*r+2*i+1);
			lv[0] = v[a];
			lv[1] = v[b];
			lv[2] = v[c];
			lv[3] = v[d];
			
			lv[0] += lv[1] + (((uint32_t*)m)[s0] ^ pgm_read_dword(&(blake_c[s1])));
			lv[3]  = ROTR32(lv[3]^lv[0], 16);
			lv[2] += lv[3];
			lv[1]  = ROTR32(lv[1]^lv[2], 12);	
			lv[0] += lv[1] + (((uint32_t*)m)[s1] ^ pgm_read_dword(&(blake_c[s0])));
			lv[3]  = ROTR32(lv[3]^lv[0], 8);
			lv[2] += lv[3];
			lv[1]  = ROTR32(lv[1]^lv[2], 7);

			v[a] = lv[0];
			v[b] = lv[1];
			v[c] = lv[2];
			v[d] = lv[3];

		}
	}
}

void blake_small_collapse(blake_small_ctx_t* ctx, uint32_t* v){
	uint8_t i;
	for(i=0; i<8; ++i){
		ctx->h[i] ^= ctx->s[i%4] ^ v[i] ^ v[8+i];
	}
}	

void blake_small_nextBlock(blake_small_ctx_t* ctx, const void* msg){
	uint32_t v[16];
	uint32_t m[16];
	union {
		uint64_t v64;
		uint32_t v32[2];
	}ctr;
	blake_small_expand(v,ctx);
	ctx->counter++;
	ctr.v64 = ctx->counter*512;
	v[12] ^= ctr.v32[0];
	v[13] ^= ctr.v32[0];
	v[14] ^= ctr.v32[1];
	v[15] ^= ctr.v32[1];
	blake_small_changeendian(m, msg);
	blake_small_compress(v, m);
	blake_small_collapse(ctx, v);
}

void blake_small_lastBlock(blake_small_ctx_t* ctx, const void* msg, uint16_t length_b){
	while(length_b>=BLAKE_SMALL_BLOCKSIZE){
		blake_small_nextBlock(ctx, msg);
		msg = (uint8_t*)msg + BLAKE_SMALL_BLOCKSIZE_B;
		length_b -= BLAKE_SMALL_BLOCKSIZE;
	}
	uint8_t buffer[64];
	uint32_t v[16];
#if BUG_2
	uint32_t tmp=0;
#endif
	union {
		uint64_t v64;
		uint32_t v32[2];
	}ctr;
	ctr.v64 = ctx->counter*512+length_b;
#if BUG_2
	if(length_b>=505){
		tmp =ctr.v32[0];
		ctr.v32[0] = ctx->counter*512;
		ctr.v32[0] |= 0x40+length_b-504;
	}
#endif	
	memset(buffer, 0, 64);
	memcpy(buffer, msg, (length_b+7)/8);
	buffer[length_b/8] |= 0x80 >> (length_b%8);
	blake_small_changeendian(buffer, buffer);
	blake_small_expand(v, ctx);
	v[12] ^= ctr.v32[0];
	v[13] ^= ctr.v32[0];
	v[14] ^= ctr.v32[1];
	v[15] ^= ctr.v32[1];
#if BUG_2
	if(length_b>=505)
		ctr.v32[0] = tmp;
#endif
#if BUG_1	
	if(length_b==0 && ctx->counter==0){
		v[14] ^= 1;
		v[15] ^= 1;
	}
#endif	
	if(length_b>512-64-2){
		blake_small_compress(v, buffer);
		blake_small_collapse(ctx, v);
		memset(buffer, 0, 64-8);
		blake_small_expand(v, ctx);
	}
	if(ctx->appendone)
		buffer[64-8-4] |= 0x01;	
	*((uint32_t*)(&(buffer[64-8]))) = ctr.v32[1];
	*((uint32_t*)(&(buffer[64-4]))) = ctr.v32[0];
	blake_small_compress(v, buffer);
	blake_small_collapse(ctx, v);
	
}

uint32_t blake32_iv[] PROGMEM = {
	0x6A09E667L, 0xBB67AE85,
	0x3C6EF372L, 0xA54FF53A,
	0x510E527FL, 0x9B05688C,
	0x1F83D9ABL, 0x5BE0CD19
};

void blake32_init(blake32_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<8; ++i){
		ctx->h[i] = pgm_read_dword(&(blake32_iv[i]));
	}
	memset(ctx->s, 0, 4*4);
	ctx->counter = 0;
	ctx->appendone = 1;
}

uint32_t blake28_iv[] PROGMEM = {
	0xC1059ED8, 0x367CD507,
	0x3070DD17, 0xF70E5939,
	0xFFC00B31, 0x68581511,
	0x64F98FA7, 0xBEFA4FA4
};

void blake28_init(blake28_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<8; ++i){
		ctx->h[i] = pgm_read_dword(&(blake28_iv[i]));
	}
	memset(ctx->s, 0, 4*4);
	ctx->counter = 0;
	ctx->appendone = 0;
}

void blake32_ctx2hash(void* dest, const blake32_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<8; ++i){
		((uint32_t*)dest)[i] = CHANGE_ENDIAN32(ctx->h[i]);
	}
}

void blake28_ctx2hash(void* dest, const blake28_ctx_t* ctx){
	uint8_t i;
	for(i=0; i<7; ++i){
		((uint32_t*)dest)[i] = CHANGE_ENDIAN32(ctx->h[i]);
	}
}

void blake32_nextBlock(blake32_ctx_t* ctx, const void* block){
	blake_small_nextBlock(ctx, block);
}

void blake28_nextBlock(blake28_ctx_t* ctx, const void* block){
	blake_small_nextBlock(ctx, block);
}

void blake32_lastBlock(blake32_ctx_t* ctx, const void* block, uint16_t length_b){
	blake_small_lastBlock(ctx, block, length_b);
}

void blake28_lastBlock(blake28_ctx_t* ctx, const void* block, uint16_t length_b){
	blake_small_lastBlock(ctx, block, length_b);
}

void blake32(void* dest, const void* msg, uint32_t length_b){
	blake_small_ctx_t ctx;
	blake32_init(&ctx);
	while(length_b>=BLAKE_SMALL_BLOCKSIZE){
		blake_small_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + BLAKE_SMALL_BLOCKSIZE_B;
		length_b -= BLAKE_SMALL_BLOCKSIZE;
	}
	blake_small_lastBlock(&ctx, msg, length_b);
	blake32_ctx2hash(dest, &ctx);
}

void blake28(void* dest, const void* msg, uint32_t length_b){
	blake_small_ctx_t ctx;
	blake28_init(&ctx);
	while(length_b>=BLAKE_SMALL_BLOCKSIZE){
		blake_small_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + BLAKE_SMALL_BLOCKSIZE_B;
		length_b -= BLAKE_SMALL_BLOCKSIZE;
	}
	blake_small_lastBlock(&ctx, msg, length_b);
	blake28_ctx2hash(dest, &ctx);
}
