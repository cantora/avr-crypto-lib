/* 
 * File:		arcfour.c
 * Author:	Daniel Otte
 * Date: 	07.06.2006
 * License: GPL
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 * 
 */
 
#include <stdint.h>
#include "arcfour.h"

/*
 * length is length of key in bytes!
 */

void arcfour_init(arcfour_ctx_t *c, uint8_t *key, uint8_t length){
	uint8_t t;
	unsigned x,y=0;
	for(x=0; x<= 255; ++x)
		c->s[x]=x;
	
	for(x=0; x<= 255; ++x){
		y += c->s[x] + key[x % length];
		y &= 0xff;
		t = c->s[y];
		c->s[y] = c->s[x];
		c->s[x] = t;
	};
		
	c->i = c->j = 0;
}

uint8_t arcfour_gen(arcfour_ctx_t *c){
	uint8_t t;
	c->i++;
	c->j += c->s[c->i];
	t = c->s[c->j];
	c->s[c->j] = c->s[c->i];
	c->s[c->i] = t;
	return c->s[(c->s[c->j] + c->s[c->i]) & 0xff];
}

