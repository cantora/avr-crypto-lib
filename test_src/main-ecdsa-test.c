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

#include <ctype.h>

#include "main-test-common.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"
#include "nist_p192.h"
#include "ecc.h"
#include "ecdsa_sign.h"

#include "hfal_sha1.h"
#include "hfal_sha224.h"
#include "hfal_sha256.h"
#include "hfal_sha384.h"
#include "hfal_sha512.h"
#include "hfal-basic.h"

#include "performance_test.h"
#include "hfal_sha1.h"
#include "base64_enc.h"
#include "base64_dec.h"

char *algo_name = "ECDSA";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

uint8_t prng_get_byte(void){
    return random8();
}

const hfdesc_t *hash_select(void){
    const hfdesc_t *hashes[] = { &sha1_desc, &sha224_desc, &sha256_desc, &sha384_desc, &sha512_desc };
    uint8_t index;
    int selection;
    printf_P(PSTR("Please select one of the following hash functions:\n"));
    for(index = 0; index < sizeof(hashes) / sizeof(hashes[0]); ++index){
        printf_P(PSTR("  %c: %S\n"), index + 'a', pgm_read_word(&(hashes[index]->name)));
    }
    do{
        printf_P(PSTR("[a]: "));
        selection = getchar();
        putchar(selection);
        if(selection == '\n' || selection == '\r'){
            selection = 0;
        }else{
            putchar('\n');
            selection -= 'a';
        }
    }while(selection > sizeof(hashes) / sizeof(hashes[0]));
    return hashes[selection];
}

uint8_t convert_hexchar_to_value(char a){
    if(a >= 0 && a <= 9){
        return a - '0';
    }
    if(a >= 'a' && a <= 'f'){
        return a - 'a';
    }
    if(a >= 'A' && a <= 'F'){
        return a - 'A';
    }
    return 0;
}


uint8_t convert_hex_to_byte(char a, char b){
    return (convert_hexchar_to_value(a) << 4) | convert_hexchar_to_value(b);
}

void *hash_message(hfdesc_t *hash_function){
    uint8_t *block, *hash_value;
    uint16_t index = 0;
    hfgen_ctx_t ctx;
    if(hash_function == NULL){
        return NULL;
    }
    block = malloc(hfal_hash_getBlocksize(hash_function) / 8);
    if(block == NULL){
        return NULL;
    }
    hash_value = malloc(hfal_hash_getHashsize(hash_function) / 8);
    if(hash_value == NULL){
        free(block);
        return NULL;
    }
    hfal_hash_init(hash_function, &ctx);
    for(;;){
        int a,b;
        a = getchar();
        if(!isxdigit(a)){
            hfal_hash_lastBlock(&ctx, block, index * 8);
            break;
        }
        putchar(a);
        b = getchar();
        if(!isxdigit(b)){
            printf_P(PSTR("*** invalid ***\n"));
            hfal_hash_free(&ctx);
            free(hash_value);
            free(block);
            return NULL;
        }
        putchar(b);
        block[index++] = convert_hex_to_byte(a, b);
        if(index == hfal_hash_getBlocksize(hash_function) / 8){
            hfal_hash_nextBlock(&ctx, block);
        }
    }
    hfal_hash_ctx2hash(hash_value, &ctx);
    hfal_hash_free(&ctx);
    free(block);
    return hash_value;
}

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
    uint32_t time;
    bigint_t k;
    uint8_t r;

    printf_P(PSTR("\n== testing key generation ==\n"));

    printf_P(PSTR("enter secret key d: "));
    bigint_read_hex_echo(&k);
    putchar('\n');

    if(ecc_chudnovsky_point_alloc(&q, 192)){
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }
    if(ecc_affine_point_alloc(&qa, 192)){
        ecc_chudnovsky_point_free(&q);
        printf_P(PSTR("ERROR: OOM! <%s %s %d>\n"), __FILE__, __func__, __LINE__);
        return;
    }

    printf_P(PSTR("(naf)  k:  "));
    bigint_print_hex(&k);
    startTimer(1);
    START_TIMER;
    r = ecc_chudnovsky_naf_multiplication(&q, &k, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    STOP_TIMER;
    time = stopTimer();
    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    printf_P(PSTR("\n time: %"PRIu32" cycles (r code: %"PRIu8")\n"), time, r);

    printf_P(PSTR("(d&a)  k:  "));
    bigint_print_hex(&k);
    startTimer(1);
    START_TIMER;
    r = ecc_chudnovsky_double_and_add(&q, &k, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    STOP_TIMER;
    time = stopTimer();
    ecc_chudnovsky_to_affine_point(&qa, &q, &nist_curve_p192);

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&qa.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&qa.y);
    printf_P(PSTR("\n time: %"PRIu32" cycles (r code: %"PRIu8")\n"), time, r);
    free(k.wordv);
    ecc_chudnovsky_point_free(&q);
    ecc_affine_point_free(&qa);
}


#endif

const uint8_t ecdsa_test_1_msg[] PROGMEM = {
/*
    0xcf, 0x71, 0xa0, 0xe4, 0xce, 0x59, 0x43, 0x11,
    0x77, 0x88, 0x50, 0x87, 0x53, 0x78, 0xd0, 0xee,
    0xa3, 0xc0, 0x32, 0xa4, 0xbc, 0xc0, 0xdc, 0x1c,
    0xf2, 0x9d, 0x01, 0xb9, 0xc5, 0x10, 0x78, 0x9c,
    0xd5, 0x2f, 0xc3, 0x8c, 0x74, 0xe6, 0xa4, 0x27,
    0x87, 0xd0, 0xf2, 0x7c, 0xe2, 0x93, 0x20, 0x7a,
    0xfd, 0xd0, 0x11, 0x7a, 0xcc, 0x71, 0xb9, 0x16,
    0x63, 0x06, 0xce, 0x56, 0xf1, 0xa7, 0xf1, 0xc6,
    0x0a, 0x9d, 0x68, 0x7d, 0x12, 0x5e, 0xb0, 0x7e,
    0x26, 0xe5, 0x51, 0xdc, 0x14, 0x0e, 0x8a, 0x04,
    0xaf, 0xa2, 0xa1, 0x6f, 0x98, 0xb5, 0x1b, 0xa9,
    0x18, 0x96, 0xbf, 0x32, 0x0f, 0xd4, 0xd6, 0xf1,
    0xa4, 0x4b, 0x46, 0xf3, 0x3d, 0xae, 0x39, 0xcc,
    0x24, 0xf0, 0x4a, 0x5d, 0x86, 0x0c, 0xb1, 0x4f,
    0x6b, 0x6e, 0x8a, 0x69, 0x73, 0xb4, 0x9f, 0xd2,
    0xa7, 0xbc, 0xeb, 0x48, 0xd7, 0x48, 0xf7, 0xeb
*/
    0xeb, 0xf7, 0x48, 0xd7, 0x48, 0xeb, 0xbc, 0xa7,
    0xd2, 0x9f, 0xb4, 0x73, 0x69, 0x8a, 0x6e, 0x6b,
    0x4f, 0xb1, 0x0c, 0x86, 0x5d, 0x4a, 0xf0, 0x24,
    0xcc, 0x39, 0xae, 0x3d, 0xf3, 0x46, 0x4b, 0xa4,
    0xf1, 0xd6, 0xd4, 0x0f, 0x32, 0xbf, 0x96, 0x18,
    0xa9, 0x1b, 0xb5, 0x98, 0x6f, 0xa1, 0xa2, 0xaf,
    0x04, 0x8a, 0x0e, 0x14, 0xdc, 0x51, 0xe5, 0x26,
    0x7e, 0xb0, 0x5e, 0x12, 0x7d, 0x68, 0x9d, 0x0a,
    0xc6, 0xf1, 0xa7, 0xf1, 0x56, 0xce, 0x06, 0x63,
    0x16, 0xb9, 0x71, 0xcc, 0x7a, 0x11, 0xd0, 0xfd,
    0x7a, 0x20, 0x93, 0xe2, 0x7c, 0xf2, 0xd0, 0x87,
    0x27, 0xa4, 0xe6, 0x74, 0x8c, 0xc3, 0x2f, 0xd5,
    0x9c, 0x78, 0x10, 0xc5, 0xb9, 0x01, 0x9d, 0xf2,
    0x1c, 0xdc, 0xc0, 0xbc, 0xa4, 0x32, 0xc0, 0xa3,
    0xee, 0xd0, 0x78, 0x53, 0x87, 0x50, 0x88, 0x77,
    0x11, 0x43, 0x59, 0xce, 0xe4, 0xa0, 0x71, 0xcf
};

const uint8_t ecdsa_test_1_d[] PROGMEM = {
    0xf3, 0xd7, 0x60, 0xd6, 0x75, 0xf2, 0xcc, 0xeb,
    0xf0, 0xd2, 0xfd, 0xb3, 0xb9, 0x41, 0x3f, 0xb0,
    0xf8, 0x4f, 0x37, 0xd1, 0xb3, 0x37, 0x4f, 0xe1
};

const uint8_t ecdsa_test_1_k[] PROGMEM = {
    0x25, 0x5f, 0x68, 0x89, 0xa2, 0x31, 0xbc, 0x57,
    0x4d, 0x15, 0xc4, 0x12, 0xfb, 0x56, 0x45, 0x68,
    0x83, 0x07, 0xa1, 0x43, 0x70, 0xbc, 0x0a, 0xcb
};

void hash_mem_P(const hfdesc_t *hfdesc, void *dest, const void *msg, uint16_t msg_len_b){
    uint16_t blocksize = hfal_hash_getBlocksize(hfdesc);
    uint8_t block[blocksize / 8];
    hfgen_ctx_t ctx;
    hfal_hash_init(hfdesc, &ctx);
    while(msg_len_b > blocksize){
        memcpy_P(block, msg, blocksize / 8);
        msg = (uint8_t*)msg + blocksize / 8;
        msg_len_b -= blocksize;
        hfal_hash_nextBlock(&ctx, block);
    }
    memcpy_P(block, msg, (msg_len_b + 7) / 8);
    hfal_hash_lastBlock(&ctx, block, msg_len_b);
    hfal_hash_ctx2hash(dest,  &ctx);
    hfal_hash_free(&ctx);
}

const uint8_t ecdsa_test_2_msg[] PROGMEM = {
    0x66, 0xa2, 0x51, 0x3d, 0x7b, 0x60, 0x45, 0xe5,
    0x66, 0x79, 0xb0, 0x32, 0xca, 0xd4, 0x5f, 0xb1,
    0x82, 0x28, 0x9c, 0xa7, 0x6a, 0x88, 0xc0, 0x6d,
    0x78, 0xc8, 0x5f, 0x3d, 0xd3, 0x80, 0x45, 0x90,
    0x20, 0x5b, 0x73, 0xa7, 0x84, 0x24, 0x9a, 0x0a,
    0x0c, 0x8b, 0xf2, 0xf2, 0x21, 0x45, 0xd1, 0x05,
    0x21, 0x9b, 0x48, 0x0d, 0x74, 0x60, 0x7c, 0x02,
    0xb8, 0xa6, 0xb6, 0xb4, 0x59, 0x25, 0x9e, 0x4f,
    0xdf, 0xe2, 0xbd, 0xb4, 0xab, 0x22, 0x38, 0x01,
    0x75, 0x35, 0x29, 0x1d, 0x7a, 0xc1, 0xab, 0xda,
    0x66, 0xc4, 0xf6, 0xdc, 0xea, 0x9e, 0x5d, 0x0b,
    0xf0, 0x5a, 0x93, 0x06, 0xf3, 0x33, 0xb0, 0x0e,
    0x56, 0x34, 0x2f, 0x75, 0x53, 0x40, 0x21, 0x1a,
    0xc2, 0x94, 0xac, 0x21, 0xa7, 0xc2, 0xb2, 0x67,
    0x12, 0xb8, 0x79, 0x95, 0x1b, 0x2e, 0x23, 0xf6,
    0x48, 0x7e, 0x4d, 0x39, 0x89, 0x9f, 0xe3, 0x74
};

const uint8_t ecdsa_test_2_d[] PROGMEM = {
    0xeb, 0x8e, 0x9f, 0x04, 0x7d, 0xb5, 0x9a, 0x80,
    0x34, 0x6f, 0xcd, 0xf1, 0xcc, 0x33, 0xbb, 0x78,
    0xbe, 0xc6, 0xb8, 0x76, 0xaf, 0x9f, 0x4b, 0x69
};

const uint8_t ecdsa_test_2_k[] PROGMEM = {
    0x8e, 0xd5, 0x00, 0x34, 0x08, 0x09, 0x60, 0x36,
    0x2e, 0xfe, 0x16, 0xd0, 0x53, 0x37, 0xa2, 0xf5,
    0x47, 0xfa, 0x11, 0xbc, 0xb1, 0xc2, 0xe8, 0x41
};

void test_sign1(void){
    bigint_word_t d_w[sizeof(ecdsa_test_1_d)];
    uint8_t rnd[sizeof(ecdsa_test_1_k)];
    uint8_t *hash;
    bigint_t d;
    const hfdesc_t *hash_desc;
    ecc_combi_point_t q;
    ecdsa_signature_t sign;
    ecdsa_ctx_t ctx;
    uint8_t r;

    putchar('\n');
    d.wordv = d_w;
    memcpy_P(rnd, ecdsa_test_1_k, sizeof(ecdsa_test_1_k));
    memcpy_P(d_w, ecdsa_test_1_d, sizeof(ecdsa_test_1_d) * sizeof(bigint_word_t));
    d.length_W = sizeof(ecdsa_test_1_d) / sizeof(bigint_word_t);
    d.info = 0;
    bigint_adjust(&d);

    hash_desc = &sha1_desc; //hash_select();
    hash = malloc(hfal_hash_getHashsize(hash_desc) / 8);
    if(hash == NULL){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
    }
    hash_mem_P(hash_desc, hash, ecdsa_test_1_msg, sizeof(ecdsa_test_1_msg) * 8);
    printf_P(PSTR("msg hash: "));
    cli_hexdump(hash, hfal_hash_getHashsize(hash_desc) / 8);
    putchar('\n');

    ecc_chudnovsky_point_alloc(&q.chudnovsky, nist_curve_p192_p.length_W * sizeof(bigint_word_t));
    ctx.basepoint = &nist_curve_p192_basepoint.chudnovsky;
    ctx.priv = &d;
    ctx.curve = &nist_curve_p192;

    printf("\n  d:  ");
    bigint_print_hex(&d);
    printf_P(PSTR("\n  Gx: "));
    bigint_print_hex(&nist_curve_p192_basepoint.affine.x);
    printf_P(PSTR("\n  Gy: "));
    bigint_print_hex(&nist_curve_p192_basepoint.affine.y);

    r = ecc_chudnovsky_multiplication(&q.chudnovsky, &d, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    if(r){
        printf_P(PSTR("ERROR: ecc_chudnovsky_multiplication() returned: %"PRIu8"\n"), r);
    }
    r = ecc_chudnovsky_to_affine_point(&q.affine, &q.chudnovsky, &nist_curve_p192);
    if(r){
        printf_P(PSTR("ERROR: ecc_chudnovsky_to_affine_point() returned: %"PRIu8"\n"), r);
    }

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&q.affine.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&q.affine.y);
    putchar('\n');
    ctx.pub = &q.affine;

    ecdsa_signature_alloc(&sign, sizeof(ecdsa_test_1_d) * sizeof(bigint_word_t));

    r = ecdsa_sign_hash(&sign, hash, hfal_hash_getHashsize(hash_desc) / 8, &ctx, rnd);
    if(r){
        printf_P(PSTR("ERROR: ecdsa_sign_message() returned: %"PRIu8"\n"), r);
    }
    printf_P(PSTR("  r: "));
    bigint_print_hex(&sign.r);
    printf_P(PSTR("\n  s: "));
    bigint_print_hex(&sign.s);

    free(hash);
    ecdsa_signature_free(&sign);
    ecc_chudnovsky_point_free(&q.chudnovsky);
}

void test_sign2(void){
    bigint_word_t d_w[sizeof(ecdsa_test_2_d)];
    uint8_t rnd[sizeof(ecdsa_test_2_k)];
    uint8_t *hash;
    bigint_t d;
    const hfdesc_t *hash_desc;
    ecc_combi_point_t q;
    ecdsa_signature_t sign;
    ecdsa_ctx_t ctx;
    uint8_t r;

    putchar('\n');
    d.wordv = d_w;
    memcpy_P(rnd, ecdsa_test_2_k, sizeof(ecdsa_test_2_k));
    memcpy_P(d_w, ecdsa_test_2_d, sizeof(ecdsa_test_2_d) * sizeof(bigint_word_t));
    d.length_W = sizeof(ecdsa_test_2_d) / sizeof(bigint_word_t);
    d.info = 0;
    bigint_adjust(&d);

    hash_desc = &sha224_desc; //hash_select();
    hash = malloc(hfal_hash_getHashsize(hash_desc) / 8);
    if(hash == NULL){
        printf_P(PSTR("DBG: XXX <%S %s %d>\n"), PSTR(__FILE__), __func__, __LINE__);
    }
    hash_mem_P(hash_desc, hash, ecdsa_test_2_msg, sizeof(ecdsa_test_1_msg) * 8);
    printf_P(PSTR("msg hash: "));
    cli_hexdump(hash, hfal_hash_getHashsize(hash_desc) / 8);
    putchar('\n');

    ecc_chudnovsky_point_alloc(&q.chudnovsky, nist_curve_p192_p.length_W * sizeof(bigint_word_t));
    ctx.basepoint = &nist_curve_p192_basepoint.chudnovsky;
    ctx.priv = &d;
    ctx.curve = &nist_curve_p192;

    printf("\n  d:  ");
    bigint_print_hex(&d);
    printf_P(PSTR("\n  Gx: "));
    bigint_print_hex(&nist_curve_p192_basepoint.affine.x);
    printf_P(PSTR("\n  Gy: "));
    bigint_print_hex(&nist_curve_p192_basepoint.affine.y);

    r = ecc_chudnovsky_multiplication(&q.chudnovsky, &d, &nist_curve_p192_basepoint.chudnovsky, &nist_curve_p192);
    if(r){
        printf_P(PSTR("ERROR: ecc_chudnovsky_multiplication() returned: %"PRIu8"\n"), r);
    }
    r = ecc_chudnovsky_to_affine_point(&q.affine, &q.chudnovsky, &nist_curve_p192);
    if(r){
        printf_P(PSTR("ERROR: ecc_chudnovsky_to_affine_point() returned: %"PRIu8"\n"), r);
    }

    printf_P(PSTR("\n  Qx: "));
    bigint_print_hex(&q.affine.x);
    printf_P(PSTR("\n  Qy: "));
    bigint_print_hex(&q.affine.y);
    putchar('\n');
    ctx.pub = &q.affine;

    ecdsa_signature_alloc(&sign, sizeof(ecdsa_test_2_d) * sizeof(bigint_word_t));

    r = ecdsa_sign_hash(&sign, hash, hfal_hash_getHashsize(hash_desc) / 8, &ctx, rnd);
    if(r){
        printf_P(PSTR("ERROR: ecdsa_sign_message() returned: %"PRIu8"\n"), r);
    }
    printf_P(PSTR("  r: "));
    bigint_print_hex(&sign.r);
    printf_P(PSTR("\n  s: "));
    bigint_print_hex(&sign.s);

    free(hash);
    ecdsa_signature_free(&sign);
    ecc_chudnovsky_point_free(&q.chudnovsky);
}
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
const char testsign1_str[]               PROGMEM = "testsign1";
const char testsign2_str[]               PROGMEM = "testsign2";
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
    { testsign1_str,               NULL, test_sign1                           },
    { testsign2_str,               NULL, test_sign2                           },
	{ performance_reduce_str,      NULL, testrun_performance_reduce_bigint    },
    { performance_invert_str,      NULL, testrun_performance_invert_bigint    },
    { performance_multiply_str,    NULL, testrun_performance_multiply_bigint  },
	{ echo_str,                (void*)1, (void_fpt)echo_ctrl                  },
	{ NULL,                        NULL, NULL                                 }
};

int main (void){
    int8_t r;
    main_setup();
    calibrateTimer();
    for(;;){
        welcome_msg(algo_name);
        r = cmd_interface(cmdlist);
        printf("r = %"PRId8"\n", r);
        cli_putstr_P(PSTR("\r\nHello!\r\n"));
	}
}
