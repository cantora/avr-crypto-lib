#ifndef NOEKEON_H_
#define NOEKEON_H_

/*
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * 
 * 
 */

#include <stdint.h>

typedef uint8_t noekeon_ctx_t[16];

void noekeon_enc(void* buffer, void* key);
void noekeon_dec(void* buffer, void* key);
void noekeon_init(void* key, noekeon_ctx_t* ctx);

#endif /*NOEKEON_H_*/
