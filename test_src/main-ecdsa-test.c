/* main-dsa-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
/*
 * ECDSA test-suit
 *
*/

#include "main-test-common.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"
#include "nist_p192.h"

#include "performance_test.h"
#include "hfal_sha1.h"
#include "base64_enc.h"
#include "base64_dec.h"

char* algo_name = "ECDSA";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_performance_invert_bigint(void){
    printf_P(PSTR("\n=== performance measurement (invert) ===\n"));
    unsigned i,j;
    uint64_t time = 0;
    bigint_t a, v;
    bigint_word_t v_w[192 / BIGINT_WORD_SIZE];
    bigint_word_t a_w[192 / BIGINT_WORD_SIZE];

    a.wordv = a_w;
    v.wordv = v_w;

    for(j = 0; j < 32; ++j){
        for(i = 0; i < 192 / BIGINT_WORD_SIZE; ++i){
            ((uint8_t*)v_w)[i] = random();
        }
        v.length_W = 192 / BIGINT_WORD_SIZE;
        v.info = 0;
        bigint_adjust(&v);

        for(i = 0; i < 16; ++i){
            startTimer(1);
            START_TIMER;
            bigint_inverse(&a, &v, &nist_curve_p192_p);
            STOP_TIMER;
            time += stopTimer();
        }
    }

    time >>= 8;
    ++time;
    time >>= 1;

    printf_P(PSTR("  invert costs %"PRIu32" cycles\n"), (uint32_t)time);
}

void testrun_performance_multiply_bigint(void){
    printf_P(PSTR("\n=== performance measurement (invert) ===\n"));
    unsigned i,j;
    uint64_t time_a = 0, time_b = 0;
    uint32_t tmp;
    bigint_t a, b, v;
    bigint_word_t v_w[192 * 2 / BIGINT_WORD_SIZE];
    bigint_word_t a_w[192 / BIGINT_WORD_SIZE];
    bigint_word_t b_w[192 / BIGINT_WORD_SIZE];

    a.wordv = a_w;
    b.wordv = b_w;
    v.wordv = v_w;

    for(j = 0; j < 32; ++j){
        for(i = 0; i < 192 / BIGINT_WORD_SIZE; ++i){
            ((uint8_t*)a_w)[i] = random();
        }
        a.length_W = 192 / BIGINT_WORD_SIZE;
        a.info = 0;
        bigint_adjust(&a);

        for(i = 0; i < 192 / BIGINT_WORD_SIZE; ++i){
            ((uint8_t*)b_w)[i] = random();
        }
        b.length_W = 192 / BIGINT_WORD_SIZE;
        b.info = 0;
        bigint_adjust(&b);

        for(i = 0; i < 16; ++i){
            startTimer(1);
            START_TIMER;
            bigint_mul_u(&v,&a, &b);
            STOP_TIMER;
            tmp = stopTimer();
            time_a += tmp;
            time_b += tmp;

            START_TIMER;
            bigint_reduce_p192(&v);
            STOP_TIMER;
            tmp = stopTimer();
            time_b += tmp;
         }
    }

    time_a >>= 8;
    ++time_a;
    time_a >>= 1;

    time_b >>= 8;
    ++time_b;
    time_b >>= 1;


    printf_P(PSTR("  multiply          costs %7"PRIu32" cycles\n"), (uint32_t)time_a);
    printf_P(PSTR("  multiply + reduce costs %7"PRIu32" cycles\n"), (uint32_t)time_b);
}

void testrun_performance_reduce_bigint(void){
    printf_P(PSTR("\n=== performance measurement (reduce) ===\n"));
    unsigned i, j;
    bigint_t a,b,v;
    bigint_word_t v_w[192 * 2 / BIGINT_WORD_SIZE];
    bigint_word_t a_w[192 * 2 / BIGINT_WORD_SIZE];
    bigint_word_t b_w[192 * 2 / BIGINT_WORD_SIZE];
    uint32_t time_a, time_b;
    int32_t time_diff;
    int16_t faster_percent;
    v.wordv = v_w;
    for(j = 0; j < 32; ++j){
        do{
            for(i = 0; i < 192 * 2 / BIGINT_WORD_SIZE; ++i){
                ((uint8_t*)v_w)[i] = random();
            }
            v.length_W = 192 * 2 / BIGINT_WORD_SIZE;
            v.info = 0;
            bigint_adjust(&v);
        }while(0);

    //    printf_P(PSTR("candidate:\n"));
    //    bigint_print_hex(&v);
        a.wordv = a_w;
        b.wordv = b_w;
        calibrateTimer();

    //    printf_P(PSTR("\n  going to test optimized version: ...\n"));
        uart0_flush();
        time_a = 0;
        for(i = 0; i < 16; ++i){
            bigint_copy(&a, &v);
            startTimer(1);
            START_TIMER;
            bigint_reduce_p192(&a);
            STOP_TIMER;
            time_a += stopTimer();
        }
    //    printf_P(PSTR("    took: %"PRIu32" cycles\nresult:"), time);
    //    bigint_print_hex(&a);


    //    printf_P(PSTR("\n  going to test not-optimized version: ...\n"));
    //    uart0_flush();
        time_b = 0;
        for(i = 0; i < 16; ++i){
            bigint_copy(&b, &v);
            startTimer(1);
            START_TIMER;
            bigint_reduce(&b, &nist_curve_p192_p);
            STOP_TIMER;
            time_b += stopTimer();
        }
    //    printf_P(PSTR("    took: %"PRIu32" cycles\nresult:"), time);
    //    bigint_print_hex(&b);

        time_diff = time_b - time_a;
        faster_percent = (time_diff * 100) / time_b;

        printf_P(PSTR("  delta: %7"PRId32"  (%3"PRId16"%%)  :-"), time_diff, faster_percent);
        if(bigint_cmp_u(&a, &b)){
            printf_P(PSTR("(\n"));
        } else {
            printf_P(PSTR(")\n"));
        }
        uart0_flush();
    }
}

uint8_t ecc_affine_point_alloc(ecc_affine_point_t *p, uint16_t length_b){
    size_t len = (length_b + BIGINT_WORD_SIZE - 1)/ BIGINT_WORD_SIZE;
    if (! (p->x.wordv = malloc(len))){
        return 1;
    }
    if (! (p->y.wordv = malloc(len))){
        free(p->x.wordv);
        return 2;
    }
    return 0;
}

void ecc_affine_point_free(ecc_affine_point_t *p){
    free(p->x.wordv);
    free(p->y.wordv);
}

uint8_t ecc_chudnovsky_point_alloc(ecc_chudnovsky_point_t *p, uint16_t length_b){
    size_t len = (length_b + BIGINT_WORD_SIZE - 1)/ BIGINT_WORD_SIZE;
    if (! (p->x.wordv = malloc(len))){
        return 1;
    }
    if (! (p->y.wordv = malloc(len))){
        free(p->x.wordv);
        return 2;
    }
    if (! (p->z1.wordv = malloc(len))){
        free(p->x.wordv);
        free(p->y.wordv);
        return 3;
    }
    if (! (p->z2.wordv = malloc(len))){
        free(p->x.wordv);
        free(p->y.wordv);
        free(p->z1.wordv);
        return 4;
    }
    if (! (p->z3.wordv = malloc(len))){
        free(p->x.wordv);
        free(p->y.wordv);
        free(p->z1.wordv);
        free(p->z2.wordv);
        return 5;
    }
    return 0;
}

void ecc_chudnovsky_point_free(ecc_chudnovsky_point_t *p){
    free(p->x.wordv);
    free(p->y.wordv);
    free(p->z1.wordv);
    free(p->z2.wordv);
    free(p->z3.wordv);
}

void testrun_square(void){
    bigint_word_t a_w[] = {
        0x82, 0x6f, 0x79, 0x39, 0x47, 0x06, 0x26, 0x9f,
        0x4b, 0xe2, 0x15, 0x61, 0x6f, 0xa1, 0xd4, 0x0c,
        0x1f, 0x24, 0x3a, 0xd4, 0xc2, 0x6d, 0xe8, 0xb6
    };

    bigint_word_t b_w[2 * 192 / BIGINT_WORD_SIZE];
    bigint_t a, b;

    a.wordv = a_w;
    a.length_W = sizeof(a_w);
    a.info = 7;

    b.wordv = b_w;
    b.info = 0;
    b.length_W = 0;

    printf_P(PSTR("\n a = "));
    bigint_print_hex(&a);
    bigint_square(&b, &a);
    printf_P(PSTR("\n a^2 = "));
    bigint_print_hex(&b);
    bigint_reduce_p192(&b);
    printf_P(PSTR("\n a^2 %% p = "));
    bigint_print_hex(&b);
    putchar('\n');


}

#if 1
/*
0: b3cfed2634516540528622e16c396c229e50bbdf773f8423
1: b6e86dc2d43a241f0cd4a16f6115e24b9f26064739796f82
2: 563f557e41731f268f82fe81c8fed959600dd46649ebeeee
3: 5e45169bd87475db886b8a7833bb0845f5b011a7ce0c1766
4: 4abf34c505a73308a804dcefacbd8f7b10b59fa6ac6421a
*/
uint8_t test_point_x_w[] = {
    0x23, 0x84, 0x3f, 0x77, 0xdf, 0xbb, 0x50, 0x9e,
    0x22, 0x6c, 0x39, 0x6c, 0xe1, 0x22, 0x86, 0x52,
    0x40, 0x65, 0x51, 0x34, 0x26, 0xed, 0xcf, 0xb3
};

uint8_t test_point_y_w[] = {
    0x82, 0x6f, 0x79, 0x39, 0x47, 0x06, 0x26, 0x9f,
    0x4b, 0xe2, 0x15, 0x61, 0x6f, 0xa1, 0xd4, 0x0c,
    0x1f, 0x24, 0x3a, 0xd4, 0xc2, 0x6d, 0xe8, 0xb6
};

uint8_t test_point_z1_w[] = {
    0xee, 0xee, 0xeb, 0x49, 0x66, 0xd4, 0x0d, 0x60,
    0x59, 0xd9, 0xfe, 0xc8, 0x81, 0xfe, 0x82, 0x8f,
    0x26, 0x1f, 0x73, 0x41, 0x7e, 0x55, 0x3f, 0x56
};

uint8_t test_point_z2_w[] = {
    0x66, 0x17, 0x0c, 0xce, 0xa7, 0x11, 0xb0, 0xf5,
    0x45, 0x08, 0xbb, 0x33, 0x78, 0x8a, 0x6b, 0x88,
    0xdb, 0x75, 0x74, 0xd8, 0x9b, 0x16, 0x45, 0x5e
};

uint8_t test_point_z3_w[] = {
    0x1a, 0x42, 0xc6, 0x6a, 0xfa, 0x59, 0x0b, 0xb1,
    0xf7, 0xd8, 0xcb, 0xfa, 0xce, 0x4d, 0x80, 0x8a,
    0x30, 0x73, 0x5a, 0x50, 0x4c, 0xf3, 0xab, 0x04
};

ecc_combi_point_t test_point = {
    .chudnovsky = {
        .x = {
            .wordv = test_point_x_w,
            .length_W = sizeof(test_point_x_w),
            .info = 7
        },
        .y = {
            .wordv = test_point_y_w,
            .length_W = sizeof(test_point_y_w),
            .info = 7
        },
        .z1 = {
            .wordv = test_point_z1_w,
            .length_W = sizeof(test_point_z1_w),
            .info = 6
        },
        .z2 = {
            .wordv = test_point_z2_w,
            .length_W = sizeof(test_point_z2_w),
            .info = 6
        },
        .z3 = {
            .wordv = test_point_z3_w,
            .length_W = sizeof(test_point_z3_w),
            .info = 2
        }
    }
};
void testrun_genkey2(void){
    ecc_chudnovsky_point_t q;
    ecc_affine_point_t qa;

    printf_P(PSTR("\n== testing key generation (2) ==\n"));

    if(ecc_chudnovsky_point_alloc(&q, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }
    if(ecc_affine_point_alloc(&qa, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }

    ecc_chudnovsky_point_print(&test_point.chudnovsky);
    ecc_chudnovsky_to_affine_point(&qa, &test_point.chudnovsky, &nist_curve_p192);
    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    printf_P(PSTR("\n================\n"));
    ecc_chudnovsky_point_double_sp(&q, &test_point.chudnovsky, &nist_curve_p192);
    ecc_chudnovsky_point_print(&q);

    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    puts("\n");

}

void testrun_genkey1(void){
    ecc_chudnovsky_point_t q;
    ecc_affine_point_t qa;

    uint8_t k_w[] = {
    //  e5ce89a34adddf25ff3bf1ffe6803f57d0220de3118798ea
        0xea, 0x98, 0x87, 0x11, 0xe3, 0x0d, 0x22, 0xd0,
        0x57, 0x3f, 0x80, 0xe6, 0xff, 0xf1, 0x3b, 0xff,
        0x25, 0xdf, 0xdd, 0x4a, 0xa3, 0x89, 0xce, 0xe5
    };

    bigint_t k = {
            .length_W = sizeof(k_w),
            .wordv = k_w,
            .info = 7
    };

    printf_P(PSTR("\n== testing key generation ==\n"));

    if(ecc_chudnovsky_point_alloc(&q, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }
    if(ecc_affine_point_alloc(&qa, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }

    printf_P(PSTR("  k:  "));
    bigint_print_hex(&k);
    ecc_chudnovsky_multiplication(&q, &k, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    puts("\n");
}

void testrun_genkey3(void){
    ecc_chudnovsky_point_t q;
    ecc_affine_point_t qa;

    uint8_t k_w[] = {
        0xb2, 0x51, 0x97, 0xc3, 0x7c, 0x61, 0xf8, 0x8f,
        0x19, 0x91, 0xcc, 0x67, 0xb5, 0x1c, 0x34, 0x23,
        0xff, 0x13, 0xad, 0x14, 0x57, 0x43, 0x14, 0x7d
    };

    bigint_t k = {
            .length_W = sizeof(k_w),
            .wordv = k_w,
            .info = 6
    };

    printf_P(PSTR("\n== testing key generation ==\n"));

    if(ecc_chudnovsky_point_alloc(&q, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }
    if(ecc_affine_point_alloc(&qa, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }

    printf_P(PSTR("  k:  "));
    bigint_print_hex(&k);
    ecc_chudnovsky_double_and_add(&q, &k, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    puts("\n");
}

void testrun_genkey(void){
    ecc_chudnovsky_point_t q;
    ecc_affine_point_t qa;

    bigint_t k;

    printf_P(PSTR("\n== testing key generation ==\n"));

    printf_P(PSTR("enter secret key d: "));
    bigint_read_hex_echo(&k);
    putchar('\n');

    if(ecc_chudnovsky_point_alloc(&q, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }
    if(ecc_affine_point_alloc(&qa, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }

    printf_P(PSTR("  k:  "));
    bigint_print_hex(&k);
    ecc_chudnovsky_multiplication(&q, &k, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    puts("\n");
}


#endif

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]               PROGMEM = "echo-test";
const char reset_prng_str[]              PROGMEM = "reset-prng";
const char quick_test_str[]              PROGMEM = "quick-test";
const char performance_reduce_str[]      PROGMEM = "performance_reduce";
const char performance_invert_str[]      PROGMEM = "performance_invert";
const char performance_multiply_str[]    PROGMEM = "performance_multiply";
const char genkey1_str[]                 PROGMEM = "genkey1";
const char genkey2_str[]                 PROGMEM = "genkey2";
const char genkey3_str[]                 PROGMEM = "genkey3";
const char genkey_str[]                  PROGMEM = "genkey";
const char square_str[]                  PROGMEM = "square";
const char echo_str[]                    PROGMEM = "echo";

const const cmdlist_entry_t cmdlist[] PROGMEM = {
//	{ reset_prng_str,              NULL, reset_prng                           },
//	{ quick_test_str,              NULL, quick_test                           },
    { square_str,                  NULL, testrun_square                       },
    { genkey_str,                  NULL, testrun_genkey                       },
    { genkey1_str,                 NULL, testrun_genkey1                      },
    { genkey2_str,                 NULL, testrun_genkey2                      },
    { genkey3_str,                 NULL, testrun_genkey3                      },
	{ performance_reduce_str,      NULL, testrun_performance_reduce_bigint    },
    { performance_invert_str,      NULL, testrun_performance_invert_bigint    },
    { performance_multiply_str,    NULL, testrun_performance_multiply_bigint  },
	{ echo_str,                (void*)1, (void_fpt)echo_ctrl                  },
	{ NULL,                        NULL, NULL                                 }
};

int main (void){
    int8_t r;
    main_setup();

    for(;;){
        welcome_msg(algo_name);
        r = cmd_interface(cmdlist);
        printf("r = %"PRId8"\n", r);
        cli_putstr_P(PSTR("\r\nHello!\r\n"));
	}
}
