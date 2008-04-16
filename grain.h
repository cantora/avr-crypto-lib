/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 */

#ifndef GRAIN_H_
#define GRAIN_H_


#include <stdint.h>

typedef struct gain_ctx_st{
	uint8_t lfsr[10];
	uint8_t nfsr[10];
} grain_ctx_t;


uint8_t grain_enc(grain_ctx_t* ctx);
void grain_init(const void* key, const void* iv, grain_ctx_t* ctx);

#endif /*GRAIN_H_*/
