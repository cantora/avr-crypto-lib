/* arcfour.c */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

