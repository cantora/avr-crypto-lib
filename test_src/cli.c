/* cli.c */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3 or later
 * 
 * components to help implementing simple command based interaction
 * 
 **/
 
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "config.h"

int16_t findstring_d0(const char* str, const char* v){
	uint8_t i=0;
	while(*v){	
		if(!strcmp(str, v)){
			return i;
		}
		while(*v++) /* go to the next string */
		;
		++i;
	}
	return -1;
}
 
int16_t findstring_d0_P(const char* str, PGM_P v){
	uint8_t i=0;
	while(pgm_read_byte(v)){	
		if(!strcmp_P(str, v)){
			return i;
		}
		while(pgm_read_byte(v++)) /* go to the next string */
		;
		++i;
	}
	return -1;
} 

#ifdef CLI_AUTO_HELP
#include "uart.h"

void cli_auto_help_P(PGM_P dbzstr){
	char c;
	uart_putstr_P(PSTR("\r\n[auto help] available commands are:\r\n\t"));
	do{
		while((c=pgm_read_byte(dbzstr++))!=0){
			uart_putc(c);
		}
		uart_putstr_P(PSTR("\r\n\t"));
	}while((c=pgm_read_byte(dbzstr))!=0);
	uart_putstr_P(PSTR("\r\n"));
}

#endif

int16_t execcommand_d0_P(const char* str, PGM_P v, void(*fpt[])(void) ){
	int16_t i=0;
	i=findstring_d0_P(str, v);
	if(i!=-1){
		if(fpt[i])
			fpt[i]();
	}
	cli_auto_help_P(v);
	return -1;
}


