/* bcal-performance.c */
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
 * \file    bcal-performance.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2010-02-16
 * \license GPLv3 or later
 *
 */

#include "bcal-performance.h"
#include "keysize_descriptor.h"
#include "blockcipher_descriptor.h"
#include "performance_test.h"
#include "stack_measuring.h"
#include "cli.h"
#include "uart_i.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define PATTERN_A 0xAA
#define PATTERN_B 0x55

/*
static
void printvalue(unsigned long v){
	char str[20];
	int i;
	ultoa(v, str, 10);
	for(i = 0; i < 10 - strlen(str); ++i){
		cli_putc(' ');
	}
	cli_putstr(str);
}
*/

void bcal_performance(const bcdesc_t *bcd){
	bcdesc_t bc;
	memcpy_P(&bc, bcd, sizeof(bcdesc_t));
	uint8_t ctx[bc.ctxsize_B];
	uint8_t data[(bc.blocksize_b + 7) / 8];
	uint16_t keysize = get_keysize(bc.valid_keysize_desc);
	uint8_t key[(keysize + 7) / 8];
	uint64_t t;
	uint8_t i;

	if(bc.type != BCDESC_TYPE_BLOCKCIPHER)
		return;
	calibrateTimer();
	print_overhead();
	printf_P(PSTR("\n\n === %S performance === \n"
	              "\ttype:             blockcipher\n"
	              "\tkeysize (bits):     %5"PRIu16"\n"
	              "\tctxsize (bytes):    %5"PRIu16"\n"
	              "\tblocksize (bits):   %5"PRIu16"\n"),
	        bc.name, keysize, bc.ctxsize_B, bc.blocksize_b);
	uart0_flush();
	t = 0;
	if(bc.init.init1){
		if((bc.flags & BC_INIT_TYPE) == BC_INIT_TYPE_1){
			for(i = 0; i < 32; ++i){
				startTimer(1);
				START_TIMER;
				(bc.init.init1)(key, &ctx);
				STOP_TIMER;
				t += stopTimer();
				if(i!=31 && bc.free){
					bc.free(&ctx);
				}
			}
		} else {
			for(i = 0; i < 32; ++i){
				startTimer(1);
				START_TIMER;
				(bc.init.init2)(key, keysize, &ctx);
				STOP_TIMER;
				t += stopTimer();
				if(i!=31 && bc.free){
					bc.free(&ctx);
				}
			}
		}
		t >>= 5;
		printf_P(PSTR("    init (cycles):      %5"PRIu16"\n"), t);
	}

    uart0_flush();
	t = 0;
	for(i = 0; i < 32; ++i){
		startTimer(0);
		START_TIMER;
		bc.enc.enc1(data, &ctx);
		STOP_TIMER;
		t += stopTimer();
	}
	t >>= 5;
	printf_P(PSTR("    encrypt (cycles):   %5"PRIu16"\n"), t);

    uart0_flush();
	t = 0;
	for(i = 0; i < 32; ++i){
		startTimer(0);
		START_TIMER;
		bc.dec.dec1(data, &ctx);
		STOP_TIMER;
		t += stopTimer();
	}
	t >>= 5;
	printf_P(PSTR("    decrypt (cycles):   %5"PRIu16"\n"), t);
    uart0_flush();

	if(bc.free){
	    uart0_flush();
		bc.free(&ctx);
	}
}

void bcal_stacksize(const bcdesc_t *bcd){
	bcdesc_t bc;
	stack_measuring_ctx_t smctx;
	memcpy_P(&bc, bcd, sizeof(bcdesc_t));
	uint8_t ctx[bc.ctxsize_B];
	uint8_t data[(bc.blocksize_b + 7) / 8];
	uint16_t keysize = get_keysize(bc.valid_keysize_desc);
	uint8_t key[(keysize + 7)/8];
	uint16_t t1 = 0, t2 = 0;

	if(bc.type != BCDESC_TYPE_BLOCKCIPHER)
		return;
	printf_P(PSTR("\n === %S stack-usage ===\n"),bc.name);

	uart0_flush();

	if(bc.init.init1){
		if((bc.flags & BC_INIT_TYPE) == BC_INIT_TYPE_1){
	    	cli();
			stack_measure_init(&smctx, PATTERN_A);
			bc.init.init1(&ctx, key);
			t1 = stack_measure_final(&smctx);
			stack_measure_init(&smctx, PATTERN_B);
			bc.init.init1(&ctx, key);
			t2 = stack_measure_final(&smctx);
			sei();
		} else {
	    	cli();
			stack_measure_init(&smctx, PATTERN_A);
			bc.init.init2(&ctx, keysize, key);
			t1 = stack_measure_final(&smctx);
			stack_measure_init(&smctx, PATTERN_B);
			bc.init.init2(&ctx, keysize, key);
            t2 = stack_measure_final(&smctx);
            sei();
        }

		t1 = (t1 > t2) ? t1 : t2;
		printf_P(PSTR("    init (bytes):       %5"PRIu16"\n"), t1);
	}
	cli();
	stack_measure_init(&smctx, PATTERN_A);
	bc.enc.enc1(data, &ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	bc.enc.enc1(data, &ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1 > t2) ? t1 : t2;
	printf_P(PSTR("    encBlock (bytes):   %5"PRIu16"\n"), t1);

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	bc.dec.dec1(data, &ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	bc.dec.dec1(data, &ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1 > t2) ? t1 : t2;
	printf_P(PSTR("    decBlock (bytes):   %5"PRIu16"\n"), t1);

	if(bc.free){
		bc.free(&ctx);
	}
}

void bcal_performance_multiple(const bcdesc_t *const *bcd_list){
	const bcdesc_t *bcd;
	for(;;){
		bcd = (void*)pgm_read_word(bcd_list);
		if(!bcd){
			puts_P(PSTR("\n End of performance figures\n"));
			return;
		}
		bcal_performance(bcd);
		bcal_stacksize(bcd);
		bcd_list = (void*)((uint8_t*)bcd_list + 2);
	}
}
