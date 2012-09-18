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

/*
 * p = 6277101735386680763835789423207666416083908700390324961279
 *   = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF
 */

static
uint8_t nist_curve_p192_p_w[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

bigint_t nist_curve_p192_p = {
        .length_W = 192 / BIGINT_WORD_SIZE,
        .wordv = nist_curve_p192_p_w,
        .info = 7
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

        /*
         * copy A3 twice in s
         */
        if(o_length >= 4 * 64 / BIGINT_WORD_SIZE){
            memcpy(s.wordv, a->wordv + 3 * 64 / BIGINT_WORD_SIZE, 64 / 8);
        }else{
            memset(s.wordv, 0, 8);
            memcpy(s.wordv, a->wordv + 3 * 64 / BIGINT_WORD_SIZE,
                    o_length * BIGINT_WORD_SIZE / 8 - 3 * 64 / 8);
        }

        memcpy(s.wordv + 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
        bigint_adjust(&s);

        /*
         * Set A3 to zero so we can use a as T
         */
        memset(a->wordv + 3 * 64 / BIGINT_WORD_SIZE, 0, BIGINT_WORD_SIZE / 8);
        a->length_W = 3 * 64 / BIGINT_WORD_SIZE;
        bigint_adjust(a);

        /*
         * Add s (alias S1) to a (alias T)
         */
        bigint_add_u(a, a, &s);


        if(o_length > 4 * 64 / BIGINT_WORD_SIZE){
            /*
             * copy A4 twice in s
             */
            if(o_length >= 5 * 64 / BIGINT_WORD_SIZE){
                memcpy(s.wordv, a->wordv + 4 * 64 / BIGINT_WORD_SIZE, 64 / 8);
            }else{
                memset(s.wordv, 0, 8);
                memcpy(s.wordv, a->wordv + 4 * 64 / BIGINT_WORD_SIZE,
                        o_length * BIGINT_WORD_SIZE / 8 - 4 * 64 / 8);
            }
            memcpy(s.wordv + 64 / BIGINT_WORD_SIZE, s.wordv, 64 / 8);
            bigint_adjust(&s);

            /*
             * Add s (alias S2) to a (alias T + S1)
             */
            bigint_add_scale_u(a, &s, 8);


            if(o_length > 5 * 64 / BIGINT_WORD_SIZE){
                /*
                 * copy A5 three times in s
                 */
                if(o_length == 6 * 64 / BIGINT_WORD_SIZE){
                    memcpy(s.wordv, a->wordv + 5 * 64 / BIGINT_WORD_SIZE, 64 / 8);
                } else {
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
                bigint_add_u(a, a, &s);
            }
        }

    }
    while(bigint_cmp_u(a, &nist_curve_p192_p) >= 0){
        bigint_sub_u(a, a, &nist_curve_p192_p);
    }

    return 0;
}

