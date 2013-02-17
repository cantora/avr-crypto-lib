/* ecdsa.h */
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

#ifndef ECDSA_H_
#define ECDSA_H_

#include "ecc.h"
#include "hashfunction_descriptor.h"

typedef struct{
    bigint_t r;
    bigint_t s;
} ecdsa_signature_t;

typedef bigint_t ecdsa_privkey_t;
typedef ecc_affine_point_t ecdsa_pubkey_t;

typedef struct{
    ecdsa_privkey_t *priv;
    ecdsa_pubkey_t  *pub;
    ecc_curve_sp_t  *curve;
    ecc_chudnovsky_point_t *basepoint;
} ecdsa_ctx_t;

typedef uint8_t(*rand_func_ptr_t)(void);

#define ECDSA_SIGNATURE_OK 1
#define ECDSA_SIGNATURE_FAIL 0

uint8_t ecdsa_sign_bigint(ecdsa_signature_t *s, const bigint_t *m,
                          const ecdsa_ctx_t *ctx, const bigint_t *k);
uint8_t ecdsa_sign_message(ecdsa_signature_t *s, const void *m, uint16_t m_len_b,
                           const hfdesc_t *hash_desc, const ecdsa_ctx_t *ctx,
                           const void *rand_in);
uint8_t ecdsa_verify_bigint(const ecdsa_signature_t *s, const bigint_t *m,
                            const ecdsa_ctx_t *ctx);
uint8_t ecdsa_verify_message(const ecdsa_signature_t *s, const void *m, uint16_t m_len_b,
                             const hfdesc_t *hash_desc, const ecdsa_ctx_t *ctx);


#endif /* ECDSA_H_ */
