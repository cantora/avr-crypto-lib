/* shavs.c */
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
 * \file	shavs.c
 * \author  Daniel Otte 
 * \date    2006-05-16
 * \license	GPLv3 or later
 * 
 */

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include "hashfunction_descriptor.h"
#include "shavs.h"
#include "string-extras.h"
#include "cli.h"

hfdesc_t*  algo=NULL;
hfdesc_t** algolist=NULL;

void shavs_listalgos(void){
	char option = 'a';
	
	cli_putstr_P(PSTR("\r\nDBG: &algolist: "));
	cli_hexdump(&algolist, 2);
	cli_putstr_P(PSTR("\r\nDBG: algolist[0]: "));
	cli_hexdump(algolist, 8);
	
	hfdesc_t* t;
	uint8_t i=0;
	cli_putstr_P(PSTR("\r\nthe following algorithms are available:\r\n"));
	while(option<='z' && (t=(hfdesc_t*)pgm_read_word(&(algolist[i])))){
		cli_putc('\t');
		cli_putc((t==algo)?'*':' ');
		cli_putc(option++);
		cli_putstr_P(PSTR(":\t"));
		cli_putstr_P((void*)(pgm_read_word(&(t->name))));
		cli_putstr_P(PSTR("\r\n"));
		i++;
	}
}

void shavs_setalgo(char* param){
	param = strstrip(param);
	if(param[1]=='\0'){ /* single letter specified */
		uint8_t i,option = param[0]-'a';
		
		if(!algolist){
			cli_putstr_P(PSTR("\r\nERROR: algolist not set!"));
			return;
		}
		for(i=0; i<=option; ++i){
			if((void*)pgm_read_word(&(algolist[i]))==NULL){
				cli_putstr_P(PSTR("\r\nERROR: invalid selection!"));
				return;
			}
		}
		algo=(hfdesc_t*)pgm_read_word(&(algolist[option]));
	} else { /* name specifyed */ 
		hfdesc_t* t=NULL;
		uint8_t i=0;
		while((t=(hfdesc_t*)pgm_read_word(&(algolist[i]))) &&
		       strcasecmp_P(param, (void*)pgm_read_word(&(t->name))))
			++i;
		if(t){
			algo=t;
		}else{
			cli_putstr_P(PSTR("\r\nERROR: could not find \""));
			cli_putstr(param);
			cli_putstr_P(PSTR("\"!"));
		}	
	}
}

