/* ecdsa.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2012 Daniel Otte (daniel.otte@rub.de)

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


#include <stdlib.h>
#include <inttypes.h>
#include "bigint.h"
#include "ecc.h"
#include "random_dummy.h"
#include "ecdsa.h"
#include "hfal-basic.h"

uint8_t ecdsa_signature_alloc(ecdsa_signature_t* s, size_t length_B){
    if(!(s->r.wordv = malloc(length_B))){
        return 1;
    }
    if(!(s->s.wordv = malloc(length_B))){
        free(s->r.wordv);
        return 1;
    }
    bigint_set_zero(&s->r);
    bigint_set_zero(&s->s);
    return 0;
}

void ecdsa_signature_free(ecdsa_signature_t* s){
    free(s->r.wordv);
    free(s->s.wordv);
}

uint8_t ecdsa_sign_bigint(ecdsa_signature_t* s, const bigint_t* m,
                          const ecdsa_ctx_t* ctx, const bigint_t* k){

    bigint_t t;
    ecc_combi_point_t q;


    if(!(t.wordv = malloc(ctx->curve->p->length_W * 2 * sizeof(bigint_word_t)))){
        return 1;
    }
    if(!ecc_chudnovsky_point_alloc(&q.chudnovsky, ctx->curve->p->length_W * sizeof(bigint_word_t))){
        free(t.wordv);
        return 1;
    }

    ecc_chudnovsky_multiplication(&q.chudnovsky, k, ctx->basepoint, ctx->curve);
    if(q.chudnovsky.x.length_W == 0){
        return 2;
    }
    ecc_chudnovsky_to_affine_point(&q.affine, &q.chudnovsky, ctx->curve);
    bigint_inverse(&s->r, k, ctx->curve->p);
    bigint_mul_u(&t, &q.affine.x, ctx->priv);
    ctx->curve->reduce_p(&t);
    bigint_add_u(&t, &t, m);
    ctx->curve->reduce_p(&t);
    bigint_mul_u(&t, &t, &s->r);
    ctx->curve->reduce_p(&t);
    if(t.length_W == 0){
        return 2;
    }
    bigint_copy(&s->r, &q.affine.x);
    bigint_copy(&s->s, &t);

    ecc_chudnovsky_point_free(&q.chudnovsky);
    free(t.wordv);

    return 0;
}
uint8_t ecdsa_sign_message(ecdsa_signature_t* s, const void* m, uint16_t m_len_b,
                           const hfdesc_t* hash_desc, const ecdsa_ctx_t* ctx,
                           const void *rand_in){
    bigint_t m_int;
    bigint_t r_int;
    uint8_t r;

    if(rand_in == NULL){
        if(!(r_int.wordv = malloc(ctx->curve->p->length_W * sizeof(bigint_word_t)))){
            return 1;
        }
    }

    if(!(m_int.wordv = malloc(hfal_hash_getHashsize(hash_desc) / 8))){
        if(rand_in == NULL){
            free(r_int.wordv);
        }
        return 1;
    }
    hfal_hash_mem(hash_desc, m_int.wordv, m, m_len_b);

    do{
        if(rand_in == NULL){
            size_t i;
            do{
                i = ctx->curve->p->length_W * sizeof(bigint_word_t) - 1;
                r_int.length_W = ctx->curve->p->length_W;
                do{
                    ((uint8_t*)r_int.wordv)[i] = prng_get_byte();
                }while(i--);
                bigint_adjust(&r_int);
            }while(bigint_cmp_u(&r_int, ctx->curve->p) >= 0);
        }
    }while((r = ecdsa_sign_bigint(s, &m_int, ctx, &r_int)) == 2 && (rand_in == NULL));

    return r;
}
