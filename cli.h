/* cli.h */
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
#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>
#include <avr/pgmspace.h>

typedef void(*void_fpt)(void);

int16_t findstring_d0(const char* str, const char* v);
int16_t findstring_d0_P(const char* str, PGM_P v);

int16_t execcommand_d0_P(const char* str, PGM_P v, void(*fpt[])(void) );
#endif /*CLI_H_*/
