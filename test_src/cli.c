/* cli.c */
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
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3 or later
 * 
 * components to help implementing simple command based interaction
 * 
 **/
 
#include <stdlib.h> 
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "string-extras.h"
#include "cli.h"
#include "config.h"
#include "hexdigit_tab.h"

cli_rx_fpt cli_rx = NULL;
cli_tx_fpt cli_tx = NULL;
uint8_t cli_echo=1;

/**
 * \brief output a character to the console
 * 
 */

void cli_putc(char c){
	if(cli_tx)
		cli_tx(c);
}

/**
 * \brief get a character from the console
 * Gets a character from the console input and blocks
 * until a character is recieved
 */
uint16_t cli_getc(void){
	if(cli_rx)
		return cli_rx();
	return ((uint16_t)-1);
}

/**
 * \brief get a character from the console
 * Gets a char from the console input (like cli_getc())
 * and echos it back to the console if echo is enabled.
 */
uint16_t cli_getc_cecho(void){
	char c;
	if(cli_rx){
		c = cli_rx();
		if(cli_tx && cli_echo)
			cli_tx(c);
		return c;
	}
	return ((uint16_t)-1);
}

/**
 * \brief ouputs a zero-terminated string from ram to the console 
 */
void cli_putstr(const char* s){
	if(!cli_tx)
		return;
	while(*s)
		cli_tx(*s++);
}


/**
 * \brief ouputs a zero-terminated string from flash to the console 
 */
void cli_putstr_P(PGM_P s){
	char c;
	if(!cli_tx)
		return;
	for(;;){
		c = pgm_read_byte(s++);
		if(!c)
			return;
		cli_tx(c);
	}
}

/**
 * \brief reads a line or max n characters from the console
 * Writes characters from the console into the supplyed buffer until a '\r'
 * character is recieved or until n character a read (whatever happens first).
 * The string will always be terminated by a '\0' character, so the buffer
 * should have at least a size of n+1. 
 */
uint8_t cli_getsn(char* s, uint16_t n){
	char c;
	if(n==0)
		return 2;
	while((c=cli_getc_cecho())!='\0' && c!='\r' && n--){
		*s++=c;
	}
	*s='\0';
	return (c=='\r')?0:1;
}

/**
 * \brief dumps the contents of a buffer to the console
 * Dumps length bytes from data to the console ouput. The dump
 * will have 2*n continous hexadecimal characters.
 */
void cli_hexdump(const void* data, uint16_t length){
	if(!cli_tx)
		return;
	while(length--){
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))>>4)));
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))&0xf)));
		data = (uint8_t*)data +1;
	}
}

/**
 * \brief dumps the contents of a buffer to the console
 * This function behaves like cli_hexdump except that the
 * bytes are dumped in reverse order. This is usefull to dump
 * integers which ar e in little endian order.
 */
void cli_hexdump_rev(const void* data, uint16_t length){
	if(!cli_tx)
		return;
	data = (uint8_t*)data + length -1;
	while(length--){
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))>>4)));
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))&0xf)));
		data = (uint8_t*)data -1;
	}
}

/**
 * \brief dumps the contents of a buffer to the console
 * Like cli_hexdump but bytes are seperated with a single space
 * on the console output.
 */
void cli_hexdump2(const void* data, uint16_t length){
	if(!cli_tx)
		return;
	while(length--){
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))>>4)));
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))&0xf)));
		cli_tx(' ');
		data = (uint8_t*)data +1;
	}
}

/**
 * \brief dumps the contents of a buffer to the console
 * Like cli_hexdump but bytes are seperated with a single space
 * on the console output.
 */
void cli_hexdump_block(const void* data, uint16_t length, uint8_t indent, uint8_t width){
	uint16_t i;
	uint8_t  j;
	if(!cli_tx)
		return;
	for(i=0; i<length; ++i){
		if(i%width==0){
			cli_putstr_P(PSTR("\r\n"));
			for(j=0; j<indent; ++j){
				cli_tx(' ');
			}
		}
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))>>4)));
		cli_tx(pgm_read_byte(hexdigit_tab_P +((*((uint8_t*)data))&0xf)));
		cli_tx(' ');
		data = (uint8_t*)data +1;
	}
}

static
void cli_auto_help(uint16_t maxcmdlength, PGM_VOID_P cmdlist){
	cmdlist_entry_t item;
	uint16_t i;
	if(!cli_tx)
		return;
	
	cli_putstr_P(PSTR("\r\n[auto help] available commands:\r\n"
	                  " <command> - <params> - <address>\r\n"));
	for(;;){
		item.cmd_name      = (void*)pgm_read_word(cmdlist+0);
		item.cmd_param_str = (void*)pgm_read_word(cmdlist+2);
		item.cmd_function  = (void_fpt)pgm_read_word(cmdlist+4);
		cmdlist = (uint8_t*)cmdlist+CMDLIST_ENTRY_SIZE;
		if(item.cmd_name==NULL){
			return;
		}
		cli_tx(' ');
		cli_putstr_P(item.cmd_name);
		i=maxcmdlength-strlen_P(item.cmd_name);
		while(i--)
			cli_tx(' ');
		cli_putstr_P(PSTR(" - "));
		if(item.cmd_param_str==NULL){
			cli_putstr_P(PSTR("none \t- 0x"));
		} else {
			if(item.cmd_param_str==(void*)1){
				cli_putstr_P(PSTR("yes  \t- 0x"));
			} else {
				cli_putstr_P(item.cmd_param_str);
				cli_putstr_P(PSTR(" \t- 0x"));
			}
		}
		cli_hexdump_rev(&item.cmd_function, 2);	
		cli_putstr_P(PSTR("\r\n"));
	}
}

void echo_ctrl(char* s){
	s = strstrip(s);
	if(s==NULL || *s=='\0'){
		cli_putstr_P(PSTR("\r\necho is "));
		cli_putstr_P(cli_echo?PSTR("on"):PSTR("off"));
		cli_putstr_P(PSTR("\r\n"));		
	}
	strlwr(s);
	if(!strcmp_P(s, PSTR("true")) || !strcmp_P(s, PSTR("on")) || *s=='1'){
		cli_echo=1;
	}
	if(!strcmp_P(s, PSTR("false")) || !strcmp_P(s, PSTR("off")) || *s=='0'){
		cli_echo=0;
	}
}

typedef void(*str_fpt)(char*);
#define CLI_ENTER     13
#define CLI_BACKSPACE  8
#define CLI_TABULATOR  9

int8_t search_and_call(char* cmd, uint16_t maxcmdlength, PGM_VOID_P cmdlist){
	PGM_VOID_P cmdlist_orig = cmdlist;
	if(*cmd=='\0' || *cmd=='#')
		return 1;
	if(!strcmp_P(cmd, PSTR("exit")))
		return 0;
	if((!strcmp_P(cmd, PSTR("help"))) || (!strcmp_P(cmd, PSTR("?")))){
		cli_auto_help(maxcmdlength, cmdlist);
		return 1;
	}
	uint16_t fwlength=firstword_length(cmd);
	char fw[fwlength+1];
	memcpy(fw, cmd, fwlength);
	fw[fwlength] = '\0';
	cmdlist_entry_t item;
	do{
		item.cmd_name =      (void*)pgm_read_word(cmdlist+0);
		item.cmd_param_str = (void*)pgm_read_word(cmdlist+2);
		item.cmd_function =  (void_fpt)pgm_read_word(cmdlist+4);
		cmdlist = (uint8_t*)cmdlist+CMDLIST_ENTRY_SIZE;
	}while(item.cmd_name!=NULL && strcmp_P(fw, item.cmd_name));
	if(item.cmd_name==NULL){
		cli_auto_help(maxcmdlength, cmdlist_orig);
	} else {
		if(item.cmd_function==NULL)
			return 2;
		switch((uint16_t)item.cmd_param_str){
			case 0:
				item.cmd_function();
				break;
			case 1:
				if(cmd[fwlength]=='\0'){
					((str_fpt)item.cmd_function)(cmd+fwlength);
				} else {
					((str_fpt)item.cmd_function)(cmd+fwlength+1);
				}
				break;
			default:
				cli_putstr_P(PSTR("\r\nparam parsing currently not implemented!\r\n"));
				break;
		}	
		
	}	
	return 1;	 
}

uint16_t max_cmd_length(PGM_VOID_P cmdlist){
	uint16_t t,ret=0;
	char* str;
	for(;;){
		str = (char*)pgm_read_word(cmdlist);
		cmdlist = (uint8_t*)cmdlist + CMDLIST_ENTRY_SIZE;
		if(str==NULL)
			return ret;
		t = strlen_P(str);
		if(t>ret)
			ret=t;
	}
}

uint8_t cli_completion(char* buffer, uint16_t maxcmdlength, PGM_VOID_P cmdlist){
	uint8_t i=0;
	char ref[maxcmdlength+1];
	char* itemstr;
	ref[0]='\0';
	/* check if we are behind the first word */
	while(buffer[i]){
		if(!isgraph(buffer[i++]))
			return 0;
	}
	for(;;){
		itemstr = (char*)pgm_read_word(cmdlist);
		if(itemstr==NULL)
			break;
		cmdlist = (uint8_t*)cmdlist +CMDLIST_ENTRY_SIZE;
		if(!strncmp_P(buffer, itemstr, i)){
			if(!ref[0]){
				strcpy_P(ref, itemstr);
			}else{
				ref[stridentcnt_P(ref, itemstr)]='\0';
			}
		}
	}
	i = strcmp(buffer, ref);
	if(i)
		strcpy(buffer, ref);
	return ~i;
}

void cli_option_listing(char* buffer, PGM_VOID_P cmdlist){
	char* itemstr;
	uint16_t len=strlen(buffer);
	for(;;){
		itemstr = (char*)pgm_read_word(cmdlist);
		if(itemstr==NULL){
			cli_putstr_P(PSTR("\r\n>"));
			cli_putstr(buffer);
			return;
		}
		cmdlist = (uint8_t*)cmdlist +CMDLIST_ENTRY_SIZE;
		if(!strncmp_P(buffer, itemstr, len)){
			cli_putstr_P(PSTR("\r\n    "));
			cli_putstr_P(itemstr);
		}
	}
}

int8_t cmd_interface(PGM_VOID_P cmd_desc){
	uint16_t cli_buffer_size;
	uint16_t cli_buffer_index;
	int8_t exit_code;
	uint8_t completion_failed=0;
	char* cli_buffer;
	char c;
	uint16_t maxcmdlength = max_cmd_length(cmd_desc);
	cli_buffer = calloc(1,cli_buffer_size=maxcmdlength+2);
	cli_buffer_index=0;
	if(!cli_rx)
		return -1;
	if(cli_tx)
		cli_tx('>');
	for(;;){
		c = cli_rx();
		switch (c){
		case CLI_ENTER:
			if((exit_code=search_and_call(cli_buffer, maxcmdlength, cmd_desc))<=0){
				free(cli_buffer);
				return exit_code;
			}
			memset(cli_buffer, 0, cli_buffer_size);
			cli_buffer_index=0;
			cli_putstr_P(PSTR("\r\n>"));
			completion_failed=0;
			break;
		case CLI_BACKSPACE:
			completion_failed=0;
			if(cli_buffer_index==0)
				break;
			cli_buffer_index--;
			cli_buffer[cli_buffer_index] = '\0';
			if(cli_echo && cli_tx){
				cli_tx(c);
			}
			break;
		case CLI_TABULATOR:
			if(completion_failed || cli_buffer_index==0){
				if(cli_tx)
					cli_option_listing(cli_buffer, cmd_desc);
			} else {
				uint16_t old_idx = cli_buffer_index;
				completion_failed = 
					~cli_completion(cli_buffer, maxcmdlength, cmd_desc);
				cli_buffer_index = strlen(cli_buffer);
				if(cli_echo && cli_tx){
					while(old_idx<cli_buffer_index){
						cli_tx(cli_buffer[old_idx++]);
					}
				}
			}
			break;
		default:
			completion_failed=0;
			if(cli_echo && cli_tx){
				cli_tx(c);
			}
			if(cli_buffer_index+1==cli_buffer_size){
				cli_buffer = realloc(cli_buffer, cli_buffer_size+=CLI_BUFFER_BS);
				if(!cli_buffer){
					return -2;
				}
				memset(cli_buffer+cli_buffer_index+1, 0, CLI_BUFFER_BS);
			}
			cli_buffer[cli_buffer_index++] = c;
		}
	}
}
