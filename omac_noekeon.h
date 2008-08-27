#ifndef NOEKEON_OMAC_H_
#define NOEKEON_OMAC_H_

#include "noekeon.h"
#include <stdint.h>

typedef uint8_t noekeon_omac_ctx_t[16];

void omac_noekeon_init(noekeon_omac_ctx_t* ctx);
void omac_noekeont_tweak(uint8_t t, const void* key, noekeon_omac_ctx_t* ctx);
void omac_noekeon_next(const void* buffer, const void* key, 
                       noekeon_omac_ctx_t* ctx);
void omac_noekeon_last(const void* buffer, uint8_t length_b, const void* key, 
                       noekeon_omac_ctx_t* ctx);
void omac_noekeon(void* dest, const void* msg, uint16_t msglength_b,
                  const void* key, uint8_t t);

#endif /*NOEKEON_OMAC_H_*/
