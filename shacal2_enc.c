/**
 * \file	shacal2_enc.c
 * \author	Daniel Otte
 * \date	2008-05-07
 * \par License:
 * GPL
 * \brief SHACAL2 encryption only implementation.
 * 
 */

#include <stdint.h>
#include <string.h>
#include "sha256.h"
#include "shacal2_enc.h"


void shacal2_enc(void* buffer, void* key, uint16_t keysize_b){
	uint8_t i;
	sha256_ctx_t ctx, t_ctx;
	memcpy(ctx.h, buffer, SHACAL2_BLOCKSIZE_B);
	
	uint8_t keybuffer[SHACAL2_KEYSIZE_B];
	memset(keybuffer, 0, SHACAL2_KEYSIZE_B);
	if(keysize_b>SHACAL2_KEYSIZE)
		keysize_b=SHACAL2_KEYSIZE;
	memcpy(keybuffer, key, (keysize_b+7)/8);
	
	memcpy(t_ctx.h, buffer, SHACAL2_BLOCKSIZE_B);
	sha256_ctx2hash((sha256_hash_t*)(&(ctx.h[0])), &t_ctx);
	memcpy(t_ctx.h, ctx.h, SHACAL2_BLOCKSIZE_B);
	sha256_nextBlock(&ctx, keybuffer);
	for(i=0; i<SHACAL2_BLOCKSIZE/32; ++i){
		ctx.h[i] -= t_ctx.h[i];
	}
	sha256_ctx2hash(buffer, &ctx);
}



