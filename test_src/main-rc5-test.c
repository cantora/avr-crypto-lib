/* main-rc5-test.c */
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
 * rc5 test-suit
 * 
*/

#include "main-test-common.h"

#include "rc5.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_rc5.h"

#define RC5_ROUNDS 12
char *algo_name = "RC5-32/12/16";

const bcdesc_t *const algolist[] PROGMEM = {
	(bcdesc_t*)&rc5_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void rc5_genctx_dummy(uint8_t *key, uint16_t keysize_b, void *ctx){
	rc5_init(key, keysize_b, RC5_ROUNDS, ctx);
}

void testrun_nessie_rc5(void){
	bcal_nessie_multiple(algolist);
}


void testrun_performance_rc5(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_rc5 },
	{ test_str,        NULL, testrun_nessie_rc5},
	{ performance_str, NULL, testrun_performance_rc5},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
    main_setup();

    for(;;){
        welcome_msg(algo_name);
        cmd_interface(cmdlist);
	}
}
