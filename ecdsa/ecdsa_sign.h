/* ecdsa_sign.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2012 Daniel Otte (daniel.otte@rub.de)

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

#ifndef ECDSA_SIGN_H_
#define ECDSA_SIGN_H_

#include "bigint.h"
#include "ecc.h"
#include "ecdsa.h"
#include "hfal-basic.h"

uint8_t ecdsa_signature_alloc(ecdsa_signature_t *s, size_t length_B);

void ecdsa_signature_free(ecdsa_signature_t *s);

uint8_t ecdsa_sign_bigint(ecdsa_signature_t *s, const bigint_t *m,
                          const ecdsa_ctx_t *ctx, const bigint_t *k);

uint8_t ecdsa_sign_hash(ecdsa_signature_t *s, const void *hash,
                           size_t hash_len_B, const ecdsa_ctx_t *ctx,
                           const void *rand_in);

uint8_t ecdsa_sign_message(ecdsa_signature_t *s, const void *m, uint16_t m_len_b,
                           const hfdesc_t *hash_desc, const ecdsa_ctx_t *ctx,
                           const void *rand_in);

#endif /* ECDSA_SIGN_H_ */
