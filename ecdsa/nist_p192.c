/* p192.c */
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

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


#include "bigint.h"
#include "ecc.h"
#include "nist_p192.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include "bigint_io.h"

#define printf_P(...)
#define bigint_print_hex(a)
#undef putchar
#define putchar(a)

/*
 * p = 6277101735386680763835789423207666416083908700390324961279
 *   = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF
 */

uint8_t nist_curve_p192_p_w[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

uint8_t nist_curve_p192_b_w[] = {
    0xb1, 0xb9, 0x46, 0xc1, 0xec, 0xde, 0xb8, 0xfe,
    0x49, 0x30, 0x24, 0x72, 0xab, 0xe9, 0xa7, 0x0f,
    0xe7, 0x80, 0x9c, 0xe5, 0x19, 0x05, 0x21, 0x64
};

uint8_t nist_curve_p192_gx_w[] = {
    0x12, 0x10, 0xff, 0x82, 0xfd, 0x0a, 0xff, 0xf4,
    0x00, 0x88, 0xa1, 0x43, 0xeb, 0x20, 0xbf, 0x7c,
    0xf6, 0x90, 0x30, 0xb0, 0x0e, 0xa8, 0x8d, 0x18
};

uint8_t nist_curve_p192_gy_w[] = {
    0x11, 0x48, 0x79, 0x1e, 0xa1, 0x77, 0xf9, 0x73,
    0xd5, 0xcd, 0x24, 0x6b, 0xed, 0x11, 0x10, 0x63,
    0x78, 0xda, 0xc8, 0xff, 0x95, 0x2b, 0x19, 0x07
};

uint8_t nist_curve_p192_z1_w[192 / BIGINT_WORD_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 };

uint8_t nist_curve_p192_z2_w[192 / BIGINT_WORD_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 };

uint8_t nist_curve_p192_z3_w[192 / BIGINT_WORD_SIZE] = {
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 };

bigint_t nist_curve_p192_p = {
    .length_W = 192 / BIGINT_WORD_SIZE,
    .wordv = nist_curve_p192_p_w,
    .info = 7
};

bigint_t nist_curve_p192_b = {
    .length_W = 192 / BIGINT_WORD_SIZE,
    .wordv = nist_curve_p192_b_w,
    .info = 6
};

ecc_combi_point_t nist_curve_p192_basepoint = {
    .chudnovsky = {
        .x = {
            .length_W = 192 / BIGINT_WORD_SIZE,
            .wordv = nist_curve_p192_gx_w,
            .info = 4
        },
        .y = {
            .length_W = 192 / BIGINT_WORD_SIZE,
            .wordv = nist_curve_p192_gy_w,
            .info = 2
        },
        .z1 = {
            .length_W = 1,
            .wordv = nist_curve_p192_z1_w,
            .info = 0
        },
        .z2 = {
            .length_W = 1,
            .wordv = nist_curve_p192_z2_w,
            .info = 0
        },
        .z3 = {
            .length_W = 1,
            .wordv = nist_curve_p192_z3_w,
            .info = 0
        }
    }
};

ecc_curve_sp_t nist_curve_p192 = {
    .b = &nist_curve_p192_b,
    .p = &nist_curve_p192_p,
    .reduce_p = bigint_reduce_p192
};


/*
 * A = ( A5 || A4 || A3 || A2 || A1 || A0 ) ; An if 64-bit
 * A mod p = B = T + S1 + S2 + S3 mod p
 *
 * T  = ( A2 || A1 || A0 )
 * S1 = ( A3 || A3 )
 * S2 = ( A4 || A4 || 0 )
 * S3 = ( A5 || A5 || A5 )
 *
 */

int bigint_reduce_p192(bigint_t *a){

    bigint_word_t s_w[3 * 64 / BIGINT_WORD_SIZE];
    bigint_t s;
    uint16_t o_length;

    if(a->info & BIGINT_NEG_MASK){
        printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
        /* negative value */
        a->info &= ~BIGINT_NEG_MASK;
        bigint_reduce_p192(a);
        a->info |= BIGINT_NEG_MASK;
        bigint_add_s(a, a, &nist_curve_p192_p);
        return 0;
    }

    o_length = a->length_W;

    if(o_length < 192 / BIGINT_WORD_SIZE){
        return 0;
    }

    if(o_length > 192 * 2 / BIGINT_WORD_SIZE){
        bigint_reduce(a, &nist_curve_p192_p);
    }

    if(o_length > 192 / BIGINT_WORD_SIZE){
        s.wordv = s_w;
        s.length_W = 2 * 64 / BIGINT_WORD_SIZE;
        s.info = 0;

        printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
        /*
         * copy A3 twice in s
         */
        if(o_length >= 4 * 64 / BIGINT_WORD_SIZE){
            printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
            memcpy(s.wordv, a->wordv + 3 * 64 / BIGINT_WORD_SIZE, 64 / 8);
        }else{
            printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
            memset(s.wordv, 0, 8);
            memcpy(s.wordv, a->wordv + 3 * 64 / BIGINT_WORD_SIZE,
                    o_length * BIGINT_WORD_SIZE / 8 - 3 * 64 / 8);
        }

        memcpy(s.wordv + 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
        bigint_adjust(&s);

        /*
         * Set A3 to zero so we can use a as T
         */
        memset(a->wordv + 3 * 64 / BIGINT_WORD_SIZE, 0, sizeof(bigint_word_t));
        a->length_W = 3 * 64 / BIGINT_WORD_SIZE;
        bigint_adjust(a);

        /*
         * Add s (alias S1) to a (alias T)
         */

        printf_P(PSTR("T: "));
        bigint_print_hex(a);
        putchar('\n');

        printf_P(PSTR("s1: "));
        bigint_print_hex(&s);
        putchar('\n');
        bigint_add_u(a, a, &s);


        if(o_length > 4 * 64 / BIGINT_WORD_SIZE){
            s.length_W = 2 * 64 / BIGINT_WORD_SIZE;
            /*
             * copy A4 twice in s
             */
            printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
            if(o_length >= 5 * 64 / BIGINT_WORD_SIZE){
                printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
                memcpy(s.wordv, a->wordv + 4 * 64 / BIGINT_WORD_SIZE, 64 / 8);
            }else{
                printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
                memset(s.wordv, 0, 8);
                memcpy(s.wordv, a->wordv + 4 * 64 / BIGINT_WORD_SIZE,
                        o_length * BIGINT_WORD_SIZE / 8 - 4 * 64 / 8);
            }
            memcpy(s.wordv + 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
            bigint_adjust(&s);

            /*
             * Add s (alias S2) to a (alias T + S1)
             */
            printf_P(PSTR("s2: "));
            bigint_print_hex(&s);
            putchar('\n');
            bigint_add_scale_u(a, &s, 8);


            if(o_length > 5 * 64 / BIGINT_WORD_SIZE){
                /*
                 * copy A5 three times in s
                 */
                printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
                if(o_length == 6 * 64 / BIGINT_WORD_SIZE){
                    printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
                    memcpy(s.wordv, a->wordv + 5 * 64 / BIGINT_WORD_SIZE, 64 / 8);
                } else {
                    printf_P(PSTR("DBG: Line: %d\n"), __LINE__);
                    memset(s.wordv, 0, 8);
                    memcpy(s.wordv, a->wordv + 5 * 64 / BIGINT_WORD_SIZE,
                            o_length * BIGINT_WORD_SIZE / 8 - 5 * 64 / 8);
                }
                memcpy(s.wordv + 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
                memcpy(s.wordv + 2 * 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
                s.length_W = 3 * 64 / BIGINT_WORD_SIZE;
                bigint_adjust(&s);

                /*
                 * Add s (alias S2) to a (alias T + S1)
                 */
                printf_P(PSTR("s3: "));
                bigint_print_hex(&s);
                putchar('\n');

                bigint_add_u(a, a, &s);
            }
        }

    }
    printf_P(PSTR("pre-result: "));
    bigint_print_hex(a);
    putchar('\n');
    while(bigint_cmp_u(a, &nist_curve_p192_p) >= 0){
        printf_P(PSTR("DBG: Line: %d\n"),  __LINE__);
        bigint_sub_u(a, a, &nist_curve_p192_p);
    }
    printf_P(PSTR("result: "));
    bigint_print_hex(a);
    putchar('\n');

    return 0;
}


