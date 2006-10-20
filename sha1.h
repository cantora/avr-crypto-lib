/**
 * \file	sha1.c
 * \author	Daniel Otte
 * \date	08.10.2006
 * \par License:
 * GPL
 * \brief SHA-1 declaration.
 * 
 */
 
#ifndef SHA1_H_
#define SHA1_H_

#include <stdint.h>


#define SHA1_HASH_BITS  160
#define SHA1_BLOCK_BITS 512

/**
 * \brief SHA-1 context type
 * 
 */
typedef struct {
	uint32_t h[5];
	uint64_t length;
} sha1_ctx_t;

typedef uint8_t sha1_hash_t[SHA1_HASH_BITS/8];

void sha1_init(sha1_ctx_t *state);

void sha1_nextBlock (sha1_ctx_t *state, void* block);
void sha1_lastBlock (sha1_ctx_t *state, void* block, uint16_t length);

void sha1_ctx2hash (sha1_hash_t *dest, sha1_ctx_t *state);
void sha1 (sha1_hash_t *dest, void* msg, uint32_t length);
//uint32_t change_endian32(uint32_t x);




#endif /*SHA1_H_*/
