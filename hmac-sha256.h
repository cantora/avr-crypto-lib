/* hmac-sha256.h */
/*
    This file is part of the This file is part of the AVR-Crypto-Lib.
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
#ifndef HMACSHA256_H_
#define HMACSHA256_H_

#include "sha256.h"

#define HMAC_BITS SHA256_HASH_BITS
#define HMAC_BYTES ((HMAC_BITS+7)/8)

typedef sha256_ctx_t hmac_sha256_ctx_t;


void hmac_sha256_init(hmac_sha256_ctx_t *s, void* key, uint16_t keylength_b);
void hmac_sha256_final(hmac_sha256_ctx_t *s, void* key, uint16_t keylength_b);
void hmac_sha256(void* dest, void* key, uint16_t keylength_b, void* msg, uint64_t msglength_b);


#endif /*HMACSHA256_H_*/
