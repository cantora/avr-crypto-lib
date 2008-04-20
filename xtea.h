/*
 * Author:	Daniel Otte
 * Date:		06.06.2006
 * License:	GPL
 */

#ifndef XTEA_H_
#define XTEA_H_

#include <stdint.h> 
 
/*
 * this fits for xtea.c and xtea-asm.S
 * 
 */
#define XTEA_BLOCKSIZE 64
#define XTEA_BLOCKSIZEB ((XTEA_BLOCKSIZE+7)/8)
#define XTEA_KEYSIZE 128
#define XTEA_KEYSIZEB ((XTEA_KEYSIZE+7)/8)


/*
 * dest: the destination where result of operation will be placed (64 bit)
 * v:	 the block to operate on (64 bit)
 * k:	 the key for en/decryption (128 bit)
 */
void xtea_enc(void* dest, const void* v, const void* k);
void xtea_dec(void* dest, const void* v, const void* k);


#endif /*XTEA_H_*/
