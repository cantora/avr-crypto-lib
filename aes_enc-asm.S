/* aes_enc-asm.S */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     aes_enc-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-10
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"


/*
 * param a: r24
 * param b: r22
 * param reducer: r0
 */
A = 28
B = 29
P = 0
xREDUCER = 25

gf256mul:
	clr P
1:	
	lsr A
	breq 4f
	brcc 2f
	eor P, B
2:
	lsl B
	brcc 3f
	eor B, xREDUCER	
3:
	rjmp 1b
4:
	brcc 2f
	eor P, B
2:
	ret

.global aes256_enc
aes256_enc:
	ldi r20, 14
	rjmp aes_encrypt_core

.global aes192_enc
aes192_enc:
	ldi r20, 12
	rjmp aes_encrypt_core

.global aes128_enc
aes128_enc:
	ldi r20, 10


/*
  void aes_encrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds)
*/
T0= 2
T1= 3
T2= 4
T3= 5
SBOX_SAVE0 = 6
SBOX_SAVE1 = 7
ST00 =  8
ST01 =  9
ST02 = 10
ST03 = 11
ST10 = 12
ST11 = 13
ST12 = 14
ST13 = 15
ST20 = 16
ST21 = 17
ST22 = 18
ST23 = 19
ST30 = 20
ST31 = 21
ST32 = 22
ST33 = 23
CTR = 24
/*
 * param state:  r24:r25
 * param ks:     r22:r23
 * param rounds: r20   
 */
.global aes_encrypt_core
aes_encrypt_core:
	push_range 2, 17
	push r28
	push r29
	push r24
	push r25
	movw r26, r22
	movw r30, r24
	mov  CTR, r20
	clt
	
	.irp param,ST00, ST01, ST02, ST03, ST10, ST11, ST12, ST13, ST20, ST21, ST22, ST23, ST30, ST31, ST32, ST33
		ld \param, Z+
	.endr
	/* key whitening */
1:
	.irp param,ST00, ST01, ST02, ST03, ST10, ST11, ST12, ST13, ST20, ST21, ST22, ST23, ST30, ST31, ST32, ST33
		ld r0, X+
		eor \param, r0
	.endr
	
	brtc 2f
	rjmp exit
2:	dec CTR
	brne 3f
	set
3:

	ldi r30, lo8(aes_sbox)
	ldi r31, hi8(aes_sbox)
	movw SBOX_SAVE0, r30
	/* encryption loop */ 

	/* SBOX substitution and shifting */
	movw r30, SBOX_SAVE0
	add r30, ST00
	adc r31, r1
	lpm ST00, Z
	movw r30, SBOX_SAVE0
	add r30, ST10
	adc r31, r1
	lpm ST10, Z
	movw r30, SBOX_SAVE0
	add r30, ST20
	adc r31, r1
	lpm ST20, Z
	movw r30, SBOX_SAVE0
	add r30, ST30
	adc r31, r1
	lpm ST30, Z

	movw r30, SBOX_SAVE0
	add r30, ST01
	adc r31, r1
	lpm T0, Z
	movw r30, SBOX_SAVE0
	add r30, ST11
	adc r31, r1
	lpm ST01, Z
	movw r30, SBOX_SAVE0
	add r30, ST21
	adc r31, r1
	lpm ST11, Z
	movw r30, SBOX_SAVE0
	add r30, ST31
	adc r31, r1
	lpm ST21, Z
	mov ST31, T0

	movw r30, SBOX_SAVE0
	add r30, ST02
	adc r31, r1
	lpm T0, Z
	movw r30, SBOX_SAVE0
	add r30, ST12
	adc r31, r1
	lpm T1, Z
	movw r30, SBOX_SAVE0
	add r30, ST22
	adc r31, r1
	lpm ST02, Z
	movw r30, SBOX_SAVE0
	add r30, ST32
	adc r31, r1
	lpm ST12, Z
	mov ST22, T0
	mov ST32, T1

	movw r30, SBOX_SAVE0
	add r30, ST03
	adc r31, r1
	lpm T0, Z
	movw r30, SBOX_SAVE0
	add r30, ST13
	adc r31, r1
	lpm T1, Z
	movw r30, SBOX_SAVE0
	add r30, ST23
	adc r31, r1
	lpm T2, Z
	movw r30, SBOX_SAVE0
	add r30, ST33
	adc r31, r1
	lpm ST03, Z
	mov ST13, T0
	mov ST23, T1
	mov ST33, T2
	
	/* mixcols (or rows in our case) */
	brtc 2f
	rjmp 1b
2:	
	ldi xREDUCER, 0x1b /* load reducer */
	
	ldi A, 2
	mov B, ST00
	rcall gf256mul
	mov T0, r0
	ldi A, 3
	mov B, ST01
	rcall gf256mul
	eor T0, r0
	eor T0, ST02
	eor T0, ST03
	
	mov T1, ST00
	ldi A, 2
	mov B, ST01
	rcall gf256mul
	eor T1, r0
	ldi A, 3
	mov B, ST02
	rcall gf256mul
	eor T1, r0
	eor T1, ST03
	
	mov T2, ST00
	eor T2, ST01
	ldi A, 2
	mov B, ST02
	rcall gf256mul
	eor T2, r0
	ldi A, 3
	mov B, ST03
	rcall gf256mul
	eor T2, r0
	
	ldi A, 3
	mov B, ST00
	rcall gf256mul
	mov T3, r0
	eor T3, ST01
	eor T3, ST02
	ldi A, 2
	mov B, ST03
	rcall gf256mul
	eor T3, r0
	
	mov ST00, T0
	mov ST01, T1
	mov ST02, T2
	mov ST03, T3
			
	ldi A, 2
	mov B, ST10
	rcall gf256mul
	mov T0, r0
	ldi A, 3
	mov B, ST11
	rcall gf256mul
	eor T0, r0
	eor T0, ST12
	eor T0, ST13
	
	mov T1, ST10
	ldi A, 2
	mov B, ST11
	rcall gf256mul
	eor T1, r0
	ldi A, 3
	mov B, ST12
	rcall gf256mul
	eor T1, r0
	eor T1, ST13
	
	mov T2, ST10
	eor T2, ST11
	ldi A, 2
	mov B, ST12
	rcall gf256mul
	eor T2, r0
	ldi A, 3
	mov B, ST13
	rcall gf256mul
	eor T2, r0
	
	ldi A, 3
	mov B, ST10
	rcall gf256mul
	mov T3, r0
	eor T3, ST11
	eor T3, ST12
	ldi A, 2
	mov B, ST13
	rcall gf256mul
	eor T3, r0
	
	mov ST10, T0
	mov ST11, T1
	mov ST12, T2
	mov ST13, T3

	ldi A, 2
	mov B, ST20
	rcall gf256mul
	mov T0, r0
	ldi A, 3
	mov B, ST21
	rcall gf256mul
	eor T0, r0
	eor T0, ST22
	eor T0, ST23
	
	mov T1, ST20
	ldi A, 2
	mov B, ST21
	rcall gf256mul
	eor T1, r0
	ldi A, 3
	mov B, ST22
	rcall gf256mul
	eor T1, r0
	eor T1, ST23
	
	mov T2, ST20
	eor T2, ST21
	ldi A, 2
	mov B, ST22
	rcall gf256mul
	eor T2, r0
	ldi A, 3
	mov B, ST23
	rcall gf256mul
	eor T2, r0
	
	ldi A, 3
	mov B, ST20
	rcall gf256mul
	mov T3, r0
	eor T3, ST21
	eor T3, ST22
	ldi A, 2
	mov B, ST23
	rcall gf256mul
	eor T3, r0
	
	mov ST20, T0
	mov ST21, T1
	mov ST22, T2
	mov ST23, T3

	ldi A, 2
	mov B, ST30
	rcall gf256mul
	mov T0, r0
	ldi A, 3
	mov B, ST31
	rcall gf256mul
	eor T0, r0
	eor T0, ST32
	eor T0, ST33
	
	mov T1, ST30
	ldi A, 2
	mov B, ST31
	rcall gf256mul
	eor T1, r0
	ldi A, 3
	mov B, ST32
	rcall gf256mul
	eor T1, r0
	eor T1, ST33
	
	mov T2, ST30
	eor T2, ST31
	ldi A, 2
	mov B, ST32
	rcall gf256mul
	eor T2, r0
	ldi A, 3
	mov B, ST33
	rcall gf256mul
	eor T2, r0
	
	ldi A, 3
	mov B, ST30
	rcall gf256mul
	mov T3, r0
	eor T3, ST31
	eor T3, ST32
	ldi A, 2
	mov B, ST33
	rcall gf256mul
	eor T3, r0
	
	mov ST30, T0
	mov ST31, T1
	mov ST32, T2
	mov ST33, T3

	/* mix colums (rows) done */

	/* add key*/
	rjmp 1b
	
exit:	
	pop r31
	pop r30
	st Z+, ST00
	st Z+, ST01
	st Z+, ST02
	st Z+, ST03
	st Z+, ST10
	st Z+, ST11
	st Z+, ST12
	st Z+, ST13
	st Z+, ST20
	st Z+, ST21
	st Z+, ST22
	st Z+, ST23
	st Z+, ST30
	st Z+, ST31
	st Z+, ST32
	st Z+, ST33
	
	pop r29
	pop r28
	pop_range 2, 17
	ret







