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

.global rotate64_1bit_left
rotate64_1bit_left:
	bst r25, 7
	rol r18
	rol r19
	rol r20
	rol r21
	rol r22
	rol r23
	rol r24
	rol r25
	bld r18, 0
	ret

.global rotate64_1bit_right
rotate64_1bit_right:
	bst r18, 0
	ror r25
	ror r24
	ror r23
	ror r22
	ror r21
	ror r20
	ror r19
	ror r18
	bld r25, 7
	ret

.global rotate64_nbit_autodir
rotate64_nbit_autodir:
	lsr r16
	brcc rotate64_nbit_left
.global rotate64_nbit_right
rotate64_nbit_right:
	ldi r30, pm_lo8(rotate64_1bit_right)
	ldi r31, pm_hi8(rotate64_1bit_right)
	rjmp icall_r16_times
.global rotate64_nbit_left
rotate64_nbit_left:
	ldi r30, pm_lo8(rotate64_1bit_left)
	ldi r31, pm_hi8(rotate64_1bit_left)
icall_r16_times:
1:	dec r16
	brmi 2f
	icall
	rjmp 1b
2:
    ret

rotate64_1byte_left:
	mov r0, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r21
	mov r21, r20
	mov r20, r19
	mov r19, r18
	mov r18, r0
	ret

rotate64_2byte_left:
	movw r0, r24
	movw r24, r22
	movw r22, r20
	movw r20, r18
	movw r18, r0
	ret

rotate64_3byte_left:
	mov r0, r25
	mov r25, r22
	mov r22, r19
	mov r19, r24
	mov r24, r21
	mov r21, r18
	mov r18, r23
	mov r23, r20
	mov r20, r0
	ret

rotate64_4byte_left:
	movw r0, r24
	movw r24, r20
	movw r20, r0
	movw r0, r22
	movw r22, r18
	movw r18, r0
	ret

rotate64_5byte_left:
	mov r0, r25
	mov r25, r20
	mov r20, r23
	mov r23, r18
	mov r18, r21
	mov r21, r24
	mov r24, r19
	mov r19, r22
	mov r22, r0
	ret

rotate64_6byte_left:
	movw r0, r18
	movw r18, r20
	movw r20, r22
	movw r22, r24
	movw r24, r0
	ret

rotate64_7byte_left:
	mov r0, r18
	mov r18, r19
	mov r19, r20
	mov r20, r21
	mov r21, r22
	mov r22, r23
	mov r23, r24
	mov r24, r25
	mov r25, r0
	ret


byte_rot_jmp_table:
	ret
	rjmp rotate64_1byte_left
	rjmp rotate64_2byte_left
	rjmp rotate64_3byte_left
	rjmp rotate64_4byte_left
	rjmp rotate64_5byte_left
	rjmp rotate64_6byte_left
	rjmp rotate64_7byte_left

.global rotate64left_code
rotate64left_code:
	ldi r30, pm_lo8(byte_rot_jmp_table)
	ldi r31, pm_hi8(byte_rot_jmp_table)
	mov r0, r16
	andi r16, 0x70
	swap r16
	add r30, r16
	adc r31, r1
	mov r16, r0
	andi r16, 0x0f
	icall
	clr r1
	rjmp rotate64_nbit_autodir


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

;	a[i][j] =  b[i][j] ^ ((~(b[i][(j + 1) % 5])) & (b[i][(j + 2) % 5]));

/*********************************************
 * chi_step
 *********************************************
	input:
		Y = a; uint8t *a;
		X = b; uint8t *b;
		Z = c; uint8t *c;
	output:
		a[0..7] ^= ~b[0..7] & c[0..7]
		X += 8
		Y += 8
		Z += 8
		r16 = 0
		trash r21, r22, r23
*/
chi_step:
	ldi r16, 8
10:
	ld r21, Y
	ld r22, X+
	ld r23, Z+
	com r22
	and r22, r23
	eor r21, r22
	st Y+, r21
	dec r16
	brne 10b
	ret

.global keccak_f1600
keccak_f1600:
	push_range 2, 9
	push r16
	push_range 28, 29

	stack_alloc_large 200, r26, r27
	adiw XL, 1

	clr r9
5:
	movw r30, r24 ; Z = a

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
   -- rho & pi --
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

	sbiw r24, 5 * 8 + 1
	movw YL, r24
	sbiw XL, 8
	movw B_REG_L, XL

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
	rcall rotate64left_code
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
/*
	-- chi --
	for(i = 0; i < 5; ++i){
        a[i][0] ^= ((~(b[i][1])) & (b[i][2]));
        a[i][1] ^= ((~(b[i][2])) & (b[i][3]));
        a[i][2] ^= ((~(b[i][3])) & (b[i][4]));
        a[i][3] ^= ((~(b[i][4])) & (b[i][0]));
        a[i][4] ^= ((~(b[i][0])) & (b[i][1]));

	}
*/
	; memcpy(a, b, 200)
	; X points at b + 32 + 8 = b + 40 = b[1][0] has to point to b[0][0]
	ldi r16, 200
	sbiw XL, 5 * 8
	movw ZL, XL
	subi YL, lo8(5 * 5 * 8)
	sbci YH, hi8(5 * 5 * 8)
	movw r2, YL
10:
	ld r22, X+
	st Y+, r22
	dec r16
	brne 10b

	; Z points at b
	movw XL, ZL
	movw r4, ZL
	adiw XL, 8
	adiw ZL, 16
	movw YL, r2
	ldi r18, 5
10:
	rcall chi_step
	rcall chi_step
	rcall chi_step
	sbiw ZL, 5 * 8
	rcall chi_step
	sbiw XL, 5 * 8
	rcall chi_step
	adiw XL, 5 * 8
	adiw ZL, 5 * 8
	dec r18
	brne 10b

	/* -- iota -- */
	ldi r30, lo8(keccak_rc_comp)
	ldi r31, hi8(keccak_rc_comp)
	add r30, r9
	adc r31, __zero_reg__
	lpm r20, Z+
	movw YL, r2
	ldi r21, 0x80
	bst r20, 6
	brtc 10f
	ldd r22, Y+7
	eor r22, r21
	std Y+7, r22
10:
	bst r20, 5
	brtc 10f
	ldd r22, Y+3
	eor r22, r21
	std Y+3, r22
10:
	bst r20, 4
	brtc 10f
	ldd r22, Y+1
	eor r22, r21
	std Y+1, r22
10:
	andi r20, 0x8f
	ld r22, Y
	eor r22, r20
	st Y, r22

	inc r9
	mov r16, r9
	cpi r16, 24
	breq 20f
	movw r24, YL
	movw r26, r4
	rjmp 5b
20:

	stack_free_large3 200

	pop_range 28, 29
	pop r16
	pop_range 2, 9

	ret
