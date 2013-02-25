/* main-arcfour-test.c */
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
 * arcfour (RC4 compatible) test-suit
 * 
*/


#include "main-test-common.h"

#include <arcfour.h>
#include "performance_test.h"

#include "scal_arcfour.h"
#include "scal-basic.h"
#include "scal-nessie.h"

char *algo_name = "Arcfour";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void arcfour_genctx_dummy(uint8_t *key, uint16_t keysize, void *ctx){
	arcfour_init(key, (uint8_t)((keysize+7)/8), ctx);
}

void testrun_nessie_arcfour(void){
	scal_nessie_run(&arcfour_desc);
}

void testrun_performance_arcfour(void){
	uint32_t t;
	uint8_t key[16];
	arcfour_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();	
	
	memset(key,  0, 16);
	
    uart0_flush();
	startTimer(1);
	arcfour_init(key, 16, &ctx);
	t = stopTimer();
	printf_P(PSTR("\tctx-gen time: %10"PRIu32"\n"), t);
	uart0_flush();
	startTimer(1);
	arcfour_gen(&ctx);
	t = stopTimer();
	printf_P(PSTR("\tencrypt time: %10"PRIu32"\n"), t);
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_arcfour },
	{ test_str,        NULL, testrun_nessie_arcfour},
	{ performance_str, NULL, testrun_performance_arcfour},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main(void) {
	main_setup();

	for(;;){
		welcome_msg(algo_name);
		cmd_interface(cmdlist);
    }

}


