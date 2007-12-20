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
 * SHABEA256-n
 */ 
 
#define BLOCKSIZE 256
#define BLOCKSIZEB (BLOCKSIZE/8)
#define HALFSIZEB  (BLOCKSIZEB/2)
#define HALFSIZE (BLOCKSIZE/2)

#define L ((uint8_t*)block+ 0)
#define R ((uint8_t*)block+16)
void shabea256(void * block, void * key, uint16_t keysize, uint8_t enc, uint8_t rounds){
	int8_t r;		/**/
	uint8_t *tb;	/**/
	uint16_t kbs;	/* bytes used for the key / temporary block */
	sha256_hash_t hash;
	
	r = (enc?0:(rounds-1));
	kbs = (keysize+7)/8;
	tb = malloc(HALFSIZEB+2+kbs);
	memcpy(tb+HALFSIZEB+2, key, kbs); /* copy key to temporary block */
	tb[HALFSIZEB+0] = 0;	/* set round counter high value to zero */
	
	for(;r!=(enc?(rounds):-1);enc?r++:r--){ /* enc: 0..(rounds-1) ; !enc: (rounds-1)..0 */
		memcpy(tb, R, HALFSIZEB); /* copy right half into tb */
		tb[HALFSIZEB+1] = r;
		sha256(&hash, tb, HALFSIZE+16+keysize);
		if(!(r==(enc?(rounds-1):0))){	
			/* swap */
			memxor(hash, L, HALFSIZE);
			memcpy(L, R, HALFSIZE);
			memcpy(R, hash, HALFSIZE);
		} else {
			/* no swap */
			memxor(L, hash, HALFSIZE);	
		}
	}
	free(tb);
}


