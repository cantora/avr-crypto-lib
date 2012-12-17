/* keccac-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2012  Daniel Otte (daniel.otte@rub.de)

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
 * \file     keccak-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2012-12-16
 * \license  GPLv3 or later
 *
 */

.nolist
#include "avr-asm-macros.S"
.list
/*
	void keccak_theta (uint64_t *a, uint64_t *b){
	// uint64_t b[5][5];
		for(i = 0; i < 5; ++i){
			b[i][0] = a[0][i] ^ a[1][i] ^ a[2][i] ^ a[3][i] ^ a[4][i];
	  	}
  	}
*/

.global keccak_theta
keccak_theta:
	movw r30, r24
	movw r26, r22

;	.irp offset, 0, 1, 2, 3, 4

	ldi r19, 5
10:
	ldi r20, 8
20:

	ld  r22, Z
	adiw ZL, 40
	ld  r21, Z
	eor r22, r21
	adiw ZL, 40
	ld  r21, Z
	eor r22, r21
	adiw ZL, 40
	ld  r21, Z
	eor r22, r21
	adiw ZL, 40
	ld  r21, Z
	eor r22, r21

	adiw r24, 1
	movw r30, r24

	st X+, r22
	dec r20
	brne 20b

	adiw XL, 8 * 4
	dec r19
	brne 10b
;	.endr

	ret
