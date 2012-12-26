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

.global rho_pi_idx_table
rho_pi_idx_table:
	.irp i, 0, 1, 2, 3, 4
		.irp j, 0, 1, 2, 3, 4
			.byte (((2 * \j + 3 * \i) % 5) * 5 + \i) * 8
		.endr
	.endr

.align 2
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
	push_range 2, 8
	push r16
	push_range 28, 29

	movw r30, r24 ; Z = a
	movw r26, r22 ; X = b
	push_range 22, 25

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

;	ret
/*
  rho & pi
	for(i = 0; i < 5; ++i){
		for(j = 0; j < 5; ++j){
			b[(2 * i + 3 * j) % 5][j] =
              rotate64left_code(a[j][i], pgm_read_byte(&(keccak_rotate_codes[i][j])));
		}
	}

   -- or --

	const uint8_t* rot_code = (const uint8_t*)keccak_rotate_codes;
    const uint8_t* idx_idx = (const uint8_t*)rho_pi_idx_table;
    uint64_t *a_tmp = (uint64_t*)a;
	for(i = 0; i < 25; ++i){
		    *((uint64_t*)(((uint8_t*)b) + pgm_read_byte(idx_idx++))) =
                rotate64left_code(*a_tmp++, pgm_read_byte(rot_code++));

	}

*/

.equ B_REG_L, 6
.equ B_REG_H, 7

	ldi r18, lo8(keccak_rotate_codes)
	ldi r19, hi8(keccak_rotate_codes)
	movw r2, r18
	ldi r18, lo8(rho_pi_idx_table)
	ldi r19, hi8(rho_pi_idx_table)
	movw r4, r18
	ldi r16, 25
	mov r8, r16

	pop YH
	pop YL
	pop B_REG_H
	pop B_REG_L

10:
	ld r18, Y+
	ld r19, Y+
	ld r20, Y+
	ld r21, Y+
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	movw ZL, r2
	lpm r16, Z+
	movw r2, ZL
	call rotate64left_code
	movw ZL, r4
	lpm r16, Z+
	movw r4, ZL
	movw XL, B_REG_L
	add XL, r16
	adc XH, __zero_reg__
	st X+, r18
	st X+, r19
	st X+, r20
	st X+, r21
	st X+, r22
	st X+, r23
	st X+, r24
	st X+, r25

	dec r8
	brne 10b

	pop_range 28, 29
	pop r16
	pop_range 2, 8

	ret
