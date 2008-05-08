#ifndef SHACAL1_ENC_H_
#define SHACAL1_ENC_H_

#include <stdint.h>

#define SHACAL1_BLOCKSIZE 160
#define SHACAL1_BLOCKSIZE_B ((SHACAL1_BLOCKSIZE+7)/8)
#define SHACAL1_KEYSIZE 512
#define SHACAL1_KEYSIZE_B ((SHACAL1_KEYSIZE+7)/8)

void shacal1_enc(void* buffer, void* key, uint16_t keysize_b);

#endif /*SHACAL1_ENC_H_*/
