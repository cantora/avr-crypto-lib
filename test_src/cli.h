/* cli.h */
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
#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>
#include <avr/pgmspace.h>

typedef void(*void_fpt)(void);

#ifdef CLI_OLD

int16_t findstring_d0(const char* str, const char* v);
int16_t findstring_d0_P(const char* str, PGM_P v);

int16_t execcommand_d0_P(const char* str, PGM_P v, void(*fpt[])(void) );

#else

typedef char (*cli_rx_fpt)(void);
typedef void (*cli_tx_fpt)(char);

#define CLI_BUFFER_BS 20

typedef struct {
	PGM_P      cmd_name;      /* string containing the function name */
	PGM_P      cmd_param_str; /* param descriptor string */
	void_fpt   cmd_function;  /* function pointer */
} cmdlist_entry_t;

extern cli_rx_fpt cli_rx;
extern cli_tx_fpt cli_tx;
extern uint8_t cli_echo;

void echo_ctrl(char* s);
int8_t cmd_interface(PGM_VOID_P cmd_desc);

#endif



#endif /*CLI_H_*/
