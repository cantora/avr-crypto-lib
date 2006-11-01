#ifndef SKIPJACK_H_
#define SKIPJACK_H_

#include <stdint.h>

/*****************************************************************************/
/**
 * block is 64 bits (=8 bytes) in size, key is 80 bits (=10 bytes) in size.
 * 
 */
void skipjack_enc(void* block, void* key);
/*****************************************************************************/
/**
 * block is 64 bits (=8 bytes) in size, key is 80 bits (=10 bytes) in size.
 * 
 */
void skipjack_dec(void* block, void* key);

#endif /*SKIPJACK_H_*/
