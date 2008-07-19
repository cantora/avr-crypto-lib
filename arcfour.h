/* arcfour.h */
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
 * File:	arcfour.h
 * Author:	Daniel Otte
 * Date: 	2006-06-07
 * License: GPL
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm. 
 */
/* 
 * \file	arcfour.h
 * \author	Daniel Otte
 * \date 	2006-06-07
 * \par License
 *      GPL
 * \brief   Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm. 
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
