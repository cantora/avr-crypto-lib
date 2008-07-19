/* seed.h */
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
/**
 * \file	seed.h
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief 	declarations for seed
 * \par License	
 * GPL
 * 
 */
#ifndef SEED_H_
#define SEED_H_

#include <stdint.h>

typedef struct{
	uint32_t k[4];
} seed_ctx_t;

/******************************************************************************/

void seed_init(uint8_t * key, seed_ctx_t * ctx);
void seed_enc(void * buffer, seed_ctx_t * ctx);
void seed_dec(void * buffer, seed_ctx_t * ctx);
	
#endif /*SEED_H_*/
