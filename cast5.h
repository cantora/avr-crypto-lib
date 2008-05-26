/* cast5.h */
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

