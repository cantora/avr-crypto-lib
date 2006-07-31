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



/*
 * dest: the destination where result of operation will be placed (64 bit)
 * v:	 the block to operate on (64 bit)
 * k:	 the key for en/decryption (128 bit)
 */
void xtea_enc(uint32_t* dest, uint32_t* v, uint32_t* k);
void xtea_dec(uint32_t* dest, uint32_t* v, uint32_t* k);


#endif /*XTEA_H_*/
