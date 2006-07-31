/**
 * File:		prng.c
 * Author:		Daniel Otte
 * Date:		17.05.2006
 * License:		GPL
 * Description:	This file contains an implementaition of a pseudo-random-number generator.
 * Extension 1:
 * 	rndCore is expanded to 512 bits for more security.
 **/


/*
 * 
 *                      ####################################################################################
 *                      #        																	       #
 *					    #		  +---------------------------+										 	   #
 * 					    #	  	  |	        			      |											   #
 * 					    #		  V				              |				  							   #
 *                      #      (concat)	            		  |											   #
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o       o---------o   #    +--------------+
 *  | entropy Block | -----> | sha-256 | --(offset)-<     | rndCore | ---> | sha-256 | --+-> | sha-256 | -----> | random Block |
 *  +---------------+   #    o---------o             (xor)+---------+      o---------o   |   o---------o   #    +--------------+
 * 						#        					      (xor)	(xor)					 |				   #
 * 						#		        				    ^	  ^						 |				   #
 * 						#				        		     \   /						 |				   #
 * 						#						            (offset)---------------------+				   #
 * 						#		        															       #
 *						####################################################################################
 * 
 */

#include <stdint.h>
#include <string.h>
#include "sha256.h"




uint32_t rndCore[16]; /* secret */

/*
 * idea is: hash the message and add it via xor to rndCore
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
		data += 512/8;
		length -= 512;	
	}
	sha256_lastBlock(&s, data, length);
	uint8_t i;
	for (i=0; i<8; ++i){
		rndCore[i+offset] ^= s.h[i];
	}
	offset ^= 8; /* hehe */
}
 
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
	sha256_init(&s);
	sha256_lastBlock(&s, b, 256);
	memcpy(b, s.h, 32);
}
 
/* this does some simple buffering */
uint8_t getRandomByte(void){
	static uint8_t block[32];
	static uint8_t i=32;
	
	if (i==32){
		getRandomBlock((void*)block);
		i=0;
	}	
	return block[i++];
}
 
 
