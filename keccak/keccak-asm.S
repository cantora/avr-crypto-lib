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

.equ __zero_reg__, 1
/*
	void keccak_theta (uint64_t *a, uint64_t *b){
	// uint64_t b[5][5];
		for(i = 0; i < 5; ++i){
			b[i][0] = a[0][i] ^ a[1][i] ^ a[2][i] ^ a[3][i] ^ a[4][i];
	  	}
  	}
*/

/*********************************************
 * theta_2a
 *********************************************
	input:
		r24:r25 = a ; uint64_t a[5][5]
		X = b       ; uint64_t *b
	output:
		a[0..4][0] ^= b
		r20 = 0
		r21 = XX
		r22 = XX
		r24:r25 += 8
		X += 8
		Z = r24:r25 + 7 + 4 * 40
*/
theta_2a:
	ldi r20, 8
10:
	movw ZL, r24
	ld  r21, X+
	.irp r, 0, 1, 2, 3, 4
		ld  r22, Z
		eor r22, r21
		st  Z, r22
	.if \r != 4
		adiw ZL, 40
	.endif
	.endr
	adiw r24, 1
	dec r20
	brne 10b
	ret

/*********************************************
 * theta_2b
 *********************************************
	input:
		r24:r25 = a+1 ; uint64_t a[5][5]
		X = b       ; uint64_t *b
	output:
		a[0..4][0] ^= rol(b,1)
		r19 = XX
		r20 = 0
		r21 = XX
		r22 = XX
		r24:r25 += 8
		X += 8
		Z = r24:r25 + 7 + 4 * 40
*/
theta_2b:
	ldi r20, 7
	ld r19, X+
	lsl r19
	rol __zero_reg__
10:
	movw ZL, r24
	ld  r21, X+
	ror __zero_reg__
	rol r21
	rol __zero_reg__
	.irp r, 0, 1, 2, 3, 4
		ld  r22, Z
		eor r22, r21
		st  Z, r22
	.if \r != 4
		adiw ZL, 40
	.endif
	.endr
	adiw r24, 1
	dec r20
	brne 10b
	add r19, __zero_reg__
	sbiw r24, 8
	movw ZL, r24
	.irp r, 0, 1, 2, 3, 4
		ld  r22, Z
		eor r22, r19
		st  Z, r22
	.if \r != 4
		adiw ZL, 40
	.endif
	.endr
	adiw r24, 9
	clr __zero_reg__
	ret


.global keccak_theta
keccak_theta:
	movw r30, r24 ; Z = a
	movw r26, r22 ; X = b
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
/*
	for(i = 0; i < 5; ++i){
		for(j = 0; j < 5; ++j){
			a[j][i] ^= b[(4 + i) % 5][0];
		}
	}

*/
/* a[0..4][0]{0..7} ^= b[4][0]{0..7} */
	sbiw XL, 5 * 8
	sbiw r24, 40
	rcall theta_2a
/* a[0..4][1]{0..7} ^= b[0][0]{0..7} */
	subi XL, lo8(4 * 5 * 8 + 8)
	sbci XH, hi8(4 * 5 * 8 + 8)
	rcall theta_2a
/* a[0..4][2]{0..7} ^= b[1][0]{0..7} */
	adiw XL, 4 * 8
	rcall theta_2a
/* a[0..4][3]{0..7} ^= b[2][0]{0..7} */
	adiw XL, 4 * 8
	rcall theta_2a
/* a[0..4][4]{0..7} ^= b[3][0]{0..7} */
	adiw XL, 4 * 8
	rcall theta_2a
/*
	for(i = 0; i < 5; ++i){
        for(j = 0; j < 5; ++j){
            a[j][i] ^= rotate64_1bit_left(b[(i + 1) % 5][0]);
        }
    }
*/
/* a[0..4][0]{0..7} ^= rol(b[1][0]{0..7}) */
	subi r24, lo8(5 * 8 - 1)
	sbci r25, hi8(5 * 8 - 1)
	subi XL, lo8(2 * 5 * 8 + 8)
	sbci XH, hi8(2 * 5 * 8 + 8)
	rcall theta_2b
/* a[0..4][1]{0..7} ^= rol(b[2][0]{0..7}) */
	adiw XL, 4 * 8
	rcall theta_2b
/* a[0..4][21]{0..7} ^= rol(b[3][0]{0..7}) */
	adiw XL, 4 * 8
	rcall theta_2b
/* a[0..4][3]{0..7} ^= rol(b[4][0]{0..7}) */
	adiw XL, 4 * 8
	rcall theta_2b
/* a[0..4][4]{0..7} ^= rol(b[0][0]{0..7}) */
	subi XL, lo8(4 * 5 * 8 + 8)
	sbci XH, hi8(4 * 5 * 8 + 8)
	rcall theta_2b


	ret
