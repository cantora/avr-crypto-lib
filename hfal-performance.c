/* hfal-performance.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file    hfal-performance.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-10
 * \license GPLv3 or later
 * 
 */

#include "hfal-performance.h"
#include "hashfunction_descriptor.h"
#include "cli.h"
#include "performance_test.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>


static
void printvalue(unsigned int v){
	char str[20];
	int i;
	ultoa(v, str, 10);
	for(i=0; i<10-strlen(str); ++i){
		cli_putc(' ');
	}
	cli_putstr(str);
}

void hfal_performance(const hfdesc_t* hd){
	hfdesc_t hf;
	memcpy_P(&hf, hd, sizeof(hfdesc_t));
	uint8_t ctx[hf.ctxsize_B];
	uint8_t data[(hf.blocksize_b+7)/8];
	uint8_t digest[(hf.hashsize_b+7)/8];
	uint64_t t;
	
	if(hf.type!=HFDESC_TYPE_HASHFUNCTION)
		return;
	calibrateTimer();
	cli_putstr_P(PSTR("\r\n\r\n === "));
	cli_putstr_P(hf.name);
	cli_putstr_P(PSTR(" performance === "
	                  "\r\n    hashsize (bits):    "));
	printvalue(hf.hashsize_b);
	
	cli_putstr_P(PSTR("\r\n    ctxsize (bytes):    "));
	printvalue(hf.ctxsize_B);
	
	cli_putstr_P(PSTR("\r\n    blocksize (bits):   "));
	printvalue(hf.blocksize_b);
	
	startTimer(0);
	START_TIMER;
	hf.init(&ctx);
	STOP_TIMER;
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n    init (cycles):      "));
	printvalue(t);
	
	startTimer(0);
	START_TIMER;
	hf.nextBlock(&ctx, data);
	STOP_TIMER;
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n    nextBlock (cycles): "));
	printvalue(t);
	
	startTimer(0);
	START_TIMER;
	hf.lastBlock(&ctx, data, 0);
	STOP_TIMER;
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n    lastBlock (cycles): "));
	printvalue(t);
	
	startTimer(0);
	START_TIMER;
	hf.ctx2hash(digest, &ctx);
	STOP_TIMER;
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n    ctx2hash (cycles):  "));
	printvalue(t);
}


void hfal_performance_multiple(const hfdesc_t** hd_list){
	const hfdesc_t* hd;
	for(;;){
		hd = (void*)pgm_read_word(hd_list);
		if(!hd)
			return;
		hfal_performance(hd);
		hd_list = (void*)((uint8_t*)hd_list + 2);
	}
}

