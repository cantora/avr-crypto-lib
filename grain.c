/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 */


#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "grain.h"


#define GRAIN_REVERSEKEY

/* s0, s1, s2, ..., s78, s79 */
#define S(i) ((ctx->lfsr[9-((i)/8)])>>(7-((i)%8)))
/* b0, b1, b2, ..., b78, b79 */
#define B(i) ((ctx->nfsr[9-((i)/8)])>>(7-((i)%8)))


uint8_t h_lut[4] PROGMEM = {0x4C, 0xB6, 0xD3, 0x26};

uint8_t grain_enc(grain_ctx_t* ctx){
	uint8_t s80, s0, c1, c2;
	uint8_t i;
	/* clock the LFSR */
	s0=S(0);
	s80 =S(62) ^ S(51) ^ S(38) ^ S(23) ^ S(13) ^ s0;
	s80 &= 1;
	c1 = s80;
	for(i=0; i<10; ++i){
		c2 = (ctx->lfsr[i])>>7;
		ctx->lfsr[i] = ((ctx->lfsr[i])<<1) | c1;
		c1 = c2;
	}
	/* clock the NFSR */
	uint8_t b80, a,b,d,e;
	b80 = B(62) ^ B(60) ^ B(52) ^ B(45) ^ 
	      B(37) ^ B(33) ^ B(28) ^ B(21) ^ 
	      B(14) ^ B( 9) ^ B( 0) ^ s0;
	b80 ^= (a = B(63) & B(60));
	b80 ^= (b = B(37) & B(33));
	b80 ^= B(15) & B( 9); /* c */
	b80 ^= (d = B(60) & B(52) & B(45));
	b80 ^= (e = B(33) & B(28) & B(21));
	b80 ^= B(63) & B(45) & B(28) & B(9); /* f */
	/* -- */
	b80 ^= b & B(60) & B(52); /* g */
	b80 ^= a & B(21) & B(15); /* h */
	b80 ^= d & B(63) & B(37); /* i */
	b80 ^= e & B(15) & B( 9); /* j */
	b80 ^= e & B(52) & B(45) & B(37); /* k */
	c1 = b80 & 1;
	for(i=0; i<10; ++i){
		c2 = (ctx->nfsr[i])>>7;
		ctx->nfsr[i] = ((ctx->nfsr[i])<<1) | c1;
		c1 = c2;
	}
	/* now the h function */
	uint8_t h;
	i = (S(2)&1) | 
	    ((S(24)&1) << 1) |
	    ((S(45)&1) << 2) |
	    ((S(63)&1) << 3) |
	    ((B(62)&1) << 4);
	
	h = (pgm_read_byte(h_lut+(i/8)))>>(i%8);
	
	h ^= B(0) ^ B(1) ^ B(3) ^ B(9) ^ B(30) ^ B(42) ^ B(55);
	return h&1;
}

#ifdef GRAIN_REVERSEKEY

static
uint8_t reverse_bits(uint8_t a){
	uint8_t lut[16] = {
		0x0, 0x8, 0x4, 0xC,   /* 0000 1000 0100 1100 */
		0x2, 0xA, 0x6, 0xE,   /* 0010 1010 0110 1110 */
		0x1, 0x9, 0x5, 0xD,   /* 0001 1001 0101 1101 */
		0x3, 0xB, 0x7, 0xF }; /* 0011 1011 0111 1111 */
	uint8_t x;
	x = ((lut[a&0xf]) << 4) | lut[a>>4];
	return x;
}
#else

#define reverse_bits(a) (a)

#endif

void grain_init(const void* key, const void* iv, grain_ctx_t* ctx){
	uint8_t i,t;
	
	/* load the 80bit key */
	for(i=0; i<10; ++i){
		ctx->nfsr[9-i] = reverse_bits(((uint8_t*)key)[i]);
	}
	/* load the 64bit iv */
	for(i=0; i<8; ++i){
		ctx->lfsr[9-i] = reverse_bits(((uint8_t*)iv)[i]);
	}
	/* set the other bits of iv to 1 */
	ctx->lfsr[0] = ctx->lfsr[1] = 0xFF;
	
	/* run it 160 times */
	for(i=0; i<160; ++i){
		t = grain_enc(ctx);
		(ctx->lfsr[0]) ^= t;
		(ctx->nfsr[0]) ^= t;
	}
}






