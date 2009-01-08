/* keysize_descriptor.c */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file    keysize_descriptor.c 
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-01-07
 * \license GPLv3 or later
 */

#include <stdint.h>
#include <avr/pgmspace.h>
#include "keysize_descriptor.h"

uint8_t is_valid_keysize_P(PGM_VOID_P ks_desc, uint16_t keysize){
	uint8_t type;
	type = pgm_read_byte(ks_desc++);
	if(type==KS_TYPE_TERMINATOR)
		return 0;
	if(type==KS_TYPE_LIST){
		uint8_t items;
		uint16_t item;
		items = pgm_read_byte(ks_desc++);
		while(items--){
			item = pgm_read_word(ks_desc);
			ks_desc+=2;
			if(item==keysize)
				return 1;
		}
		ks_desc -= 2;
	}
	if(type==KS_TYPE_RANGE){
		uint16_t max, min;
		min = pgm_read_word(ks_desc);
		ks_desc+=2;
		max = pgm_read_word(ks_desc);
		if(min<=keysize && keysize<=max)
			return 1;
	}
	if(type==KS_TYPE_ARG_RANGE){
		uint16_t max, min, dist, offset;
		min = pgm_read_word(ks_desc);
		ks_desc+=2;
		max = pgm_read_word(ks_desc);
		ks_desc+=2;
		dist = pgm_read_word(ks_desc);
		ks_desc+=2;
		offset = pgm_read_word(ks_desc);
		if(min<=keysize && keysize<=max && (keysize%dist==offset))
			return 1;
	}
	if(type>KS_TYPE_ARG_RANGE){
		/* bad error, you may insert a big warning message here */
		return 0;
	}
	return is_valid_keysize(ks_desc+1, keysize) /* search the next record */
}


