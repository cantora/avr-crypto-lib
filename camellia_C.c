/* camellia_C.c */
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
 * 
 * 
 * 
 */
 
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "camellia.h"
#include "uart.h"
#include "debug.h"
#include <util/delay.h>
 
/*****************************************************************************/

uint8_t rol(uint8_t a, uint8_t n){return ((a<<n) | (a>>(8-n)));}

/*****************************************************************************/

uint8_t ror(uint8_t a, uint8_t n){return ((a<<(8-n)) | (a>>n));}

/*****************************************************************************/

uint32_t rol32(uint32_t a, uint8_t n){
	return ((a<<n)|(a>>(32-n)));
}

/*****************************************************************************/

uint64_t rol64(uint64_t a, uint8_t n){
	return ((a<<n)|(a>>(64-n)));
}

/*****************************************************************************/
 
uint8_t camellia_s1_table[256] PROGMEM = {
	112, 130,  44, 236, 179,  39, 192, 229, 228, 133,  87,  53, 234,  12, 174,  65,
	 35, 239, 107, 147,  69,  25, 165,  33, 237,  14,  79,  78,  29, 101, 146, 189,
	134, 184, 175, 143, 124, 235,  31, 206,  62,  48, 220,  95,  94, 197,  11,  26,
	166, 225,  57, 202, 213,  71,  93,  61, 217,   1,  90, 214,  81,  86, 108,  77,
	139,  13, 154, 102, 251, 204, 176,  45, 116,  18,  43,  32, 240, 177, 132, 153,
	223,  76, 203, 194,  52, 126, 118,   5, 109, 183, 169,  49, 209,  23,   4, 215,
	 20,  88,  58,  97, 222,  27,  17,  28,  50,  15, 156,  22,  83,  24, 242,  34,
	254,  68, 207, 178, 195, 181, 122, 145,  36,   8, 232, 168,  96, 252, 105,  80,
	170, 208, 160, 125, 161, 137,  98, 151,  84,  91,  30, 149, 224, 255, 100, 210,
	 16, 196,   0,  72, 163, 247, 117, 219, 138,   3, 230, 218,   9,  63, 221, 148,
	135,  92, 131,   2, 205,  74, 144,  51, 115, 103, 246, 243, 157, 127, 191, 226,
	 82, 155, 216,  38, 200,  55, 198,  59, 129, 150, 111,  75,  19, 190,  99,  46,
	233, 121, 167, 140, 159, 110, 188, 142,  41, 245, 249, 182,  47, 253, 180,  89,
	120, 152,   6, 106, 231,  70, 113, 186, 212,  37, 171,  66, 136, 162, 141, 250,
	114,   7, 185,  85, 248, 238, 172,  10,  54,  73,  42, 104,  60,  56, 241, 164,
	 64,  40, 211, 123, 187, 201,  67, 193,  21, 227, 173, 244, 119, 199, 128, 158
};

/*****************************************************************************/

uint8_t camellia_s1(uint8_t b){
	return pgm_read_byte_near(&(camellia_s1_table[b]));
}

/*****************************************************************************/

uint8_t camellia_s2(uint8_t b){
	return rol(pgm_read_byte_near(&(camellia_s1_table[b])),1);
}

/*****************************************************************************/

uint8_t camellia_s3(uint8_t b){
	return ror(pgm_read_byte_near(&(camellia_s1_table[b])),1);
}

/*****************************************************************************/

uint8_t camellia_s4(uint8_t b){
	return pgm_read_byte_near(&(camellia_s1_table[rol(b,1)]));
}

/*****************************************************************************/

uint64_t camellia_s(uint64_t d){
//	uart_putstr("\n\r S von "); uart_hexdump(&(d), 8);
	#define D ((uint8_t*)(&d))
	D[7] = camellia_s1(D[7]);
	D[6] = camellia_s2(D[6]);
	D[5] = camellia_s3(D[5]);
	D[4] = camellia_s4(D[4]);
	
	D[3] = camellia_s2(D[3]);
	D[2] = camellia_s3(D[2]);
	D[1] = camellia_s4(D[1]);
	D[0] = camellia_s1(D[0]);
	#undef D
//	uart_putstr(" ist "); uart_hexdump(&(d), 8);
	return d;
}

/*****************************************************************************/

uint64_t camellia_p(uint64_t d){
	uint64_t z=0;
	#define D ((uint8_t*)(&d))
	#define Z ((uint8_t*)(&z))
/*
	Z[0] = D[4] ^ D[3] ^ D[1];
	Z[1] = D[5] ^ D[0] ^ D[2];
	Z[2] = D[6] ^ D[1] ^ D[3];
	Z[3] = D[7] ^ D[2] ^ D[0];
	Z[4] = D[0] ^ D[6] ^ D[5];
	Z[5] = D[1] ^ D[7] ^ D[6];
	Z[6] = D[2] ^ D[4] ^ D[7];
	Z[7] = D[3] ^ D[5] ^ D[4];
*/
//	Z[7] = z1 z3 z4 z6 z7 z8
//	uart_putstr("\n\r P von "); uart_hexdump(&(d), 8);
	
	Z[7] = D[7] ^        D[5] ^ D[4] ^        D[2] ^ D[1] ^ D[0];
	Z[6] = D[7] ^ D[6]        ^ D[4] ^ D[3] ^        D[1] ^ D[0];
	Z[5] = D[7] ^ D[6] ^ D[5] ^        D[3] ^ D[2] ^        D[0];
	Z[4] =        D[6] ^ D[5] ^ D[4] ^ D[3] ^ D[2] ^ D[1]       ;
	Z[3] = D[7] ^ D[6] ^                      D[2] ^ D[1] ^ D[0];
	Z[2] =        D[6] ^ D[5] ^        D[3] ^        D[1] ^ D[0];
	Z[1] =               D[5] ^ D[4] ^ D[3] ^ D[2] ^        D[0];
	Z[0] = D[7] ^               D[4] ^ D[3] ^ D[2] ^ D[1]       ;
	
//	uart_putstr(" ist "); uart_hexdump(&(z), 8);
			
	#undef Z
	#undef D
	return z;
}

/*****************************************************************************/

uint64_t camellia_f(uint64_t x, uint64_t k){
	uint64_t y;
	y = camellia_p(camellia_s(x ^ k));
/*
	uart_putstr("\r\nEfunc X=");
	uart_hexdump(&(x), 8);
	uart_putstr("  K=");
	uart_hexdump(&(k), 8);
	uart_putstr("  Y=");
	uart_hexdump(&(y), 8);
*/	
	return y;
}

/*****************************************************************************/

uint64_t camellia_fl(uint64_t x, uint64_t k){
	uint64_t lx[1], lk[1], y[1];
	lx[0]=x; lk[0] = k; 
	
	#define Y ((uint32_t*)y)
	#define X ((uint32_t*)lx)
	#define K ((uint32_t*)lk)

	Y[0] = rol32((X[1]) & K[1] ,1) ^ (X[0]); /* Yr */
	Y[1] = (Y[0] | K[0]) ^ (X[1]);           /* Yl */
	
/*	
	uart_putstr("\r\nFL(");
	uart_hexdump(&(x), 8);
	uart_putstr(", ");
	uart_hexdump(&(k), 8);
	uart_putstr(") = ");
	uart_hexdump(y, 8);
*/
	#undef K
	#undef X
	#undef Y
	return y[0];	
}

/*****************************************************************************/

uint64_t camellia_fl_inv(uint64_t y, uint64_t k){
//volatile	uint32_t xl, xr;
	volatile uint64_t ly[1], lk[1], x[1];
	ly[0]=y; lk[0] = k; 
	#define Y ((uint32_t*)ly) 
	#define X ((uint32_t*)x) 
	#define K ((uint32_t*)lk) 
	
	X[1]=(Y[0] | K[0]) ^ Y[1];
	X[0]=rol32((X[1] & K[1]),1) ^ Y[0];
	
/*	
	uart_putstr("\r\nFL_inv(");
	uart_hexdump(&(y), 8);
	uart_putstr(", ");
	uart_hexdump(&(k), 8);
	uart_putstr(") = ");
*/	
	#undef K
	#undef X
	#undef Y
	return x[0];	
}

/*****************************************************************************/

uint64_t camellia_sigma[6]={
	0xA09E667F3BCC908BLL,
	0xB67AE8584CAA73B2LL,
	0xC6EF372FE94F82BELL,
	0x54FF53A5F1D36F1CLL,
	0x10E527FADE682D1DLL,
	0xB05688C2B3E6C1FDLL
};	

/*****************************************************************************/

void camellia128_ctx_dump(camellia128_ctx_t *s){
	uart_putstr("\r\n==State Dump==");
	uart_putstr("\n\rKAl: "); uart_hexdump(&(s->kal), 8);
	uart_putstr("\n\rKAr: "); uart_hexdump(&(s->kar), 8);
	uart_putstr("\n\rKLl: "); uart_hexdump(&(s->kll), 8);
	uart_putstr("\n\rKLr: "); uart_hexdump(&(s->klr), 8);	
	return;
}

/*****************************************************************************/

void camellia128_init(const uint8_t* key, camellia128_ctx_t* s){
	uint8_t i;
	s->kll = 0; //((uint64_t*)key)[0];
	
	/* load the key, endian-adjusted, to kll,klr */
	for(i=0; i<8; ++i){
		s->kll <<= 8;
		s->kll |= *key++;
	}
	for(i=0; i<8; ++i){
		s->klr <<= 8;
		s->klr |= *key++;
	}
	
	s->kal = s->kll;
	s->kar = s->klr;
	
	s->kar ^= camellia_f(s->kal, camellia_sigma[0]);
	s->kal ^= camellia_f(s->kar, camellia_sigma[1]);
	
	s->kal ^= s->kll;
	s->kar ^= s->klr;
	
	s->kar ^= camellia_f(s->kal, camellia_sigma[2]);
	s->kal ^= camellia_f(s->kar, camellia_sigma[3]);
	/**/
//	uart_putstr("\n\r----------------init finished--------------------");
}

/*****************************************************************************/

void camellia128_keyop(camellia128_ctx_t* s, int8_t q){
	/* first we do 16 bit left-rols for kl and ka (128bit each) */
	uint32_t temp;
	
	temp = (s->kal)>>(64-16-q);
	s->kal = s->kal<<(16+q) | s->kar>>(64-16-q);
	s->kar = s->kar<<(16+q) | temp;
	
	temp = (s->kll)>>(64-16-q);
	s->kll = s->kll<<(16+q) | s->klr>>(64-16-q);
	s->klr = s->klr<<(16+q) | temp;
	/* after doing the 16-bit rol we have to rol 1 bit left or rigth depending on q */
}

/*****************************************************************************/

void camellia128_keyop_inv(camellia128_ctx_t* s, int8_t q){
	/* first we do 16 bit right-rols for kl and ka (128bit each) */
	uint32_t temp;
	
	temp = (s->kar)&(0xffffff>>(24-16-q));
	s->kar = s->kar>>(16+q) | s->kal<<(64-16-q);
	s->kal = s->kal>>(16+q) | ((uint64_t)temp)<<(64-16-q);
	
	temp = (s->klr)&(0xffffff>>(24-16-q));
	s->klr = s->klr>>(16+q) | s->kll<<(64-16-q);
	s->kll = s->kll>>(16+q) | ((uint64_t)temp)<<(64-16-q);
	/* after doing the 16-bit rol we have to rol 1 bit left or rigth depending on q */
}

/*****************************************************************************/

#define SEL_KA 1
#define SEL_KL 0

#define KEY_POSTC1		0x00
#define KEY_POSTC2		0x01
#define KEY_INC2		0x02

#define KEY_DIR			0x04
#define KEY_DIR_NORM	0x00
#define KEY_DIR_INV		0x04

#define KEY_AMMOUNT		0x08 
#define KEY_ROL17		0x08
#define KEY_ROL15		0x00

void camellia_6rounds(const camellia128_ctx_t* s, uint64_t* bl, uint64_t* br, uint8_t roundop, uint8_t keychoice){
	uint8_t i;
	uint64_t* k[4];
	k[0] = &(s->kll);
	k[1] = &(s->klr);
	k[2] = &(s->kal);
	k[3] = &(s->kar);
	for(i=0; i<3; ++i){ /* each cycle */
		br[0] ^= camellia_f(bl[0],*(k[(keychoice&1)*2+((roundop&KEY_DIR)?1:0)]));
		keychoice >>= 1;
		
		if((i == 1) && (roundop&KEY_INC2)){
			((roundop&KEY_DIR)?camellia128_keyop_inv:camellia128_keyop)(s,(roundop&KEY_AMMOUNT)?1:-1);
		}
		
		bl[0] ^= camellia_f(br[0],*(k[(keychoice&1)*2+((roundop&KEY_DIR)?0:1)]));
		keychoice >>= 1;
		
		/* check if we should do some keyop */
		if((i == (roundop&1)) && (!(roundop&KEY_INC2)) ){
			((roundop&KEY_DIR)?camellia128_keyop_inv:camellia128_keyop)(s,(roundop&KEY_AMMOUNT)?1:-1);
			/* isn't it fuckin nice what we can do in C?! */
		}
	}
}

/*****************************************************************************/


void camellia128_enc(void* block, const camellia128_ctx_t* s){

	#define BL (((uint64_t*)block)[0])
	#define BR (((uint64_t*)block)[1])
	/* endian adjustment */
	 /*BL*/
	 /* 1 2 3 4 5 6 7 8
	  *	8 7 6 5 4 3 2 1
	  */
	 
	uint64_t temp64;

	temp64 = BL;
	BL = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
	temp64 = BR;
	BR = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
	
	/* Prewhitening */
	BL ^= s->kll;
	BR ^= s->klr;
	
	/* the first 6 */
/*	
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->kar);
   camellia128_keyop(s, -1);
	BR ^= camellia_f(BL, s->kll);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->kar);
*/
	camellia_6rounds(s, &BL, &BR, KEY_ROL15 | KEY_DIR_NORM | KEY_POSTC1 , 0x33);
	/* FL injection  */
   camellia128_keyop((camellia128_ctx_t*)s, -1);
	BL = camellia_fl(BL, s->kal);
	BR = camellia_fl_inv(BR, s->kar);
   camellia128_keyop((camellia128_ctx_t*)s, -1);
	/* middle 6 */
/*	BR ^= camellia_f(BL, s->kll);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kal);
   camellia128_keyop(s, -1);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->kar);
/*/
	camellia_6rounds(s, &BL, &BR, KEY_ROL15 | KEY_DIR_NORM | KEY_INC2 , 0x34);
	/* FL injection  */
   camellia128_keyop((camellia128_ctx_t*)s, 1);
   	BL = camellia_fl(BL, s->kll);
	BR = camellia_fl_inv(BR, s->klr);
   camellia128_keyop((camellia128_ctx_t*)s, 1);
   /* last 6 */
/*	BR ^= camellia_f(BL, s->kll);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->kar);
   camellia128_keyop(s, 1);
  
	BR ^= camellia_f(BL, s->kll);
	BL ^= camellia_f(BR, s->klr);
*/
	camellia_6rounds(s, &BL, &BR, KEY_ROL17 | KEY_DIR_NORM | KEY_POSTC2 , 0x0C);
	/* Postwhitening */
	BR ^= s->kal;
	BL ^= s->kar;
	
	temp64 = BR;
	BR = BL;
	BL = temp64;

	
	BL = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
	temp64 = BR;
	BR = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
		
	#undef BL
	#undef BR	
}

/*****************************************************************************/

void camellia128_dec(void* block, const camellia128_ctx_t* s){

	#define BL (((uint64_t*)block)[1])
	#define BR (((uint64_t*)block)[0])
	/* endian adjustment */
	 /*BL*/
	 /* 1 2 3 4 5 6 7 8
	  * 8 7 6 5 4 3 2 1
	  */
	 
	uint64_t temp64;
	
	temp64 = BL;

	BL = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
	temp64 = BR;
	BR = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
		
	camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	/* Prewhitening */
	BR ^= s->kal; /* kw3 */
	BL ^= s->kar; /* kw4 */

	/* the first 6 */
/*	
	BL ^= camellia_f(BR, s->klr); /* k18 * /
	BR ^= camellia_f(BL, s->kll); /* k17 * /
   camellia128_keyop_inv(s, 1);
	BL ^= camellia_f(BR, s->kar);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kll);
*/
	camellia_6rounds(s, &BR, &BL, KEY_ROL17 | KEY_DIR_INV | KEY_POSTC1 , 0x0C);

	/* FL injection  */
   camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	BR = camellia_fl(BR, s->klr);
	BL = camellia_fl_inv(BL, s->kll);
   camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	/* middle 6 */	
/*	BL ^= camellia_f(BR, s->kar);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->klr);
   camellia128_keyop_inv(s, -1);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kll);
*/
	camellia_6rounds(s, &BR, &BL, KEY_ROL15 | KEY_DIR_INV | KEY_INC2 , 0x0B);

	/* FL injection  */
   camellia128_keyop_inv(s, -1);
   	BR = camellia_fl(BR, s->kar);
	BL = camellia_fl_inv(BL, s->kal);
   camellia128_keyop_inv((camellia128_ctx_t*)s, -1);
   /* last 6 */
/*   
	BL ^= camellia_f(BR, s->kar);
	BR ^= camellia_f(BL, s->kal);
	BL ^= camellia_f(BR, s->klr);
	BR ^= camellia_f(BL, s->kll);
   camellia128_keyop_inv(s, -1);
	BL ^= camellia_f(BR, s->kar);
	BR ^= camellia_f(BL, s->kal);
/*/
	camellia_6rounds(s, &BR, &BL, KEY_ROL15 | KEY_DIR_INV | KEY_POSTC2 , 0x33);
	
	/* Postwhitening */
	BL ^= s->kll; /* kw1 */ 
	BR ^= s->klr; /* kw2 */
	
	temp64 = BR;
	BR = BL;
	BL = temp64;

	
	BL = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
	temp64 = BR;
	BR = temp64 >> 56 | temp64 << 56  /* swap the most out bytes (1 & 8) */
		| (temp64 & (0xffLL<<48))>>(5*8) | (temp64 & (0xffLL<< 8))<<(5*8) /* 2 & 7 */
		| (temp64 & (0xffLL<<40))>>(3*8) | (temp64 & (0xffLL<<16))<<(3*8) /* 3 & 6 */		
		| (temp64 & (0xffLL<<32))>>(1*8) | (temp64 & (0xffLL<<24))<<(1*8); /* 4 & 5 */
		
}

/*****************************************************************************/
/*****************************************************************************/



/* EOF */
