/* xtea-asm.S 
 * Author:	Daniel Otte
 * Date:		06.06.2006
 * License: GPL
 *  Implementation of XTEA for AVR
 *  include xtea.h in your C-Project to use this functions.
*/

V01 = 2
V02 = 3
V03 = 4
V04 = 5
V11 = 6
V12 = 7
V13 = 8
V14 = 9
Accu1 = 14
Accu2 = 15
Accu3 = 16
Accu4 = 17
Sum1 = 18
Sum2 = 19
Sum3 = 20
Sum4 = 21
Func1 = 22
Func2 = 23
Func3 = 24
Func4 = 25
C = 28 /* der kleine Zaehler fuer zwischendurch */

.global xtea_enc
; == xtea_enc ==
; xtea encrytion function
; param1: 16-bit pointer to destination for encrypted block 
;  given in r25,r24
; param2: 16-bit pointer to the block (64-bit) which is to encrypt 
;  given in r23,r22
; param3: 16-bit pointer to the key (128-bit) 
;  given in r21,r20
;
xtea_enc:
 /* prolog */
 	push r2
 	push r3
 	push r4
 	push r5
 	push r6
 	push r7
 	push r8
 	push r9
 	push r14
 	push r15
 	push r16
 	push r17
 	push r28
 	
 /* load the block */
 	movw r26, r22 /* X points to block */
 	movw r30, r20 /* Z points to key   */
 	ld V01, X+
 	ld V02, X+
 	ld V03, X+
 	ld V04, X+
 	ld V11, X+
 	ld V12, X+
 	ld V13, X+
 	ld V14, X+
; 	push r25
; 	push r24
 	movw r26, r24 /* X points to destination */
 
	ldi Func1, 32
	mov r0, Func1 /* r1 is cycle-counter */
	clr Sum1
	clr Sum2
	movw Sum3, Sum1
	clt

1:
	movw Accu1, V11
	movw Accu3, V13
	ldi C, 4
2:	lsl Accu1
	rol Accu2
	rol Accu3
	rol Accu4
	dec C
	brne 2b			/* Accu == V1 << 4 */

	movw Func1, V11
	movw Func3, V13
	ldi C, 5
3:	lsr Func4
	ror Func3
	ror Func2
	ror Func1
	dec C
	brne 3b			/* Func == V1 >> 5 */
	
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4
	add Accu1, V11
	adc Accu2, V12
	adc Accu3, V13
	adc Accu4, V14	/* Accu == ( (V1<<4)^(V1>>5) ) + V1 */
	
	brtc 4f
	mov C, Sum2
	lsr C
	andi C,(0x03 <<2)
	clt
	rjmp 5f
4:	
	mov C, Sum1	/* calc key offset */
	andi C, 0x03
	lsl C
	lsl C
	set
	
5:	
	add r30, C
	adc r31, r1
	ld  Func1, Z
	ldd Func2, Z+1
	ldd Func3, Z+2
	ldd Func4, Z+3 /* Func = key[sum & 3] */
	sub r30, C
	sbci r31, 0
	add Func1, Sum1
	adc Func2, Sum2
	adc Func3, Sum3
	adc Func4, Sum4 
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4 /* Accu = ((V1<<4 ^ V1>>5) + V1) ^ (sum + key[sum&3])  */
	add Accu1, V01
	adc Accu2, V02
	adc Accu3, V03
	adc Accu4, V04
	
	movw V01, V11
	movw V03, V13
	movw V11, Accu1
	movw V13, Accu3
	
	/* sum += delta */ /* delta == 0x9E3779B9 */
	brtc 6f
	ldi C, 0xB9
	add Sum1, C
	ldi C, 0x79
	adc Sum2, C
	ldi C, 0x37
	adc Sum3, C
	ldi C, 0x9E
	adc Sum4, C
	rjmp 1b
	
6:	
	dec r0
	breq 7f
	rjmp 1b 
 
 7:
 /* write block back */
 ;	pop r26
 ;	pop r27
 	st X+, V01
 	st X+, V02
 	st X+, V03
  	st X+, V04
 	st X+, V11
 	st X+, V12
 	st X+, V13
 	st X+, V14
 
 /* epilog */
 	pop r28
 	pop r17
 	pop r16
 	pop r15
 	pop r14
 	pop r9
 	pop r8
 	pop r7
 	pop r6
 	pop r5
 	pop r4
 	pop r3
 	pop r2
 	ret

;####################################################################
 
 /* #endif TWO_IN_ONE */	
 
 /* #ifdef TWO_IN_ONE */
 /* now we use the same base-structure for enc- and decryption
 	to indicate operation mode we use the highest bit of param3 (16 bit pointer to key),
 	this is ok, since even the larges atmel today has "only" 8k of ram,
 	but you shouldn't use this feature while using external ram. 
 */
.global xtea_enc
 	ori r21, 0x80
 	
.global xtea_dec
; == xtea_dec ==
; xtea decrytion function
; param1: 16-bit pointer to destination for decrypted block 
;  given in r25,r24
; param2: 16-bit pointer to the block (64-bit) which is to derypt 
;  given in r23,r22
; param3: 16-bit pointer to the key (128-bit) 
;  given in r21,r20
;
/*
void xtea_dec(uint32_t* dest, uint32_t* v, uint32_t* k) {
    uint32_t v0=v[0], v1=v[1], i;
    uint32_t sum=0xC6EF3720, delta=0x9E3779B9;
    for(i=0; i<32; i++) {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
        sum -= delta;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
    }
    dest[0]=v0; dest[1]=v1;
}
*/

xtea_dec:
 /* prolog */
 	push r2
 	push r3
 	push r4
 	push r5
 	push r6
 	push r7
 	push r8
 	push r9
 	push r14
 	push r15
 	push r16
 	push r17
 	push r28 
 /* load the block */
 	movw r26, r22 /* Z points to block */
 	movw r30, r20 /* X points to key   */
 	ld V01, X+
 	ld V02, X+
 	ld V03, X+
 	ld V04, X+
 	ld V11, X+
 	ld V12, X+
 	ld V13, X+
 	ld V14, X+
 	movw r26, r24 /* Z points to destination */
 
	ldi Sum1, 32
	mov r0, Sum1 /* r1 is cycle-counter */
	ldi Sum1, 0x20 /* sum = 0xC6EF3720 */
	ldi Sum2, 0x37
	ldi Sum3, 0xEF
	ldi Sum4, 0xC6
	clt

1:
	movw Accu1, V01
	movw Accu3, V03
	ldi C, 4
2:	lsl Accu1
	rol Accu2
	rol Accu3
	rol Accu4
	dec C
	brne 2b			/* Accu == V0 << 4 */

	movw Func1, V01
	movw Func3, V03
	ldi C, 5
3:	lsr Func4
	ror Func3
	ror Func2
	ror Func1
	dec C
	brne 3b			/* Func == V0 >> 5 */
	
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4
	add Accu1, V01
	adc Accu2, V02
	adc Accu3, V03
	adc Accu4, V04	/* Accu == ( (V0<<4)^(V0>>5) ) + V0 */
	
	brts 4f
	mov C, Sum2
	lsr C
	andi C,(0x03 <<2)
	set
	rjmp 5f
4:	
	mov C, Sum1	/* calc key offset */
	andi C, 0x03
	lsl C
	lsl C
	clt
	
5:	
	add r30, C
	adc r31, r1
	ld  Func1, Z
	ldd Func2, Z+1
	ldd Func3, Z+2
	ldd Func4, Z+3 /* Func = key[sum & 3] */
	sub r30, C
	sbci r31, 0
	add Func1, Sum1
	adc Func2, Sum2
	adc Func3, Sum3
	adc Func4, Sum4 
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4 /* Accu = ((V0<<4 ^ V0>>5) + V0) ^ (sum + key[sum&3])  */
	sub V11, Accu1
	sbc V12, Accu2
	sbc V13, Accu3
	sbc V14, Accu4
	
	movw Accu1, V01
	movw Accu3, V03
	movw V01, V11
	movw V03, V13
	movw V11, Accu1
	movw V13, Accu3
	
	/* sum += delta */ /* delta == 0x9E3779B9 */
	brtc 6f
	subi Sum1, 0xB9
	sbci Sum2, 0x79
	sbci Sum3, 0x37
	sbci Sum4, 0x9E
	rjmp 1b
	
6:	
	dec r0
	breq 7f
	rjmp 1b 
 
7:
 /* write block back */
 	st X+, V01
 	st X+, V02
 	st X+, V03
  	st X+, V04
 	st X+, V11
 	st X+, V12
 	st X+, V13
 	st X+, V14
 
 /* epilog */
 	pop r28
 	pop r17
 	pop r16
 	pop r15
 	pop r14
 	pop r9
 	pop r8
 	pop r7
 	pop r6
 	pop r5
 	pop r4
 	pop r3
 	pop r2
 	ret
 	
 /* #endif */

;####################################################################
 
 #ifdef TWO_IN_ONE
 /* now we use the same base-structure for enc- and decryption
 	to indicate operation mode we use the highest bit of param3 (16 bit pointer to key),
 	this is ok, since even the larges atmel today has "only" 8k of ram,
 	but you shouldn't use this feature while using external ram. 
 */
.global xtea_enc
 	ori r21, 0x80
 	
.global xtea_dec
; == xtea_dec ==
; xtea decrytion function
; param1: 16-bit pointer to destination for decrypted block 
;  given in r25,r24
; param2: 16-bit pointer to the block (64-bit) which is to derypt 
;  given in r23,r22
; param3: 16-bit pointer to the key (128-bit) 
;  given in r21,r20
;
/*
void xtea_dec(uint32_t* dest, uint32_t* v, uint32_t* k) {
    uint32_t v0=v[0], v1=v[1], i;
    uint32_t sum=0xC6EF3720, delta=0x9E3779B9;
    for(i=0; i<32; i++) {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
        sum -= delta;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
    }
    dest[0]=v0; dest[1]=v1;
}
*/

xtea_dec:
 /* prolog */
 	push r2
 	push r3
 	push r4
 	push r5
 	push r6
 	push r7
 	push r8
 	push r9
 	push r14
 	push r15
 	push r16
 	push r17
 	push r28 
 /* set T-bit if we are going to encrypt, clear otherwise */
 	bst r21, 7
 	andi r21, 0x7f /* fix r21:r22 to a real addr */
 /* load the block */
 	movw r26, r22 /* Z points to block */
 	movw r30, r20 /* X points to key   */
 	ld V01, X+
 	ld V02, X+
 	ld V03, X+
 	ld V04, X+
 	ld V11, X+
 	ld V12, X+
 	ld V13, X+
 	ld V14, X+
 	movw r26, r24 /* Z points to destination */
 
	ldi Sum1, 32
	mov r0, Sum1 /* r1 is cycle-counter */
	ldi Sum1, 0x20 /* sum = 0xC6EF3720 */
	ldi Sum2, 0x37
	ldi Sum3, 0xEF
	ldi Sum4, 0xC6
	clt

1:
	movw Accu1, V01
	movw Accu3, V03
	ldi C, 4
2:	lsl Accu1
	rol Accu2
	rol Accu3
	rol Accu4
	dec C
	brne 2b			/* Accu == V0 << 4 */

	movw Func1, V01
	movw Func3, V03
	ldi C, 5
3:	lsr Func4
	ror Func3
	ror Func2
	ror Func1
	dec C
	brne 3b			/* Func == V0 >> 5 */
	
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4
	add Accu1, V01
	adc Accu2, V02
	adc Accu3, V03
	adc Accu4, V04	/* Accu == ( (V0<<4)^(V0>>5) ) + V0 */
	
	brts 4f
	mov C, Sum2
	lsr C
	andi C,(0x03 <<2)
	set
	rjmp 5f
4:	
	mov C, Sum1	/* calc key offset */
	andi C, 0x03
	lsl C
	lsl C
	clt
	
5:	
	add r30, C
	adc r31, r1
	ld  Func1, Z
	ldd Func2, Z+1
	ldd Func3, Z+2
	ldd Func4, Z+3 /* Func = key[sum & 3] */
	sub r30, C
	sbci r31, 0
	add Func1, Sum1
	adc Func2, Sum2
	adc Func3, Sum3
	adc Func4, Sum4 
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4 /* Accu = ((V0<<4 ^ V0>>5) + V0) ^ (sum + key[sum&3])  */
	sub V11, Accu1
	sbc V12, Accu2
	sbc V13, Accu3
	sbc V14, Accu4
	
	movw Accu1, V01
	movw Accu3, V03
	movw V01, V11
	movw V03, V13
	movw V11, Accu1
	movw V13, Accu3
	
	/* sum += delta */ /* delta == 0x9E3779B9 */
	brtc 6f
	subi Sum1, 0xB9
	sbci Sum2, 0x79
	sbci Sum3, 0x37
	sbci Sum4, 0x9E
	rjmp 1b
	
6:	
	dec r0
	breq 7f
	rjmp 1b 
 
7:
 /* write block back */
 	st X+, V01
 	st X+, V02
 	st X+, V03
  	st X+, V04
 	st X+, V11
 	st X+, V12
 	st X+, V13
 	st X+, V14
 
 /* epilog */
 	pop r28
 	pop r17
 	pop r16
 	pop r15
 	pop r14
 	pop r9
 	pop r8
 	pop r7
 	pop r6
 	pop r5
 	pop r4
 	pop r3
 	pop r2
 	ret
 	
 #endif

