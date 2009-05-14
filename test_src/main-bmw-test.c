/* main-bmw-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
 * BlueMidnightWish test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "bmw_small.h"
#include "bmw_large.h"
#include "cli.h"
#include "hfal_bmw_small.h"
#include "hfal_bmw_large.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "BlueMidnightWish";


const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&bmw224_desc,
	(hfdesc_t*)&bmw256_desc,
	(hfdesc_t*)&bmw384_desc,
	(hfdesc_t*)&bmw512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void performance_bmw(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_bmw(void){
	hfal_nessie_multiple(algolist);
}

void bmw224_test(void* msg, uint32_t length_b){
	hfal_test(&bmw224_desc, msg, length_b);
}

void bmw256_test(void* msg, uint32_t length_b){
	hfal_test(&bmw256_desc, msg, length_b);
}

void bmw384_test(void* msg, uint32_t length_b){
	hfal_test(&bmw384_desc, msg, length_b);
}

void bmw512_test(void* msg, uint32_t length_b){
	hfal_test(&bmw512_desc, msg, length_b);
}

void testrun_stdtest_bmw(void){
	char* msg0 = "abc";
	char* msg1 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"; 
	bmw224_test(msg0, strlen(msg0)*8);
	bmw224_test(msg1, strlen(msg1)*8);
	bmw256_test(msg0, strlen(msg0)*8);
	bmw256_test(msg1, strlen(msg1)*8);
	bmw384_test(msg0, strlen(msg0)*8);
	bmw384_test(msg1, strlen(msg1)*8);
	bmw512_test(msg0, strlen(msg0)*8);
	bmw512_test(msg1, strlen(msg1)*8);
}

void testshort(void){
	char* msg0 = "abc";
	bmw224_test(msg0, strlen(msg0)*8);
}

void testlshort(void){
	char* msg0 = "abc";
	bmw384_test(msg0, strlen(msg0)*8);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testshort_str[]   PROGMEM = "short";
const char testlshort_str[]  PROGMEM = "lshort";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_bmw},
	{ test_str,            NULL, testrun_stdtest_bmw},
	{ testshort_str,       NULL, testshort},
	{ testlshort_str,      NULL, testlshort},
	{ performance_str,     NULL, performance_bmw},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&bmw256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		
		cmd_interface(cmdlist);
	}
}	
