/* 
 * File:		arcfour-asm.S
 * Author:	Daniel Otte
 * Date: 	07.06.2006
 * License: GPL
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 * 
 */
 
 /* +---+---+---------------------+
 *  | i | j | ......<256>........ |
 *  +---+---+---------------------+
 */
 
.global arcfour_init

;== arcfour_init ==
;  this function initialises the context
; param1: 16-bit pointer to a ctx struct
;	given in r25,r24
; param2: 16-bit pointer to a key
;	given in r23,r22
; param1: 8-bit integer indicating keylength in byte
;	given in 	r20

arcfour_init:
	push r29
	push r28
	push r2
	
	movw r26, r24	/* X points to ctx */
	movw r30, r22	/* Z points to key */
	st X+, r1
	st X+, r1		/* X points to S */
	
1:		
	st X+, r1 
	inc r1
	brne 1b
	
	adiw r24, 2		/* r24:r25 points to S */
	clr r21			/* r21 is j */
	mov r18, r20		/* r18 is keyindex counter */
	clr r0
2:
	movw r26, r24
	ld r19, Z+
	add r21, r19		/* j+= key[i%length] */
	
	add r26, r1
	adc r27, r0
	ld r19, X
	add r21, r19		/* j += S[i] */
	
	dec r18		/* check the key-index counter */
	brne 3f
	movw r30, r22
	mov r18, r20
3:	/* now swap(S[i], S[j]) */ /* r19 is still S[i] */
	movw r28, r24 
	add r28, r21
	adc r29, r0		/* Y points to S[j]*/
	ld r2, Y
	st Y, r19
	st X, r2	
	inc r1
	brne 2b	
	
	pop r2
	pop r28
	pop r29
	ret

/*
uint8_t arcfour_gen(arcfour_ctx_t *c){
	uint8_t t;
	c->i++;
	c->j += c->s[c->i];
	t = c->s[c->j];
	c->s[c->j] = c->s[c->i];
	c->s[c->i] = t;
	return c->s[(c->s[c->j] + c->s[c->i]) & 0xff];
}
*/
.global arcfour_gen

;== arcfour_gen ==
;  this function initialises the context
; param1: 16-bit pointer to a ctx struct
;	given in r25,r24

arcfour_gen:
	movw r26, r24
	ld r18, X
	inc r18
	st X+, r18
	movw r30, r26
	ld r19, X+
	add r26, r18
	adc r27, r1
	ld r20, X
	add r19, r20
	st Z+, r19		/* i,j loaded&saved; X->S[i]; Z->S[0]; r20=S[i] */
	add r30, r19
	adc r31, r1
	ld r21, Z		/* X->S[i]; Z->S[j]; r20=S[i]; r21=S[j]*/
	st Z, r20
	st X, r21
	add r20, r21
	adiw r24, 2
	movw r26, r24 /* X and Z point to S */
	add r26, r20
	adc r27, r1
	ld r24, X
	clr r25
	ret






