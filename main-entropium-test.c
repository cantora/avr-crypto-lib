/*
 * entropium test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "entropium.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* cipher_name = "Entropium";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_entropium(void){
	char c, str[16];
	uint8_t data[32];
	uint32_t i=0;
	while(!uart_getc_nb(&c)){
		entropium_getRandomBlock(data);
		uart_putstr_P(PSTR("\r\n "));
		ultoa(i, str, 10);
		for(c=strlen(str); c<11; ++c){
			uart_putc(' ');
		}
		uart_putstr(str);
		++i;
		uart_putstr_P(PSTR(" : "));
		uart_hexdump(data, 32);
	}
	uart_putstr_P(PSTR("\r\n\r\n"));
}


void testrun_performance_entropium(void){
	uint16_t i,c;
	uint64_t t;
	char str[16];
	uint8_t data[32];
	
	calibrateTimer();
	getOverhead(&c, &i);
	uart_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(c, str, 10);
	uart_putstr_P(PSTR("\r\n\tconst overhead:     "));
	uart_putstr(str);
	utoa(i, str, 10);
	uart_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	uart_putstr(str);
	
	startTimer(1);
	entropium_addEntropy(128, data);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tadd entropy time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	entropium_getRandomBlock(data);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tget random time:  "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_entropium, testrun_entropium, testrun_performance_entropium};

	while(1){ 
		if (!getnextwordn(str,20)){DEBUG_S("DBG: W1\r\n"); goto error;}
		if(execcommand_d0_P(str, u, v)<0){
			uart_putstr_P(PSTR("\r\nunknown command\r\n"));
		}
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}

