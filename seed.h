/**
 * \file	seed.h
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief 	declarations for seed
 * \par License	
 * GPL
 * 
 */
#ifndef SEED_H_
#define SEED_H_

#include <stdint.h>

typedef struct{
	uint32_t k[4];
} seed_ctx_t;

/******************************************************************************/

void seed_init(seed_ctx_t * ctx, uint8_t * key);
void seed_encrypt(seed_ctx_t * ctx, void * buffer);
void seed_decrypt(seed_ctx_t * ctx, void * buffer);
	
#endif /*SEED_H_*/
