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
#include <ctype.h>
#include "hashfunction_descriptor.h"
#include "hfal-basic.h"
#include "shavs.h"
#include "string-extras.h"
#include "cli.h"

hfdesc_t*  shavs_algo=NULL;
hfdesc_t** shavs_algolist=NULL;

void shavs_listalgos(void){
	char option = 'a';
		
	hfdesc_t* t;
	uint8_t i=0;
	cli_putstr_P(PSTR("\r\nthe following algorithms are available:\r\n"));
	while(option<='z' && (t=(hfdesc_t*)pgm_read_word(&(shavs_algolist[i])))){
		cli_putc('\t');
		cli_putc((t==shavs_algo)?'*':' ');
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
		
		if(!shavs_algolist){
			cli_putstr_P(PSTR("\r\nERROR: shavs_algolist not set!"));
			return;
		}
		for(i=0; i<=option; ++i){
			if((void*)pgm_read_word(&(shavs_algolist[i]))==NULL){
				cli_putstr_P(PSTR("\r\nERROR: invalid selection!"));
				return;
			}
		}
		shavs_algo=(hfdesc_t*)pgm_read_word(&(shavs_algolist[option]));
	} else { /* name specifyed */ 
		hfdesc_t* t=NULL;
		uint8_t i=0;
		while((t=(hfdesc_t*)pgm_read_word(&(shavs_algolist[i]))) &&
		       strcasecmp_P(param, (void*)pgm_read_word(&(t->name))))
			++i;
		if(t){
			shavs_algo=t;
		}else{
			cli_putstr_P(PSTR("\r\nERROR: could not find \""));
			cli_putstr(param);
			cli_putstr_P(PSTR("\"!"));
		}	
	}
}

static uint16_t buffer_idx=0;
static uint8_t  in_byte=0;
static uint8_t* buffer;
static uint16_t buffersize_B;
static hfgen_ctx_t ctx;

static
uint8_t buffer_add(char c){
	uint8_t v,t;
	if(c>='0' && c<='9'){
		v=c-'0';
	}else{
		if(c>='a' && c<='f'){
			v=c-'a'+10;
		}else{
			if(c>='A' && c<='F'){
				v=c-'A'+10;
			}else{
				return 1;
			}
		}
			
	}
	if(buffer_idx==buffersize_B){
		hfal_hash_nextBlock(&ctx, buffer);
		buffer_idx=0;
		in_byte=0;
	}

	t=buffer[buffer_idx];
	if(in_byte){
		t = (t&0xF0) | v;
		buffer[buffer_idx]=t;
		buffer_idx++;
	}else{
		t = (t&0x0F) | (v<<4);
		buffer[buffer_idx]=t;
	}
	in_byte ^= 1;
	return 0;
}

void shavs_test1(void){
	char lenstr[21];
	char* len2;
	uint32_t length=0;
	uint8_t len_set=0;
	if(!shavs_algo){
		cli_putstr_P(PSTR("\r\nERROR: select algorithm first!"));
		return;
	}
	
	buffersize_B=pgm_read_word(&(shavs_algo->blocksize_b))/8;
	buffer = malloc(buffersize_B);

	for(;;){
		do{	
			cli_putstr_P(PSTR("\r\n"));
			cli_getsn(lenstr, 20);
			len2 = strstrip(lenstr);
			if(!strncasecmp_P(len2, PSTR("LEN"), 3)){
				while(*len2 && *len2!='=')
					len2++;
				if(*len2=='='){
					len2++;
					length=strtoul(len2, NULL, 0);
					len_set=1;
				}
			} else {
				if(!strncasecmp_P(len2, PSTR("EXIT"), 4)){
					return;
				}
			}		
		}while(!len_set);
		volatile int16_t expect_input;
		char c;
				
		if(length==0){
			expect_input=2;
		}else{
			expect_input=((length+7)/8)*2;
		}
		
		buffer_idx = 0;
		in_byte=0;
		len_set = 0;

		hfal_hash_init(shavs_algo, &ctx);
		cli_putstr_P(PSTR("\r\n"));
		while((c=cli_getc_cecho())!='M' && c!='m'){
			if(!isblank(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (1)!\r\n"));
				return;
			}
		}
		if((c=cli_getc_cecho())!='s' && c!='S'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (2)!\r\n"));
				return;
		}
		if((c=cli_getc_cecho())!='g' && c!='G'){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (3)!\r\n"));
				return;
		}
		while((c=cli_getc_cecho())!='='){
			if(!isblank(c)){
				cli_putstr_P(PSTR("\r\nERROR: wrong input (4)!\r\n"));
				return;
			}
		}
		
		buffer_idx=0;
		while(expect_input>0){
			c=cli_getc_cecho();
			if(buffer_add(c)==0){
				--expect_input;
			}else{
				if(!isblank((uint16_t)c)){
					cli_putstr_P(PSTR("\r\nERROR: wrong input (5) ("));
					cli_putc(c);
					cli_putstr_P(PSTR(")!\r\n"));
					return;
				}
			}
		}
		
		uint8_t diggest[pgm_read_word(shavs_algo->hashsize_b)/8];
		if(length%(buffersize_B*8)==0)
			hfal_hash_nextBlock(&ctx, buffer);
		hfal_hash_lastBlock(&ctx, buffer, length%(buffersize_B*8));
		hfal_hash_ctx2hash(diggest, &ctx);
		hfal_hash_free(&ctx);
		cli_putstr_P(PSTR("\r\n MD = "));
		cli_hexdump(diggest, pgm_read_word(&(shavs_algo->hashsize_b))/8);
		
	}
	
}

