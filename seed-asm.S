/**
 * \file	seed-asm.S
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief	SEED parts in assembler for AVR
 * \par License	
 * GPL
 * 
 */
SPL = 0x3D
SPH = 0x3E
SREG = 0x3F


/*******************************************************************************

	void changeendian32(uint32_t * a){
		*a = (*a & 0x000000FF) << 24 |
			 (*a & 0x0000FF00) <<  8 |
			 (*a & 0x00FF0000) >>  8 |
			 (*a & 0xFF000000) >> 24;
	}

*/
/*
.global changeendian32
; === change_endian32 ===
; function that changes the endianess of a 32-bit word
;  param1: the 32-bit word
;	given in r25,r24,r23,22 (r25 is most significant)
;  modifys: r21, r22
changeendian32:
	movw r20,  r22 ; (r22,r23) --> (r20,r21)
	mov r22, r25
	mov r23, r24
	mov r24, r21
	mov r25, r20 
	ret

*/

/*******************************************************************************
	uint32_t bigendian_sum32(uint32_t a, uint32_t b){
		changeendian32(&a);
		changeendian32(&b);
		a += b;
		changeendian32(&a);
		return a;
	}
*/

.global bigendian_sum32
; === bigendian_sum32 ===
; function that adds two 32-bit words in the bigendian way and returns the result
;  param1: the first 32-bit word
;	given in r25,r24,r23,22 (r25 is most significant for little endian)
;  param2: the second 32-bit word
;	given in r21,r20,r19,18 (r21 is most significant for little endian)
;  modifys: 
bigendian_sum32:
	add r25, r21
	adc r24, r20
	adc r23, r19
	adc r22, r18
	ret
	
.global bigendian_sub32
; === bigendian_sub32 ===
; function that subtracts a 32-bit words from another in the bigendian way and returns the result
;  param1: the minuend 32-bit word
;	given in r25,r24,r23,22 (r25 is most significant for little endian)
;  param2: the subtrahend 32-bit word
;	given in r21,r20,r19,18 (r21 is most significant for little endian)
;  modifys: 
bigendian_sub32:
	sub r25, r21
	sbc r24, r20
	sbc r23, r19
	sbc r22, r18
	ret
	








	