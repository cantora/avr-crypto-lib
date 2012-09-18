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

void testrun_performance_bigint(void){
    printf_P(PSTR("\n=== performance measurement ===\n"));
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

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]        PROGMEM = "echo-test";
const char reset_prng_str[]       PROGMEM = "reset-prng";
const char quick_test_str[]       PROGMEM = "quick-test";
const char performance_str[]      PROGMEM = "performance";
const char echo_str[]             PROGMEM = "echo";

const const cmdlist_entry_t cmdlist[] PROGMEM = {
//	{ reset_prng_str,       NULL, reset_prng                    },
//	{ quick_test_str,       NULL, quick_test                    },
	{ performance_str,      NULL, testrun_performance_bigint    },
	{ echo_str,         (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                 NULL, NULL                          }
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
