/* hmac-sha256.h */
/*
    This file is part of the AVR-Crypto-Lib.
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
#ifndef HMACSHA1_H_
#define HMACSHA1_H_

#include "sha1.h"

#define HMACSHA1_BITS SHA1_HASH_BITS
#define HMACSHA1_BYTES ((HMAC_BITS+7)/8)

typedef sha1_ctx_t hmac_sha1_ctx_t;


void hmac_sha1_init(hmac_sha1_ctx_t *s, void* key, uint16_t keylength_b);
void hmac_sha1_final(hmac_sha1_ctx_t *s, void* key, uint16_t keylength_b);
void hmac_sha1(void* dest, void* key, uint16_t keylength_b, void* msg, uint64_t msglength_b);


#endif /*HMACSHA1_H_*/
