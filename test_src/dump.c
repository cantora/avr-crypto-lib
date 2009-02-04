/* dump.c */
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
/**
 * \file     dump.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-02-04
 * \license  GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include "cli.h" 
#include "string-extras.h" 
 
#define DUMP_WIDTH 16

static
void dump_chars(uint8_t* buffer){
	uint8_t i;
	cli_putc('|');
	for(i=0; i<DUMP_WIDTH; ++i){
		if(isprint(buffer[i])){
			cli_putc(buffer[i]);
		}else{
			cli_putc('.');
		}
	}
	cli_putc('|');
}


void dump(char* s){
	uint8_t flash=1;
	uint32_t addr=0;
	uint32_t size=128;
	uint8_t i,buffer[DUMP_WIDTH];
	char tstr[9];
	s=strstrip(s);
	if(*s=='r' || *s=='R' || *s=='m' || *s=='M')
		flash=0;
	if(isalpha(*s)){
		while(isalpha(*s))
			++s;
	}
	char* eptr;
	if(*s)
		addr = strtoul(s, &eptr, 0);
	if(eptr)
		size = strtoul(eptr, NULL, 0);
	if(!size)
		size = 32;	
	
	cli_putstr_P(PSTR("\r\ndumping "));	
	ultoa(size, tstr, 10);
	cli_putstr(tstr);
	cli_putstr_P(PSTR(" bytes of "));
	cli_putstr_P(flash?PSTR("flash"):PSTR("ram"));
	cli_putstr_P(PSTR(", beginning at 0x"));	
	ultoa(addr, tstr, 16);
	cli_putstr(tstr);
	cli_putstr_P(PSTR(":\r\n"));	
	while(size>=DUMP_WIDTH){
		if(flash){
			for(i=0; i<DUMP_WIDTH; ++i){
#ifdef pgm_read_byte_far				
				buffer[i]=pgm_read_byte_far(addr+i);
#else
				buffer[i]=pgm_read_byte(addr+i);
#endif
			}
		}else{
			memcpy(buffer, (void*)((uint16_t)addr), DUMP_WIDTH);
		}
		ultoa(addr, tstr, 16);
		sprintf(tstr,"%6lX", addr);
		cli_putstr(tstr);
		cli_putstr_P(PSTR(": "));	
		cli_hexdump2(buffer, DUMP_WIDTH);
		cli_putc('\t');
		dump_chars(buffer);
		addr+=DUMP_WIDTH;
		size-=DUMP_WIDTH;
		cli_putstr_P(PSTR("\r\n"));
	}
	if(size){
		if(flash){
			for(i=0; i<size; ++i){
#ifdef pgm_read_byte_far				
				buffer[i]=pgm_read_byte_far(addr+i);
#else
				buffer[i]=pgm_read_byte(addr+i);
#endif
			}
		}else{
			memcpy(buffer, (void*)((uint16_t)addr), size);
		}
		ultoa(addr, tstr, 16);
		sprintf(tstr,"%6lX", addr);
		cli_putstr(tstr);
		cli_putstr_P(PSTR(": "));	
		cli_hexdump2(buffer, size);
		cli_putstr_P(PSTR("\r\n"));
	}
}

