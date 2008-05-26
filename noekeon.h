/* noekeon.h */
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
#ifndef NOEKEON_H_
#define NOEKEON_H_

/*
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * 
 * 
 */

#include <stdint.h>

typedef uint8_t noekeon_ctx_t[16];

void noekeon_enc(void* buffer, void* key);
void noekeon_dec(void* buffer, void* key);
void noekeon_init(void* key, noekeon_ctx_t* ctx);

#endif /*NOEKEON_H_*/
