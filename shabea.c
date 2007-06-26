/**
 * \file	shabea.c
 * \author	Daniel Otte 
 * \date	2007-06-07
 * \brief	SHABEA - a SHA Based Encrytion Algorithm implementation
 * \par License	
 * GPL
 * 
 * SHABEAn-r where n is the blocksize and r the number of round used
 * 
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#include "config.h"
#include "uart.h"
#include "debug.h"
/*
 * 
 */
void memxor(uint8_t * dest, uint8_t * src, uint8_t length){
	while(length--){
		*dest++ ^= *src++;
	}
} 

/*
 * SHABEA128-16
 */ 
#define L ((uint8_t*)block+0)
#define R ((uint8_t*)block+8)
void shabea128(void * block, void * key, uint16_t keysize, uint8_t enc, uint8_t rounds){
	int8_t r;		/**/
	uint8_t *tb;	/**/
	uint16_t kbs;	/* bytes used for the key / temporary block */
	sha256_hash_t hash;
	
	r = (enc?0:(rounds-1));
	kbs = keysize/8 + ((keysize&7)?1:0);
	tb = malloc(8+2+kbs);
	memcpy(tb+8+2, key, kbs);
	tb[8+0] = 0;
	
	for(;r!=(enc?(rounds):-1);enc?r++:r--){ /* enc: 0..(rounds-1) ; !enc: (rounds-1)..0 */
		memcpy(tb, R, 8); /* copy right half into tb */
		tb[8+1] = r;
		sha256(&hash, tb, 64+16+keysize);
		if(!(r==(enc?(rounds-1):0))){	
			/* swap */
			memxor(hash, L, 8);
			memcpy(L, R, 8);
			memcpy(R, hash, 8);
		} else {
			/* no swap */
			memxor(L, hash, 8);	
		}
	}
	free(tb);
}


