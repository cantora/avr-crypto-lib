/* memxor.S */
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
 * File:        memxor.S
 * Author:      Daniel Otte
 * Date:        2008-08-07
 * License:     GPLv3 or later
 * Description: memxor, XORing one block into another
 * 
 */
 
#include <avr/io.h>


.macro push_ p1:req, p2:vararg
	push \p1
.ifnb \p2	
	push_ \p2
.endif
.endm

.macro pop_ p1:req, p2:vararg
	pop \p1
.ifnb \p2	
	pop_ \p2
.endif
.endm

.macro push_range from:req, to:req
	push \from
.if     \to-\from
	push_range "(\from+1)",\to
.endif		
.endm

.macro pop_range from:req, to:req
	pop \to
.if     \to-\from
	pop_range \from,"(\to-1)"	
.endif
.endm

.macro stack_alloc size:req, reg1=r30, reg2=r31
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	sbiw r30, \size 
	out  _SFR_IO_ADDR(SPH), \reg2
	out  _SFR_IO_ADDR(SPL), \reg1
.endm

.macro stack_free size:req, reg1=r30, reg2=r31
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	adiw r30, \size 
	out  _SFR_IO_ADDR(SPH), \reg2
	out  _SFR_IO_ADDR(SPL), \reg1
.endm

/*
 * void memxor(void* dest, const void* src, uint16_t n);
 */
 /*
  * param dest is passed in r24:r25
  * param src  is passed in r22:r23
  * param n    is passed in r20:r21
  */
.global memxor
memxor:
	movw r30, r24
	movw r26, r22
	movw r24, r20
	tst r24
	brne 1f
	tst r25
	breq 2f
1:
	ld r20, X+
	ld r21, Z
	eor r20, r21
	st Z+, r20
	sbiw r24, 1
	brne 1b
2:
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	

