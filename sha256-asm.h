/**
 * \file		sha256-asm.h
 * \author	Daniel Otte 
 * \date		16.05.2006
 * \par License	
 * GPL
 * 
 */

#ifndef SHA256ASM_H_
#define SHA256ASM_H_


#define SHA256_HASH_BITS  256
#define SHA256_BLOCK_BITS 512

typedef struct {
	uint32_t h[8];
	uint64_t length;
} sha256_ctx_t;

typedef uint8_t sha256_hash_t[SHA256_HASH_BITS/8];

void sha256_ctx2hash(sha256_hash_t *dest, sha256_ctx_t *state);
void sha256(sha256_hash_t *dest, void* msg, uint32_t length);
void sha256_init(sha256_ctx_t *state);
void sha256_nextBlock(sha256_ctx_t *state, void* block);
void sha256_lastBlock(sha256_ctx_t *state, void* block, uint16_t length);
uint32_t rotr32(uint32_t, uint8_t);
uint32_t change_endian32(uint32_t x);

#endif /*SHA256ASM_H_*/
