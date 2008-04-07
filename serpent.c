/* serpent.c
 * a bitsliced implementation of the serpent cipher for avr microcontrollers
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 */

#include <stdint.h>
#include <string.h> /* memset() */
#include <avr/pgmspace.h>
#include "serpent.h"
#include "serpent-sboxes.h"

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

#define GOLDEN_RATIO 0x9e3779b9l

static uint32_t gen_w(uint32_t * b, uint8_t i){
	uint32_t ret;
	ret = b[0] ^ b[3] ^ b[5] ^ b[7] ^ GOLDEN_RATIO ^ (uint32_t)i;
	ret = rotl32(ret, 11);
	return ret;
} 

/* key must be 256bit (32 byte) large! */
void serpent_genctx(void * key, uint8_t keysize, serpent_ctx_t * ctx){
	uint32_t buffer[8];
	uint8_t i,j;
	if(keysize){
		/* keysize is less than 256 bit, padding needed */
		memset(buffer, 0, 32);
		memcpy(buffer, key, (keysize+7)/8);
		((uint8_t*)buffer)[keysize/8] |= 1<<(keysize%8);
	} else {
		/* keysize is 256 bit */
		memcpy(buffer, key, 32); 
	}
	for(i=0; i<33; ++i){
		for(j=0; j<4; ++j){
			ctx->k[i][j] = gen_w(buffer, i*4+j);
			memmove(buffer, &(buffer[1]), 7*4); /* shift buffer one to the "left" */
			buffer[7] = ctx->k[i][j];
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






