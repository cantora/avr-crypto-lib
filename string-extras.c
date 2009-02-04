/* string_extras.c */
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
 * \file	string_extras.c
 * \author  Daniel Otte 
 * \date    2006-05-16
 * \license	GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <ctype.h>

uint16_t stridentcnt_P(char* a, PGM_P b){
	uint16_t i=0;
	char c;
	for(;;){
		c = pgm_read_byte(b++);
		if(*a != c || c=='\0')
			return i;
		i++;
		a++;
	}
}

uint16_t firstword_length(char* s){
	uint16_t ret=0;
	while(isgraph(*s++))
		ret++;
	return ret; 
}

char* strstrip(char* str){
	if(!str)
		return str;
	char* endptr;
	while(*str && (*str==' ' || *str=='\t'))
		++str;
	endptr=str;
	while(*endptr)
		++endptr;
	do{
		--endptr;
	}while(*endptr==' ' || *endptr=='\t');
	endptr[1]='\0';
	return str;
}
