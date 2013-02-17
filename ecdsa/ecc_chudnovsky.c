/* ecc_chudnovsky.c */
/*
    This file is part of the ARM-Crypto-Lib.
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
#include <string.h>
#include "bigint.h"
#include "ecc.h"
#include <stdlib.h>
#include <string.h>

#if 1
#include <stdio.h>
#include "bigint_io.h"
#include "uart_i.h"
#include <avr/pgmspace.h>
#endif

#if 0
#define printf_P(...)
#define bigint_print_hex(a)
#undef putchar
#define putchar(a)
#endif

uint8_t ecc_chudnovsky_point_alloc(ecc_chudnovsky_point_t *p, size_t length_B){
    if(!(p->x.wordv = malloc(length_B))){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
        return 1;
    }
    if(!(p->y.wordv = malloc(length_B))){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
        free(p->x.wordv);
        return 1;
    }
    if(!(p->z1.wordv = malloc(length_B))){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
        free(p->x.wordv);
        free(p->y.wordv);
        return 1;
    }
    if(!(p->z2.wordv = malloc(length_B))){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
        free(p->x.wordv);
        free(p->y.wordv);
        free(p->z1.wordv);
        return 1;
    }
    if(!(p->z3.wordv = malloc(length_B))){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
        free(p->x.wordv);
        free(p->y.wordv);
        free(p->z1.wordv);
        free(p->z2.wordv);
        return 1;
    }
    bigint_set_zero(&p->x);
    bigint_set_zero(&p->y);
    bigint_set_zero(&p->z1);
    bigint_set_zero(&p->z2);
    bigint_set_zero(&p->z3);
    return 0;
}

void ecc_chudnovsky_point_free(ecc_chudnovsky_point_t *p){
    free(p->x.wordv);
    free(p->y.wordv);
    free(p->z1.wordv);
    free(p->z2.wordv);
    free(p->z3.wordv);
}

/*
 * if (Y == 0)
 *   return POINT_AT_INFINITY
 * S = 4*X*Y^2
 * M = 3*(X + Z^2)*(X - Z^2)
 * X' = M^2 - 2*S
 * Y' = M*(S - X') - 8*Y^4
 * Z' = 2*Y*Z
 * Z'^2 = Z'^2
 * Z'^3 = Z'^2 * Z'
 * return (X', Y', Z', Z'^2, Z'^3)
 */

uint8_t ecc_affine_to_chudnovsky_point(ecc_chudnovsky_point_t *dest,
                                       const ecc_affine_point_t *src){
    if(src->y.length_W == 0){
        /* point at infinity */
        bigint_set_zero(&dest->y);
        return 0;
    }
    bigint_copy(&dest->x, &src->x);
    bigint_copy(&dest->y, &src->y);
    dest->z1.wordv[0] = 1;
    dest->z2.wordv[0] = 1;
    dest->z3.wordv[0] = 1;
    dest->z1.length_W = 1;
    dest->z2.length_W = 1;
    dest->z3.length_W = 1;
    dest->z1.info = 0;
    dest->z2.info = 0;
    dest->z3.info = 0;
    bigint_adjust(&dest->z1);
    bigint_adjust(&dest->z2);
    bigint_adjust(&dest->z3);
    return 0;
}

uint8_t ecc_chudnovsky_to_affine_point(ecc_affine_point_t *dest,
                                       const ecc_chudnovsky_point_t *src,
                                       const ecc_curve_sp_t *curve){
    if(src->y.length_W == 0){
        /* point at infinity */
        bigint_set_zero(&dest->y);
        return 0;
    }
    bigint_word_t t_w[curve->p->length_W * 2];
    bigint_word_t z1_w[curve->p->length_W * 2];
    bigint_word_t z2_w[curve->p->length_W];
    bigint_t t, z1, z2;
    t.wordv =  t_w;
    z1.wordv = z1_w;
    z2.wordv = z2_w;

    bigint_inverse(&z1, &src->z1, curve->p);
    bigint_square(&t, &z1);
    curve->reduce_p(&t);
    bigint_copy(&z2, &t);
    bigint_mul_u(&t, &src->x, &z2);
    curve->reduce_p(&t);
    bigint_copy(&dest->x, &t);
    bigint_mul_u(&t, &z1, &z2);
    curve->reduce_p(&t);
    bigint_mul_u(&t, &t, &src->y);
    curve->reduce_p(&t);
    bigint_copy(&dest->y, &t);

    return 0;
}

void ecc_chudnovsky_point_print(const ecc_chudnovsky_point_t *p){
    if(p->y.length_W == 0){
        printf_P(PSTR(" ECC point = point-at-infinity\n"));
        return;
    }
    printf_P(PSTR(" ECC point x  = "));
    bigint_print_hex(&p->x);
    printf_P(PSTR("\n ECC point y  = "));
    bigint_print_hex(&p->y);
    printf_P(PSTR("\n ECC point z1 = "));
    bigint_print_hex(&p->z1);
    printf_P(PSTR("\n ECC point z2 = "));
    bigint_print_hex(&p->z2);
    printf_P(PSTR("\n ECC point z3 = "));
    bigint_print_hex(&p->z3);
    putchar('\n');
}

uint8_t ecc_chudnovsky_point_double_sp(ecc_chudnovsky_point_t *dest,
                                       const ecc_chudnovsky_point_t *a,
                                       const ecc_curve_sp_t *curve){
    if(a->y.length_W == 0){
        /* point at infinity */
        bigint_set_zero(&dest->y);
        return 0;
    }
    bigint_word_t s_w[curve->p->length_W * 2];
    bigint_word_t m_w[curve->p->length_W * 2];
    bigint_word_t t_w[curve->p->length_W * 2];
    bigint_t s, m, t;

    s.wordv = s_w;
    m.wordv = m_w;
    t.wordv = t_w;

    /* compute s*/
    bigint_square(&t, &a->y);
    curve->reduce_p(&t);
    bigint_mul_u(&s, &t, &a->x);
    curve->reduce_p(&s);
    bigint_shiftleft(&s, 2);
    curve->reduce_p(&s);

    /* compute m */
    bigint_sub_u(&t, &a->x, &a->z2);
//    /**/curve->reduce_p(&t);
    bigint_add_u(&m, &a->x, &a->z2);
//    /**/curve->reduce_p(&m);
    bigint_mul_s(&m, &m, &t);
    curve->reduce_p(&m);
    bigint_copy(&t, &m);
    bigint_shiftleft(&t, 1);
    bigint_add_s(&m, &m, &t);
    curve->reduce_p(&m);

    /* compute new z1 */
    bigint_mul_u(&t, &a->z1, &a->y);
    curve->reduce_p(&t);
    bigint_shiftleft(&t, 1);
    curve->reduce_p(&t);
    bigint_copy(&dest->z1, &t);

    /* compute new x */
    bigint_square(&t, &m);
    curve->reduce_p(&t);
    bigint_sub_s(&t, &t, &s);
    bigint_sub_s(&t, &t, &s);
    curve->reduce_p(&t);
    bigint_copy(&dest->x, &t);

    /* compute new y */
    bigint_sub_s(&s, &s, &t);
    curve->reduce_p(&s);
    bigint_mul_s(&s, &s, &m);
    curve->reduce_p(&s);
    bigint_square(&t, &a->y);
    curve->reduce_p(&t);
    bigint_square(&t, &t);
    curve->reduce_p(&t);
    bigint_shiftleft(&t, 3);
    curve->reduce_p(&t);
    bigint_sub_s(&s, &s, &t);
    curve->reduce_p(&s);
    bigint_copy(&dest->y, &s);

    /* compute new z2 */
    bigint_square(&t, &dest->z1);
    curve->reduce_p(&t);
    bigint_copy(&dest->z2, &t);

    /* compute new z3 */
    bigint_mul_u(&t, &t, &dest->z1);
    curve->reduce_p(&t);
    bigint_copy(&dest->z3, &t);

    return 0;
}

void ecc_chudnovsky_point_copy(ecc_chudnovsky_point_t *dest,
                               const ecc_chudnovsky_point_t *src){
    bigint_copy(&dest->x, &src->x);
    bigint_copy(&dest->y, &src->y);
    bigint_copy(&dest->z1, &src->z1);
    bigint_copy(&dest->z2, &src->z2);
    bigint_copy(&dest->z3, &src->z3);
}

uint8_t ecc_chudnovsky_point_add_sp(ecc_chudnovsky_point_t *dest,
                                    const ecc_chudnovsky_point_t *a,
                                    const ecc_chudnovsky_point_t *b,
                                    const ecc_curve_sp_t *curve){
    if(a->y.length_W == 0){
        ecc_chudnovsky_point_copy(dest, b);
        return 0;
    }
    if(b->y.length_W == 0){
        ecc_chudnovsky_point_copy(dest, a);
        return 0;
    }
    bigint_word_t u1_w[curve->p->length_W * 2];
    bigint_word_t u2_w[curve->p->length_W * 2];
    bigint_word_t s1_w[curve->p->length_W * 2];
    bigint_word_t s2_w[curve->p->length_W * 2];
    bigint_t u1, u2, s1, s2;

    u1.wordv = u1_w;
    u2.wordv = u2_w;
    s1.wordv = s1_w;
    s2.wordv = s2_w;

    /* compute u1 */
    bigint_mul_u(&u1, &a->x, &b->z2);
    curve->reduce_p(&u1);

    /* compute u2 */
    bigint_mul_u(&u2, &b->x, &a->z2);
    curve->reduce_p(&u2);

    /* compute s1 */
    bigint_mul_u(&s1, &a->y, &b->z3);
    curve->reduce_p(&s1);

    /* compute s2 */
    bigint_mul_u(&s2, &b->y, &a->z3);
    curve->reduce_p(&s2);

    if(bigint_cmp_u(&u1, &u2) == 0){
        if(bigint_cmp_u(&s1, &s2)){
            /* point at infinity */
            bigint_set_zero(&dest->y);
            return 0;
        }else{
            /* a == b --> dest = 2*a */
            ecc_chudnovsky_point_double_sp(dest, a, curve);
        }
    }

    bigint_word_t h_w[curve->p->length_W * 2];
    bigint_word_t r_w[curve->p->length_W * 2];
    bigint_t h, r;

    h.wordv = h_w;
    r.wordv = r_w;
    /* compute h */
    bigint_sub_s(&h, &u2, &u1);
    /**/curve->reduce_p(&h);

    /* compute r */
    bigint_sub_s(&r, &s2, &s1);
//    /**/curve->reduce_p(&r);

    /* compute new z */
    bigint_mul_u(&s2, &a->z1, &b->z1);
    curve->reduce_p(&s2);
    bigint_mul_s(&s2, &s2, &h);
    curve->reduce_p(&s2);
    bigint_copy(&dest->z1, &s2);

    /* compute u1*h^2 and h^3 */
    bigint_square(&s2, &h);
    curve->reduce_p(&s2);
    bigint_mul_s(&h, &s2, &h);
    curve->reduce_p(&h);
    bigint_mul_s(&u1, &s2, &u1);
    curve->reduce_p(&u1);

    /* compute new x */
    bigint_square(&u2, &r);
    curve->reduce_p(&u2);
    bigint_sub_s(&u2, &u2, &h);
    curve->reduce_p(&u2);
    bigint_sub_s(&u2, &u2, &u1);
    bigint_sub_s(&u2, &u2, &u1);
    curve->reduce_p(&u2);
    bigint_copy(&dest->x, &u2);

    /* compute new y */
    bigint_sub_s(&u1, &u1, &u2);
    curve->reduce_p(&u1);
    bigint_mul_s(&s2, &u1, &r);
    curve->reduce_p(&s2);
    bigint_mul_s(&s1, &s1, &h);
    curve->reduce_p(&s1);
    bigint_sub_s(&s2, &s2, &s1);
    curve->reduce_p(&s2);
    bigint_copy(&dest->y, &s2);


    /* compute new z2 */
    bigint_square(&s1, &dest->z1);
    curve->reduce_p(&s1);
    bigint_copy(&dest->z2, &s1);

    /* compute new z2 */
    bigint_mul_u(&s1, &s1, &dest->z1);
    curve->reduce_p(&s1);
    bigint_copy(&dest->z3, &s1);

    return 0;
}

uint8_t ecc_chudnovsky_double_and_add(ecc_chudnovsky_point_t *dest,
                                      const bigint_t *k,
                                      const ecc_chudnovsky_point_t *p,
                                      const ecc_curve_sp_t *curve){
    uint16_t i;
    uint8_t s = 0;
    bigint_word_t v, t;

    for(i = k->length_W; i > 0; --i){
        v = 1 << (BIGINT_WORD_SIZE - 1);
        t = k->wordv[i - 1];
        do{
            if(s){
                ecc_chudnovsky_point_double_sp(dest, dest, curve);
                if(v & t){
                    ecc_chudnovsky_point_add_sp(dest, dest, p, curve);
                }
            }else{
                if(v & t){
                    s = 1;
                    ecc_chudnovsky_point_copy(dest, p);
                }
            }
            v >>= 1;
        }while(v);
    }
    return 0;
}

uint8_t bigint_to_naf(uint8_t *dest, uint16_t *length, const bigint_t *src){
    if(src->length_W == 0){
        *dest = 0;
        *length = 2;
        return 0;
    }

    memset(dest, 0, src->length_W * sizeof(bigint_word_t) * 2 +1);

    uint16_t i = 0;
    uint8_t t; /* 3 -> -1 ; 1 -> 1; 0 -> 0 (2 should not happen) */
    bigint_t k, p;
    bigint_word_t k_w[src->length_W + 1];
    bigint_word_t p_w = 1;
    p.wordv = &p_w;
    p.info = 0;
    p.length_W = 1;
    k.wordv = k_w;
    bigint_copy(&k, src);

    while(k.length_W >= 1){
        if(k.wordv[0] & 1){
            t = k.wordv[0] & 3;
            if(t == 1){
                bigint_sub_u(&k, &k, &p);
            }else{
                bigint_add_u(&k, &k, &p);
            }
        }else{
            t = 0;
        }
        dest[(i * 2) / 8] |= t << ((2 * i) & 7);
        bigint_shiftright(&k, 1);
        i += 1;
    }
    *length = i;
    return 0;
}

void print_naf(uint8_t *naf, uint16_t length){
    if(!length){
        return;
    }
    --length;
    int8_t t;
    do{
        t = (naf[(length * 2) / 8] >> ((length * 2) & 7)) & 3;
        switch(t & 3){
        case 0: putchar('0'); break;
        case 1: putchar('1'); break;
        case 3: putchar('-'); putchar('1'); break;
        default: putchar('E');
        }
        if(length){
            putchar(' ');
        }
    }while(length--);
}

uint8_t ecc_chudnovsky_naf_multiplication(ecc_chudnovsky_point_t *dest,
                                          const bigint_t *k,
                                          const ecc_chudnovsky_point_t *p,
                                          const ecc_curve_sp_t *curve){
    if(k->length_W == 0 || p->y.length_W == 0){
        bigint_set_zero(&dest->y);
        return 0;
    }

    uint8_t *t, q;
    uint16_t i;
    ecc_chudnovsky_point_t p_;
    bigint_word_t y_[curve->p->length_W];

    /* p_ = -p*/

    memcpy(&p_, p, sizeof(p_));
    p_.y.wordv = y_;
    bigint_copy(&p_.y, &p->y);
    p_.y.info |= BIGINT_NEG_MASK;
    bigint_add_s(&p_.y, &p_.y, curve->p);


    if(!(t = calloc(k->length_W * sizeof(bigint_word_t) * 2 + 1, 1))){
        return 1;
    }
    bigint_to_naf(t, &i, k);

 //   printf(" naf: ");
 //   print_naf(t, i);

    --i;
    dest->y.length_W = 0;
    do{
        q = (t[(i * 2) / 8] >> ((i * 2) & 7)) & 3;
        ecc_chudnovsky_point_double_sp(dest, dest, curve);
        if(q == 1){
            ecc_chudnovsky_point_add_sp(dest, dest, p, curve);
        }
        if(q == 3){
            ecc_chudnovsky_point_add_sp(dest, dest, &p_, curve);
        }
    }while(i--);

    free(t);

    return 0;
}

uint8_t ecc_chudnovsky_multiplication(ecc_chudnovsky_point_t *dest,
                                      const bigint_t *k,
                                      const ecc_chudnovsky_point_t *p,
                                      const ecc_curve_sp_t *curve){
    return ecc_chudnovsky_naf_multiplication(dest, k, p, curve);
}



uint8_t ecc_chudnovsky_multipy_and_sum(ecc_chudnovsky_point_t *dest,
                                      const bigint_t *k,
                                      const ecc_chudnovsky_point_t *p,
                                      const bigint_t *l,
                                      const ecc_chudnovsky_point_t *q,
                                      const ecc_curve_sp_t *curve){
    return ecc_chudnovsky_naf_multiplication(dest, k, p, curve);
}
