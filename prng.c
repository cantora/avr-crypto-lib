/**
 * \file		prng.c
 * \author		Daniel Otte
 * \date		17.05.2006
 * \par License:
 * 	GPL
 * \brief	This file contains an implementaition of a pseudo-random-number generator.
 * 
 * Extension 1:
 * 	rndCore is expanded to 512 bits for more security.
 *
 * \verbatim
 *                      ################################################################################################
 *                      #                                                                                              #
 *                      #         +---------------------------+                                                        #
 *                      #         |                           |                             +---+                      #
 *                      #         V                           |                             |   |                      #
 *                      #      (concat)                       |                             |   V                      #
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o      | o----o     o---------o   #    +--------------+
 *  | entropy Block | -----> | sha-256 | --(offset)-<     | rndCore | ---> | sha-256 | --+--+-| +1 |---> | sha-256 | -----> | random Block |
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o   |    o----o     o---------o   #    +--------------+
 *                      #                                 (xor) (xor)                    |                             #
 *                      #                                   ^     ^                      |                             #
 *                      #                                    \   /                       |                             #
 *                      #                                   (offset)---------------------+                             #
 *                      #                                                                                              #
 *                      ################################################################################################
 * \endverbatim
 */

 /* \verbatim
 *                      ################################################################################################
 *                      #                                                                                              #
 *                      #         +---------------------------+                                                        #
 *                      #         |                           |                             +---+                      #
 *                      #         V                           |                             |   |                      #
 *                      #      (concat)                       |                             |   V                      #
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o      | o----o     o---------o   #    +--------------+
 *  | entropy Block | -----> | sha-256 | --(offset)-<     | rndCore | ---> | sha-256 | --+--+-| +1 |---> | sha-256 | -----> | random Block |
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o   |    o----o     o---------o   #    +--------------+
 *                      #                                 (xor) (xor)                    |                             #
 *                      #                                   ^     ^                      |                             #
 *                      #                                    \   /                       |                             #
 *                      #                                   (offset)---------------------+                             #
 *                      #                                                                                              #
 *                      ################################################################################################
 * \endverbatim
 */

#include <stdint.h>
#include <string.h>
#include "sha256.h"
#include "prng.h"

/**
 * \brief secret entropy pool. 
 * This is the core of the random which is generated
 */
uint32_t rndCore[16]; 

/*************************************************************************/

/**
 * \brief This function adds entropy to the central entropy pool
 * 
 * @param length This ist the length of the random data in BITS. 
 * @param data This is the random data which should be added to the entropy pool
*/
/* idea is: hash the message and add it via xor to rndCore
 *
 * length in bits 
 * 
 * we simply first "hash" rndCore, then entropy.
 */
void addEntropy(unsigned length, void* data){
	sha256_ctx_t s;
	static uint8_t offset=0; /* selects if higher or lower half gets updated */
	sha256_init(&s);
	sha256_nextBlock(&s, rndCore);
	while (length>=512){
		sha256_nextBlock(&s, data);
		data = (uint8_t*)data+ 512/8;
		length -= 512;	
	}
	sha256_lastBlock(&s, data, length);
	uint8_t i;
	for (i=0; i<8; ++i){
		rndCore[i+offset] ^= s.h[i];
	}
	offset ^= 8; /* hehe */
}

/*************************************************************************/
/**
 * \brief This function fills a given buffer with 32 random bytes
 * @param b Pointer to buffer wich is to fill
 */
void getRandomBlock(uint32_t *b){
	sha256_ctx_t s;
	uint8_t offset=8;
	
	sha256_init(&s);
	sha256_lastBlock(&s, rndCore, 512); /* remeber the byte order! */
	uint8_t i;
	for (i=0; i<8; ++i){
		rndCore[i+offset] ^= s.h[i];
	}
	offset ^= 8; /* hehe */
	memcpy(b, s.h, 32); /* back up first hash in b */
	((uint8_t*)b)[*b&31]++; 	/* the important increment step */
	sha256_init(&s);
	sha256_lastBlock(&s, b, 256);
	memcpy(b, s.h, 32);
}

/*************************************************************************/
 
/**
 * \brief This function simply returns a random byte
 * @return a random byte
 */
uint8_t getRandomByte(void){
	static uint8_t block[32];
	static uint8_t i=32;
	
	if (i==32){
		getRandomBlock((void*)block);
		i=0;
	}	
	return block[i++];
}

/*************************************************************************/
 
/**
 * \brief This function fills the given bock with length random bytes
 * @return a random byte
 */
 
void fillBlockRandom(void* block, unsigned length){
	while(length>RANDOMBLOCK_SIZE){
		getRandomBlock(block);
		block += RANDOMBLOCK_SIZE;
		length -= RANDOMBLOCK_SIZE;
	}
	while(length){
		*((uint8_t*)block) = getRandomByte();
		++block; --length;
	}
}
 
 
