/**
 * 
 * implementation of HMAC as described in RFC2104
 * Author:	Daniel Otte
 * 
 * License:	GPL
 **/

/* 
 * hmac = hash ( k^opad , hash( k^ipad  , msg))
 */

#include <stdint.h>
#include <string.h>
#include "config.h"
#include "sha256.h"

#define IPAD 0x36
#define OPAD 0x5C

typedef sha256_ctx_t hmac_sha256_ctx_t;

#ifndef HMAC_SHORTONLY

void hmac_sha256_init(hmac_sha256_ctx_t *s, void* key, uint16_t kl){
	uint8_t buffer[SHA256_BLOCK_BITS/8];
	uint8_t i;
	
	if (kl > SHA256_BLOCK_BITS){
		sha256((void*)buffer, key, kl);
	} else {
		memcpy(buffer, key, (kl+7/8));
	}
	
	for (i=0; i<SHA256_BLOCK_BITS/8; ++i){
		buffer[i] ^= IPAD;
	}
	
	sha256_init(s);
	sha256_nextBlock(s, buffer);
#if defined SECURE_WIPE_BUFFER
	memset(buffer, 0, SHA256_BLOCK_BITS/8);
#endif
}

void hmac_sha256_final(hmac_sha256_ctx_t *s, void* key, uint16_t kl){
	uint8_t buffer[SHA256_BLOCK_BITS/8];
	uint8_t i;
	sha256_ctx_t a;
	
	if (kl > SHA256_BLOCK_BITS){
		sha256((void*)buffer, key, kl);
	} else {
		memcpy(buffer, key, (kl+7)/8);
	}
	
	for (i=0; i<SHA256_BLOCK_BITS/8; ++i){
		buffer[i] ^= OPAD;
	}
	
	sha256_init(&a);
	sha256_nextBlock(&a, buffer); /* hash key ^ opad */
	sha256_ctx2hash((void*)buffer, s);  /* copy hash(key ^ ipad, msg) to buffer */
	sha256_lastBlock(s, buffer, SHA256_HASH_BITS);
#if defined SECURE_WIPE_BUFFER
	memset(buffer, 0, SHA256_BLOCK_BITS/8);
	memset(a.h, 0, 8*4);
#endif	
}

#endif

/*
void hmac_sha256_nextBlock()
void hmac_sha256_lastBlock()
*/

/*
 * keylength in bits!
 * message length in bits!
 */
void hmac_sha256(void* dest, void* key, uint16_t kl, void* msg, uint64_t ml){ /* a one-shot*/
	sha256_ctx_t s;
	uint8_t i;
	uint8_t buffer[SHA256_BLOCK_BITS/8];
	
	memset(buffer, 0, SHA256_BLOCK_BITS/8);
	
	/* if key is larger than a block we have to hash it*/
	if (kl > SHA256_BLOCK_BITS){
		sha256((void*)buffer, key, kl);
	} else {
		memcpy(buffer, key, (kl+7)/8);
	}
	
	for (i=0; i<SHA256_BLOCK_BITS/8; ++i){
		buffer[i] ^= IPAD;
	}
	sha256_init(&s);
	sha256_nextBlock(&s, buffer);
	while (ml >= SHA256_BLOCK_BITS){
		sha256_nextBlock(&s, msg);
		msg = (uint8_t*)msg + SHA256_BLOCK_BITS/8;
		ml -=  SHA256_BLOCK_BITS;
	}
	sha256_lastBlock(&s, msg, ml);
	/* since buffer still contains key xor ipad we can do ... */
	for (i=0; i<SHA256_BLOCK_BITS/8; ++i){
		buffer[i] ^= IPAD ^ OPAD;
	}
	sha256_ctx2hash(dest, &s); /* save inner hash temporary to dest */
	sha256_init(&s);
	sha256_nextBlock(&s, buffer);
	sha256_lastBlock(&s, dest, SHA256_HASH_BITS);
	sha256_ctx2hash(dest, &s);
}
