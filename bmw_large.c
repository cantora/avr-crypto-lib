/* bmw_large.c */
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
 * \file    bmw_large.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include "bmw_large.h"

#define SHL64(a,n) ((a)<<(n))
#define SHR64(a,n) ((a)>>(n))
#define ROTL64(a,n) (((a)<<(n))|((a)>>(64-(n))))
#define ROTR64(a,n) (((a)>>(n))|((a)<<(64-(n))))

#define BUG24 1

#define DEBUG 0
#if DEBUG
 #include "cli.h"
 
 void ctx_dump(const bmw_large_ctx_t* ctx){
 	uint8_t i;
	cli_putstr_P(PSTR("\r\n==== ctx dump ===="));
	for(i=0; i<16;++i){
		cli_putstr_P(PSTR("\r\n h["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(ctx->h[i]), 8);
	}
	cli_putstr_P(PSTR("\r\n counter = "));
	cli_hexdump(&(ctx->counter), 4);
 }
 
 void dump_x(const uint64_t* q, uint8_t elements, char x){
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
		cli_hexdump_rev(&(q[i]), 8);
	}
 }
#else
 #define ctx_dump(x)
 #define dump_x(a,b,c)
#endif

uint64_t bmw_large_s0(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ SHL64(x, 3)
		^ ROTL64(x, 4)
		^ ROTR64(x, 64-37);
	return r;	
}

uint64_t bmw_large_s1(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ SHL64(x, 2)
		^ ROTL64(x,13)
		^ ROTR64(x,64-43);
	return r;	
}

uint64_t bmw_large_s2(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ SHL64(x, 1)
		^ ROTL64(x, 19)
		^ ROTR64(x, 64-53);
	return r;	
}

uint64_t bmw_large_s3(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ SHL64(x, 2)
		^ ROTL64(x, 28)
		^ ROTR64(x, 64-59);
	return r;	
}

uint64_t bmw_large_s4(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ x;
	return r;	
}

uint64_t bmw_large_s5(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ x;
	return r;	
}

uint64_t bmw_large_r1(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 5);
	return r;	
}

uint64_t bmw_large_r2(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 11);
	return r;	
}

uint64_t bmw_large_r3(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 27);
	return r;	
}

uint64_t bmw_large_r4(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 32);
	return r;	
}

uint64_t bmw_large_r5(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-37);
	return r;	
}

uint64_t bmw_large_r6(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-43);
	return r;	
}

uint64_t bmw_large_r7(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-53);
	return r;	
}

uint64_t bmw_large_expand1(uint8_t j, const uint64_t* q, const void* m){
	uint64_t(*s[])(uint64_t) = {bmw_large_s1, bmw_large_s2, bmw_large_s3, bmw_large_s0};
	uint64_t r;
	uint8_t i;
	r = 0x0555555555555555LL*(j+16);
	for(i=0; i<16; ++i){
		r += s[i%4](q[j+i]);
	}
	r += ((uint64_t*)m)[j];
	r += ((uint64_t*)m)[j+3];
	r -= ((uint64_t*)m)[j+10];
	return r;
}

uint64_t bmw_large_expand2(uint8_t j, const uint64_t* q, const void* m){
	uint64_t(*rf[])(uint64_t) = {bmw_large_r1, bmw_large_r2, bmw_large_r3,
	                             bmw_large_r4, bmw_large_r5, bmw_large_r6,
							     bmw_large_r7};
	uint64_t r;
	uint8_t i;
	r = 0x0555555555555555LL*(j+16);
	for(i=0; i<14; i+=2){
		r += q[j+i];
	}
	for(i=0; i<14; i+=2){
		r += rf[i/2](q[j+i+1]);
	}
	r += bmw_large_s5(q[j+14]);
	r += bmw_large_s4(q[j+15]);
	r += ((uint64_t*)m)[j];
	r += ((uint64_t*)m)[(j+3)%16];
	r -= ((uint64_t*)m)[(j+10)%16];
	return r;
}

void bmw_large_f0(uint64_t* q, const uint64_t* h, const void* m){
	uint64_t t[16];
	uint8_t i;
	uint64_t(*s[])(uint64_t)={ bmw_large_s0, bmw_large_s1, bmw_large_s2,
	                           bmw_large_s3, bmw_large_s4 };
	for(i=0; i<16; ++i){
		t[i] = h[i] ^ ((uint64_t*)m)[i];
	}
	dump_x(t, 16, 'T');
	q[ 0] = (t[ 5] - t[ 7] + t[10] + t[13] + t[14]);
	q[ 1] = (t[ 6] - t[ 8] + t[11] + t[14] - t[15]);
	q[ 2] = (t[ 0] + t[ 7] + t[ 9] - t[12] + t[15]);
	q[ 3] = (t[ 0] - t[ 1] + t[ 8] - t[10] + t[13]);
	q[ 4] = (t[ 1] + t[ 2] + t[ 9] - t[11] - t[14]);
	q[ 5] = (t[ 3] - t[ 2] + t[10] - t[12] + t[15]);
	q[ 6] = (t[ 4] - t[ 0] - t[ 3] - t[11] + t[13]); 
	q[ 7] = (t[ 1] - t[ 4] - t[ 5] - t[12] - t[14]);
	q[ 8] = (t[ 2] - t[ 5] - t[ 6] + t[13] - t[15]);
	q[ 9] = (t[ 0] - t[ 3] + t[ 6] - t[ 7] + t[14]);
	q[10] = (t[ 8] - t[ 1] - t[ 4] - t[ 7] + t[15]);
	q[11] = (t[ 8] - t[ 0] - t[ 2] - t[ 5] + t[ 9]);
	q[12] = (t[ 1] + t[ 3] - t[ 6] - t[ 9] + t[10]);
	q[13] = (t[ 2] + t[ 4] + t[ 7] + t[10] + t[11]);
	q[14] = (t[ 3] - t[ 5] + t[ 8] - t[11] - t[12]);
	q[15] = (t[12] - t[ 4] - t[ 6] - t[ 9] + t[13]); 
	dump_x(q, 16, 'W');
	for(i=0; i<16; ++i){
		q[i] = s[i%5](q[i]);
	}	
}

void bmw_large_f1(uint64_t* q, const void* m){
	uint8_t i;
	q[16] = bmw_large_expand1(0, q, m);
	q[17] = bmw_large_expand1(1, q, m);
	for(i=2; i<16; ++i){
		q[16+i] = bmw_large_expand2(i, q, m);
	}
}

void bmw_large_f2(uint64_t* h, const uint64_t* q, const void* m){
	uint64_t xl=0, xh;
	uint8_t i;
	for(i=16;i<24;++i){
		xl ^= q[i];
	}
	xh = xl;
	for(i=24;i<32;++i){
		xh ^= q[i];
	}
#if DEBUG	
	cli_putstr_P(PSTR("\r\n XL = "));
	cli_hexdump_rev(&xl, 4);
	cli_putstr_P(PSTR("\r\n XH = "));
	cli_hexdump_rev(&xh, 4);
#endif
	memcpy(h, m, 16*8);
	h[0] ^= SHL64(xh, 5) ^ SHR64(q[16], 5);
	h[1] ^= SHR64(xh, 7) ^ SHL64(q[17], 8);
	h[2] ^= SHR64(xh, 5) ^ SHL64(q[18], 5);
	h[3] ^= SHR64(xh, 1) ^ SHL64(q[19], 5);
	h[4] ^= SHR64(xh, 3) ^ q[20];
	h[5] ^= SHL64(xh, 6) ^ SHR64(q[21], 6);
	h[6] ^= SHR64(xh, 4) ^ SHL64(q[22], 6);
	h[7] ^= SHR64(xh,11) ^ SHL64(q[23], 2);
	for(i=0; i<8; ++i){
		h[i] += xl ^ q[24+i] ^ q[i];
	}
	for(i=0; i<8; ++i){
		h[8+i] ^= xh ^ q[24+i];
		h[8+i] += ROTL64(h[(4+i)%8],i+9);
	}
	h[ 8] += SHL64(xl, 8) ^ q[23] ^ q[ 8];
	h[ 9] += SHR64(xl, 6) ^ q[16] ^ q[ 9];
	h[10] += SHL64(xl, 6) ^ q[17] ^ q[10];
	h[11] += SHL64(xl, 4) ^ q[18] ^ q[11];
	h[12] += SHR64(xl, 3) ^ q[19] ^ q[12];
	h[13] += SHR64(xl, 4) ^ q[20] ^ q[13];
	h[14] += SHR64(xl, 7) ^ q[21] ^ q[14];
	h[15] += SHR64(xl, 2) ^ q[22] ^ q[15];
}

void bmw_large_nextBlock(bmw_large_ctx_t* ctx, const void* block){
	uint64_t q[32];
	dump_x(block, 16, 'M');
	bmw_large_f0(q, ctx->h, block);
	dump_x(q, 16, 'Q');
	bmw_large_f1(q, block);
	dump_x(q, 32, 'Q');
	bmw_large_f2(ctx->h, q, block);
	ctx->counter += 1;
	ctx_dump(ctx);
}

void bmw_large_lastBlock(bmw_large_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t buffer[128];
	while(length_b >= BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(ctx, block);
		length_b -= BMW_LARGE_BLOCKSIZE;
		block = (uint8_t*)block + BMW_LARGE_BLOCKSIZE_B;
	}
	memset(buffer, 0, 128);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b>>3] |= 0x80 >> (length_b&0x07);
	if(length_b+1>128*8-64){
		bmw_large_nextBlock(ctx, buffer);
		memset(buffer, 0, 128-8);
		ctx->counter -= 1;
	}
	*((uint64_t*)&(buffer[128-8])) = (uint64_t)(ctx->counter*1024LL)+(uint64_t)length_b;
	bmw_large_nextBlock(ctx, buffer);
}

void bmw384_init(bmw384_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x0001020304050607LL;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x0808080808080808LL;
	}
#if BUG24	
	ctx->h[6] = 0x3031323324353637LL;
#endif
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw512_init(bmw512_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x8081828384858687LL;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x0808080808080808LL;
	}
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw384_nextBlock(bmw384_ctx_t* ctx, const void* block){
	bmw_large_nextBlock(ctx, block);
}

void bmw512_nextBlock(bmw512_ctx_t* ctx, const void* block){
	bmw_large_nextBlock(ctx, block);
}

void bmw384_lastBlock(bmw384_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_large_lastBlock(ctx, block, length_b);
}

void bmw512_lastBlock(bmw512_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_large_lastBlock(ctx, block, length_b);
}

void bmw384_ctx2hash(void* dest, const bmw384_ctx_t* ctx){
	memcpy(dest, &(ctx->h[10]), 384/8);
}

void bmw512_ctx2hash(void* dest, const bmw512_ctx_t* ctx){
	memcpy(dest, &(ctx->h[8]), 512/8);
}

void bmw384(void* dest, const void* msg, uint32_t length_b){
	bmw_large_ctx_t ctx;
	bmw384_init(&ctx);
	while(length_b>=BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(&ctx, msg);
		length_b -= BMW_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_LARGE_BLOCKSIZE_B;
	}
	bmw_large_lastBlock(&ctx, msg, length_b);
	bmw384_ctx2hash(dest, &ctx);
}

void bmw512(void* dest, const void* msg, uint32_t length_b){
	bmw_large_ctx_t ctx;
	bmw512_init(&ctx);
	while(length_b>=BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(&ctx, msg);
		length_b -= BMW_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_LARGE_BLOCKSIZE_B;
	}
	bmw_large_lastBlock(&ctx, msg, length_b);
	bmw512_ctx2hash(dest, &ctx);
}

