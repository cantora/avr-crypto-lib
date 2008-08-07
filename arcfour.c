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
 * File:        arcfour.c
 * Author:      Daniel Otte
 * email:       daniel.otte@rub.de
 * Date:        2006-06-07
 * License:     GPLv3 or later
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 * 
 */
 
#include <stdint.h>
#include "arcfour.h"

/*
 * length is length of key in bytes!
 */

void arcfour_init(const void *key, uint8_t length_B, arcfour_ctx_t *ctx){
	uint8_t t;
	uint16_t x,y=0;
	for(x=0; x<= 255; ++x)
		ctx->s[x]=x;
	
	for(x=0; x<= 255; ++x){
		y += ctx->s[x] + ((uint8_t*)key)[x % length_B];
		y &= 0xff;
		/* ctx->s[y] <--> ctx->s[x] */
		t = ctx->s[y];
		ctx->s[y] = ctx->s[x];
		ctx->s[x] = t;
	}	
	ctx->i = ctx->j = 0;
}

uint8_t arcfour_gen(arcfour_ctx_t *ctx){
	uint8_t t;
	ctx->i++;
	ctx->j += ctx->s[ctx->i];
	/* ctx->s[i] <--> ctx->s[j] */
	t = ctx->s[ctx->j];
	ctx->s[ctx->j] = ctx->s[ctx->i];
	ctx->s[ctx->i] = t;
	return ctx->s[(ctx->s[ctx->j] + ctx->s[ctx->i]) & 0xff];
}

