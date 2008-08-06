#ifndef NOEKEON_OMAC_H_
#define NOEKEON_OMAC_H_

#include "noekeon.h"
#include <stdint.h>

void noekeon_omac(void* dest, const void* msg, uint16_t msglength_b,
                  const void* key, uint8_t t);

#endif /*NOEKEON_OMAC_H_*/
