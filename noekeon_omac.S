/* noekeon_omac.S */
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
/*
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2008-08-06
 * \license GPLv3 or later
 * 
 * 
 * 
 */

#include <avr/io.h>
#include "avr-asm-macros.S"


push_r18_r27_func:
	pop r31
	pop r30
	push_range 18, 27
	ijmp

pop_r18_r27_func:
	pop r31
	pop r30
	pop_range 18, 27
	ijmp

.extern noekeon_enc

/*
 * void noekeon_omac(void* dest, const void* msg, uint16_t msglength_b, 
 *                   const void* key, uint8_t t)  
 */

/* param dest        is passed in r24:r25
 * param msg         is passed in r22:r23
 * param msglength_b is passed in r20:r21
 * param key         is passed in r18:r19
 * param t           is passed in r16
 */
.global noekeon_omac
noekeon_omac:
	stack_alloc 48
	adiw r30, 1
	ldi r17, 48
1:
	st Z+, r1
	dec r17
	brne 1b
	sbiw r30, 48
	movw r26, r22
	movw r22, r18
	movw r18, r24
	movw r24, r30
	cpi r16, 0xff
	breq 2f
	st Z, r16
	rcall push_r18_r27_func
	rcall noekeon_enc
	rcall pop_r18_r27_func
	movw r30, r24  
2:	
	tst r21
	brne fullblock
	cpi r20, 128+1
	brlo lastblock
fullblock:	
	/* copy block to stack buffer */
	ldi r16, 16
1:	
	ld r0, X+
	ld r17, Z
	eor r0, r17
	st Z+, r0
	dec r16
	brne 1b
	rcall push_r18_r27_func
	rcall noekeon_enc
	rcall pop_r18_r27_func
	movw r30, r24
	subi r20, 128
	sbci r21, 0
	rjmp 2 
lastblock:
	adiw r24, 16
	rcall push_r18_r27_func
	rcall noekeon_enc
	rcall pop_r18_r27_func

	bst r20, 6 /* set t bit if msglength_b%128==0*/
2:
	ldi r16, 16
	clc
	movw r30, r24 /* z points to encrypted null  vector (L) */	
1:	
	ld r0, Z
	rol r0
	st Z+, r0
	dec r16
	brne 1b
	brtc 2f
	clt
	rjmp 2b
2: /* B/P has been calculated */
	ldi r16, 16
	sbiw r30, 32
3:
	ld r0, Z
	ldd r17, Z+16
	eor r0, r17
	st Z+, r0
	dec r16
	brne 3b
   /* B/P has been xored into stack buffer */
   /* now we have to xor-in the remaining message */
    mov r16, r20
    subi r16, -7
    lsr r16
    lsr r16
    lsr r16  /* r 1*/
    sbiw r30, 16
4: 
    ld r0, X+
    ld r17, Z
    eor r0, r17
    st Z+, r0
    dec r16
    brne 4b
    /* now we have only to insert the 1 at the end of message if msglength_b%128 != 0 */
    sbiw r30, 1
    andi r20, 0x07
    breq 7f
    ldi r17, 1
5:
    dec r20
    breq 6f
    lsl r17
    rjmp 5b  
6:
	ld r0, Z
	eor r0, r17
	st Z, r0
7:
	call noekeon_enc
	stack_free 48
	ret












