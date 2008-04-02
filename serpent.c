/* serpent.c
 * a bitsliced implementation of the serpent cipher for avr microcontrollers
 * author: Daniel Otte
 * license: GPLv3
 */

#include <stdint.h>
#include <string.h> /* memset() */
#include <avr/pgmspace.h>

#include "uart.h"
#include "debug.h"


/*
  S0:  3  8 15  1 10  6  5 11 14 13  4  2  7  0  9 12
  S1: 15 12  2  7  9  0  5 10  1 11 14  8  6 13  3  4
  S2:  8  6  7  9  3 12 10 15 13  1 14  4  0 11  5  2
  S3:  0 15 11  8 12  9  6  3 13  1  2  4 10  7  5 14
  S4:  1 15  8  3 12  0 11  6  2  5  4 10  9 14  7 13
  S5: 15  5  2 11  4 10  9 12  0  3 14  8 13  6  7  1
  S6:  7  2 12  5  8  4  6 11 14  9  1 15 13  3 10  0
  S7:  1 13 15  0 14  8  2 11  7  4 12 10  9  3  5  6
*/

uint8_t sbox[] PROGMEM = {
 0x38, 0xF1, 0xA6, 0x5B, 0xED, 0x42, 0x70, 0x9C,
 0xFC, 0x27, 0x90, 0x5A, 0x1B, 0xE8, 0x6D, 0x34, 
 0x86, 0x79, 0x3C, 0xAF, 0xD1, 0xE4, 0x0B, 0x52,
 0x0F, 0xB8, 0xC9, 0x63, 0xD1, 0x24, 0xA7, 0x5E,
 0x1F, 0x83, 0xC0, 0xB6, 0x25, 0x4A, 0x9E, 0x7D,
 0xF5, 0x2B, 0x4A, 0x9C, 0x03, 0xE8, 0xD6, 0x71,
 0x72, 0xC5, 0x84, 0x6B, 0xE9, 0x1F, 0xD3, 0xA0,
 0x1D, 0xF0, 0xE8, 0x2B, 0x74, 0xCA, 0x93, 0x56,
/* now the inverted sboxes */
 0xD3, 0xB0, 0xA6, 0x5C, 0x1E, 0x47, 0xF9, 0x82,
 0x58, 0x2E, 0xF6, 0xC3, 0xB4, 0x79, 0x1D, 0xA0,
 0xC9, 0xF4, 0xBE, 0x12, 0x03, 0x6D, 0x58, 0xA7,
 0x09, 0xA7, 0xBE, 0x6D, 0x35, 0xC2, 0x48, 0xF1,
 0x50, 0x83, 0xA9, 0x7E, 0x2C, 0xB6, 0x4F, 0xD1,
 0x8F, 0x29, 0x41, 0xDE, 0xB6, 0x53, 0x7C, 0xA0,
 0xFA, 0x1D, 0x53, 0x60, 0x49, 0xE7, 0x2C, 0x8B,
 0x30, 0x6D, 0x9E, 0xF8, 0x5C, 0xB7, 0xA1, 0x42
};        
         
/* 
  InvS0: 13  3 11  0 10  6  5 12  1 14  4  7 15  9  8  2
  InvS1:  5  8  2 14 15  6 12  3 11  4  7  9  1 13 10  0
  InvS2: 12  9 15  4 11 14  1  2  0  3  6 13  5  8 10  7
  InvS3:  0  9 10  7 11 14  6 13  3  5 12  2  4  8 15  1
  InvS4:  5  0  8  3 10  9  7 14  2 12 11  6  4 15 13  1
  InvS5:  8 15  2  9  4  1 13 14 11  6  5  3  7 12 10  0
  InvS6: 15 10  1 13  5  3  6  0  4  9 14  7  2 12  8 11
  InvS7:  3  0  6 13  9 14 15  8  5 12 11  7 10  1  4  2
*/
/*
uint8_t invsbox[] PROGMEM = {
 0xD3, 0xB0, 0xA6, 0x5C, 0x1E, 0x47, 0xF9, 0x82,
 0x58, 0x2E, 0xF6, 0xC3, 0xB4, 0x79, 0x1D, 0xA0,
 0xC9, 0xF4, 0xBE, 0x12, 0x03, 0x6D, 0x58, 0xA7,
 0x09, 0xA7, 0xBE, 0x6D, 0x35, 0xC2, 0x48, 0xF1,
 0x50, 0x83, 0xA9, 0x7E, 0x2C, 0xB6, 0x4F, 0xD1,
 0x8F, 0x29, 0x41, 0xDE, 0xB6, 0x53, 0x7C, 0xA0,
 0xFA, 0x1D, 0x53, 0x60, 0x49, 0xE7, 0x2C, 0x8B,
 0x30, 0x6D, 0x9E, 0xF8, 0x5C, 0xB7, 0xA1, 0x42
}
*/

static uint8_t byteflip(uint8_t v){
	uint8_t tab[] = { 0x0, 0x8, 0x4, 0xC,
					  0x2, 0xA, 0x6, 0xE,
					  0x1, 0x9, 0x5, 0xD,
					  0x3, 0xB, 0x7, 0xF };
	uint8_t ret;
	ret = ((tab[v&0xf]) << 4) | tab[v>>4];
	return ret;
}

static uint8_t getbit(void* b, uint8_t addr){
	uint8_t t;
	t = ((uint8_t*)b)[addr/8];
	t = (t&(1<<(addr&0x7)))?1:0;
	return t;
}

static void setbit(void* b, uint8_t addr, uint8_t v){
	uint8_t t;
	t = ((uint8_t*)b)[addr/8];
	if(v){
		t |= 1<<(addr&0x7);
	} else {
		t &= ~(1<<(addr&0x7));
	}
	((uint8_t*)b)[addr/8] = t;	
}
/*
uint8_t ip_table[] PROGMEM = {
	 0, 32, 64,  96,  1, 33, 65,  97,  2, 34, 66,  98,  3, 35, 67,  99,
	 4, 36, 68, 100,  5, 37, 69, 101,  6, 38, 70, 102,  7, 39, 71, 103,
	 8, 40, 72, 104,  9, 41, 73, 105, 10, 42, 74, 106, 11, 43, 75, 107,
	12, 44, 76, 108, 13, 45, 77, 109, 14, 46, 78, 110, 15, 47, 79, 111,
	16, 48, 80, 112, 17, 49, 81, 113, 18, 50, 82, 114, 19, 51, 83, 115,
	20, 52, 84, 116, 21, 53, 85, 117, 22, 54, 86, 118, 23, 55, 87, 119,
	24, 56, 88, 120, 25, 57, 89, 121, 26, 58, 90, 122, 27, 59, 91, 123,
	28, 60, 92, 124, 29, 61, 93, 125, 30, 62, 94, 126, 31, 63, 95, 127
};
* /
uint8_t fp_table[] PROGMEM = {
	 0,  4,  8, 12, 16, 20, 24, 28, 32,  36,  40,  44,  48,  52,  56,  60,
	64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
	 1,  5,  9, 13, 17, 21, 25, 29, 33,  37,  41,  45,  49,  53,  57,  61,
	65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125,
	 2,  6, 10, 14, 18, 22, 26, 30, 34,  38,  42,  46,  50,  54,  58,  62,
	66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126,
	 3,  7, 11, 15, 19, 23, 27, 31, 35,  39,  43,  47,  51,  55,  59,  63,
	67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127
};
*/
/*
static void ip_stupid(void* out, void* in){
	uint8_t i,x,t;
	for(i=0; i<128; ++i){
		t = pgm_read_byte(ip_table + i);
		x = getbit(in, t);
		setbit(out, i, x);
	}
}
*/
/*
static void fp_stupid(void* out, void* in){
	uint8_t i,x,t;
	for(i=0; i<128; ++i){
		t = pgm_read_byte(fp_table + i);
		x = getbit(in, t);
		setbit(out, i, x);
	}
}
*/
/* this is a implementation of the needed propertys only! */
#define SHR_O(a) c=(a)&1; ((a) = (a)>>1)
#define SHR_I(a) ((a) = (c?0x80:0x00)| ((a)>>1))

static void ip(uint8_t *o, uint32_t *i){
	uint8_t c; // carry 
	uint8_t n,m;
	memset(o, 0, 16);
	for(n=0; n<16; ++n){
		for(m=0; m<2; ++m){
		SHR_O(i[0]);
		SHR_I(o[n]);
		SHR_O(i[1]);
		SHR_I(o[n]);
		SHR_O(i[2]);
		SHR_I(o[n]);
		SHR_O(i[3]);
		SHR_I(o[n]);
		}
	}
}

#undef SHR_I
#define SHR_I(a) ((a) = (c?0x80000000L:0x00L)| ((a)>>1)) /* we use 32-bit words here */


static void fp(uint32_t *o, uint32_t *i){
	uint8_t c; // carry 
	uint8_t n,m;
	memset(o, 0, 16);
	for(n=0; n<4; ++n){
		for(m=0; m<8; ++m){
		SHR_O(i[n]);
		SHR_I(o[0]);
		SHR_O(i[n]);
		SHR_I(o[1]);
		SHR_O(i[n]);
		SHR_I(o[2]);
		SHR_O(i[n]);
		SHR_I(o[3]);
		}
	}
}

/******************************************************************************/

static void sbox128x(uint8_t box, void* w){
	uint8_t sb[16];
	uint8_t i,t,x;
	box &= 0x0f;
	/* load sbox */
	for(i=0; i<8; ++i){
		t = pgm_read_byte(sbox + box*8 + i);
		sb[2*i+0]=t>>4;
		sb[2*i+1]=t&0xf;
	}
	uint8_t o[16];
	ip(o, w);
	
	for(i=0; i<16; ++i){
		t = ((uint8_t*)o)[i];
		x = sb[t>>4];
		x <<= 4;
		x |= sb[t&0xf];
		((uint8_t*)o)[i] = x;
	}
	fp(w, o);
}

static void sbox128(void * w, uint8_t box){
	sbox128x(box&0x7, w);
}

static void inv_sbox128(void * w, uint8_t box){
	sbox128x(((box&0x7)|0x8), w);
}

/******************************************************************************/

void memxor(void * dest, void * src, uint8_t size){
	while(size--){
		*((uint8_t*)dest) ^= *((uint8_t*)src);
		dest = (uint8_t*)dest +1;
		src  = (uint8_t*)src  +1;
	}
}

/******************************************************************************/

uint32_t rotl32(uint32_t a, uint8_t n){
	return ((a<<n) | (a>>(32-n)));
}


uint32_t rotr32(uint32_t a, uint8_t n){
	return ((a>>n) | (a<<(32-n)));
}


#define X0 (((uint32_t*)b)[0])
#define X1 (((uint32_t*)b)[1])
#define X2 (((uint32_t*)b)[2])
#define X3 (((uint32_t*)b)[3])

static void lt(uint8_t *b){
	X0 = rotl32(X0, 13);
	X2 = rotl32(X2,  3);
	X1 ^= X0 ^ X2;
	X3 ^= X2 ^ (X0 << 3);
	X1 = rotl32(X1, 1);
	X3 = rotl32(X3, 7);
	X0 ^= X1 ^ X3;
	X2 ^= X3 ^ (X1 << 7);
	X0 = rotl32(X0, 5);
	X2 = rotr32(X2, 10);
}

static void inv_lt(uint8_t *b){
	X2 = rotl32(X2, 10);
	X0 = rotr32(X0, 5);
	X2 ^= X3 ^ (X1 << 7);
	X0 ^= X1 ^ X3;
	X3 = rotr32(X3, 7);
	X1 = rotr32(X1, 1);
	X3 ^= X2 ^ (X0 << 3);
	X1 ^= X0 ^ X2;
	X2 = rotr32(X2,  3);
	X0 = rotr32(X0, 13);
}

typedef uint32_t serpent_subkey_t[4];

typedef struct serpent_ctx_st {
	serpent_subkey_t k[33];
}  serpent_ctx_t;


#define GOLDEN_RATIO 0x9e3779b9l

static uint32_t gen_w(uint32_t * b, uint8_t i){
	uint32_t ret;
	ret = b[0] ^ b[3] ^ b[5] ^ b[7] ^ GOLDEN_RATIO ^ (uint32_t)i;
	ret = rotl32(ret, 11);
	return ret;
} 

/* key must be 256bit (32 byte) large! */
void serpent_genctx(void * key, serpent_ctx_t * ctx){
	uint32_t buffer[8];
	uint8_t i,j;
	memcpy(buffer, key, 32); 
	for(i=0; i<33; ++i){
		for(j=0; j<4; ++j){
			ctx->k[i][j] = gen_w(buffer, i*4+j);
			memmove(buffer, &(buffer[1]), 7*4); /* shift buffer one to the "left" */
			buffer[7] = ctx->k[i][j];
/*		
			uart_putstr("\r\n  w[");
			uart_putc('0'+(4*i+j)/100);
			uart_putc('0'+((4*i+j)/10)%10);
			uart_putc('0'+(4*i+j)%10);
			uart_putstr("] = ");
			uart_hexdump(&(ctx->k[i][j]), 4);
*/
		}
	}
	for(i=0; i<33; ++i){
		sbox128(ctx->k[i],3-i);
	}
}


void serpent_enc(void* buffer, serpent_ctx_t * ctx){
	uint8_t i;
	for(i=0; i<31; ++i){
		memxor((uint8_t*)buffer, ctx->k[i], 16);
		sbox128(buffer, i);
		lt((uint8_t*)buffer);
	}
	memxor((uint8_t*)buffer, ctx->k[i], 16);
	sbox128(buffer, i);
	++i;
	memxor((uint8_t*)buffer, ctx->k[i], 16);
}

void serpent_dec(void* buffer, serpent_ctx_t * ctx){
	int8_t i=32;
	
	memxor((uint8_t*)buffer, ctx->k[i], 16);
	--i;
	inv_sbox128(buffer, i);
	memxor((uint8_t*)buffer, ctx->k[i], 16);
	--i;
	for(; i>=0; --i){
		inv_lt((uint8_t*)buffer);
		inv_sbox128(buffer, i);
		memxor((uint8_t*)buffer, ctx->k[i], 16);
	}
}






