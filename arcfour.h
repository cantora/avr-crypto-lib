/* 
 * File:	arcfour.h
 * Author:	Daniel Otte
 * Date: 	07.06.2006
 * License: GPL
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 * 
 */
#ifndef ARCFOUR_H_
#define ARCFOUR_H_

#include <stdint.h>

typedef struct {
	uint8_t i,j;
	uint8_t s[256];
} arcfour_ctx_t;
 

void arcfour_init(arcfour_ctx_t *c, uint8_t *key, uint8_t length);
uint8_t arcfour_gen(arcfour_ctx_t *c);

#endif
