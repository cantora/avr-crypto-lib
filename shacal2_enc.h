#ifndef SHACAL2_ENC_H_
#define SHACAL2_ENC_H_

#include <stdint.h>
#include "sha256.h"

#define SHACAL2_BLOCKSIZE  SHA256_HASH_BITS
#define SHACAL2_BLOCKSIZE_B ((SHACAL2_BLOCKSIZE+7)/8)
#define SHACAL2_KEYSIZE SHA256_BLOCK_BITS
#define SHACAL2_KEYSIZE_B ((SHACAL2_KEYSIZE+7)/8)

void shacal2_enc(void* buffer, void* key, uint16_t keysize_b);


#endif /*SHACAL2_ENC_H_*/
