/* 
 * File:	md5.h
 * Author:	Daniel Otte
 * Date: 	31.07.2006
 * License: GPL
 * Description: Implementation of the MD5 hash algorithm as described in RFC 1321
 * 
 */


#ifndef MD5_H_
#define MD5_H_

#include <stdint.h>

typedef struct md5_ctx_st {
	uint32_t a[4];
	uint32_t counter;
} md5_ctx_t;

 
void md5_init(md5_ctx_t *s);
void md5_nextBlock(md5_ctx_t *state, void* block);
void md5_lastBlock(md5_ctx_t *state, void* block, uint16_t length);

#endif /*MD5_H_*/
