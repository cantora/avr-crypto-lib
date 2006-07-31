/* 
 * File:	cast5.h
 * Author:	Daniel Otte
 * Date: 	26.07.2006
 * License: GPL
 * Description: Implementation of the CAST5 (aka CAST-128) cipher algorithm as described in RFC 2144
 * 
 */
#ifndef CAST5_H_
#define CAST5_H_ 

#include <stdint.h> 

#ifndef BOOL
#define BOOL
 #ifndef __BOOL
 #define __BOOL
  #ifndef __BOOL__
  #define __BOOL__
	typedef enum{false=0,true=1} bool;
  #endif
 #endif
#endif


typedef struct cast5_ctx_st{
	uint32_t	mask[16];
	uint8_t		rotl[8];	/* 4 bit from every rotation key is stored here */
	uint8_t		roth[2];	/* 1 bit from every rotation key is stored here */
	bool		shortkey;
} cast5_ctx_t;

void cast5_init(cast5_ctx_t* s, uint8_t* key, uint8_t keylength);
void cast5_enc(cast5_ctx_t *s, void* block);
void cast5_dec(cast5_ctx_t *s, void* block);



#endif

