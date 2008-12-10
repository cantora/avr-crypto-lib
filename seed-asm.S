/* seed-asm.S */
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
/**
 * \file	seed-asm.S
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief	SEED parts in assembler for AVR
 * \par License	
 * GPLv3 or later
 * 
 */
#include "avr-asm-macros.S"

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

/******************************************************************************/	
/*
#define M0 0xfc
#define M1 0xf3
#define M2 0xcf
#define M3 0x3f

#define X3 (((uint8_t*)(&x))[0])
#define X2 (((uint8_t*)(&x))[1])
#define X1 (((uint8_t*)(&x))[2])
#define X0 (((uint8_t*)(&x))[3])

#define Z3 (((uint8_t*)(&z))[0])
#define Z2 (((uint8_t*)(&z))[1])
#define Z1 (((uint8_t*)(&z))[2])
#define Z0 (((uint8_t*)(&z))[3])

uint32_t g_function(uint32_t x){
	uint32_t z;
	/ * sbox substitution * /
	X3 = pgm_read_byte(&(seed_sbox2[X3]));
	X2 = pgm_read_byte(&(seed_sbox1[X2]));
	X1 = pgm_read_byte(&(seed_sbox2[X1]));
	X0 = pgm_read_byte(&(seed_sbox1[X0]));
	/ * now the permutation * /
	Z0 = (X0 & M0) ^ (X1 & M1) ^ (X2 & M2) ^ (X3 & M3);
	Z1 = (X0 & M1) ^ (X1 & M2) ^ (X2 & M3) ^ (X3 & M0);
	Z2 = (X0 & M2) ^ (X1 & M3) ^ (X2 & M0) ^ (X3 & M1);
	Z3 = (X0 & M3) ^ (X1 & M0) ^ (X2 & M1) ^ (X3 & M2);
	return z;
}
*/
M0 = 0xfc
M1 = 0xf3
M2 = 0xcf
M3 = 0x3f
X0 = 18
X1 = 19
X2 = 20
X3 = 21
Z0 = 25
Z1 = 24
Z2 = 23
Z3 = 22
T0 = X0
T1 = 26
T2 = 27
T3 = X1
/*
 *  param x: r22:r25
 *  X0 = R25
 *  X1 = R24
 *  X2 = R23
 *  X3 = R22
 */    
.global g_function
g_function:
	ldi r30, lo8(seed_sbox1)
	ldi r31, hi8(seed_sbox1)
 	movw r26, r30
	add r30, Z2
	adc r31, r1
	lpm X2, Z
	movw r30, r26
	add r30, Z0
	adc r31, r1
	lpm X0, Z
	inc r27 /* switch X to point to sbox2 */
	movw r30, r26
	add r30, Z3
	adc r31, r1
	lpm X3, Z
	movw r30, r26
	add r30, Z1
	adc r31, r1
	lpm X1, Z
	/* now the secound part */
	mov Z0, X0
	mov Z1, X0
	mov Z2, X0
	mov Z3, X0
	andi Z0, M0
	andi Z1, M1
	andi Z2, M2
	andi Z3, M3	
	mov T0, X1
	mov T1, X1
	mov T2, X1
	; mov T3, X1 /* T3 = X1 */
	andi T0, M1
	andi T1, M2
	andi T2, M3
	andi T3, M0
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	mov T0, X2
	mov T1, X2
	mov T2, X2
	mov T3, X2
	andi T0, M2
	andi T1, M3
	andi T2, M0
	andi T3, M1
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	mov T0, X3
	mov T1, X3
	mov T2, X3
	mov T3, X3
	andi T0, M3
	andi T1, M0
	andi T2, M1
	andi T3, M2
	eor Z0, T0
	eor Z1, T1
	eor Z2, T2
	eor Z3, T3
	ret

seed_sbox1:
.byte   169,  133,  214,  211,   84,   29,  172,   37 
.byte    93,   67,   24,   30,   81,  252,  202,   99 
.byte    40,   68,   32,  157,  224,  226,  200,   23 
.byte   165,  143,    3,  123,  187,   19,  210,  238 
.byte   112,  140,   63,  168,   50,  221,  246,  116 
.byte   236,  149,   11,   87,   92,   91,  189,    1 
.byte    36,   28,  115,  152,   16,  204,  242,  217 
.byte    44,   231, 114,  131,  155,  209,  134,  201 
.byte    96,   80,  163,  235,   13,  182,  158,   79 
.byte   183,   90,  198,  120,  166,   18,  175,  213 
.byte    97,  195,  180,   65,   82,  125,  141,    8 
.byte    31,  153,    0,   25,    4,   83,  247,  225 
.byte   253,  118,   47,   39,  176,  139,   14,  171 
.byte   162,  110,  147,   77,  105,  124,    9,   10 
.byte   191,  239,  243,  197,  135,   20,  254,  100 
.byte   222,   46,   75,   26,    6,   33,  107,  102 
.byte     2,  245,  146,  138,   12,  179,  126,  208 
.byte   122,   71,  150,  229,   38,  128,  173,  223 
.byte   161,   48,   55,  174,   54,   21,   34,   56 
.byte   244,  167,   69,   76,  129,  233,  132,  151 
.byte    53,  203,  206,   60,  113,   17,  199,  137 
.byte   117,  251,  218,  248,  148,   89,  130,  196 
.byte   255,   73,   57,  103,  192,  207,  215,  184 
.byte    15,  142,   66,   35,  145,  108,  219,  164 
.byte    52,  241,   72,  194,  111,   61,   45,   64 
.byte   190,   62,  188,  193,  170,  186,   78,   85 
.byte    59,  220,  104,  127,  156,  216,   74,   86 
.byte   119,  160,  237,   70,  181,   43,  101,  250 
.byte   227,  185,  177,  159,   94,  249,  230,  178 
.byte    49,  234,  109,   95,  228,  240,  205,  136 
.byte    22,   58,   88,  212,   98,   41,    7,   51 
.byte   232,   27,    5,  121,  144,  106,   42,  154


seed_sbox2:
.byte    56,  232,   45,  166,  207,  222,  179,  184 
.byte   175,   96,   85,  199,   68,  111,  107,   91 
.byte   195,   98,   51,  181,   41,  160,  226,  167 
.byte   211,  145,   17,    6,   28,  188,   54,   75 
.byte   239,  136,  108,  168,   23,  196,   22,  244 
.byte   194,   69,  225,  214,   63,   61,  142,  152 
.byte    40,   78,  246,   62,  165,  249,   13,  223 
.byte   216,   43,  102,  122,   39,   47,  241,  114 
.byte    66,  212,   65,  192,  115,  103,  172,  139 
.byte   247,  173,  128,   31,  202,   44,  170,   52 
.byte   210,   11,  238,  233,   93,  148,   24,  248 
.byte    87,  174,    8,  197,   19,  205,  134,  185 
.byte   255,  125,  193,   49,  245,  138,  106,  177 
.byte   209,   32,  215,    2,   34,    4,  104,  113 
.byte     7,  219,  157,  153,   97,  190,  230,   89 
.byte   221,   81,  144,  220,  154,  163,  171,  208 
.byte   129,   15,   71,   26,  227,  236,  141,  191 
.byte   150,  123,   92,  162,  161,   99,   35,   77 
.byte   200,  158,  156,   58,   12,   46,  186,  110 
.byte   159,   90,  242,  146,  243,   73,  120,  204 
.byte    21,  251,  112,  117,  127,   53,   16,    3 
.byte   100,  109,  198,  116,  213,  180,  234,    9 
.byte   118,   25,  254,   64,   18,  224,  189,    5 
.byte   250,    1,  240,   42,   94,  169,   86,   67 
.byte   133,   20,  137,  155,  176,  229,   72,  121 
.byte   151,  252,   30,  130,   33,  140,   27,   95 
.byte   119,   84,  178,   29,   37,   79,    0,   70 
.byte   237,   88,   82,  235,  126,  218,  201,  253 
.byte    48,  149,  101,   60,  182,  228,  187,  124 
.byte    14,   80,   57,   38,   50,  132,  105,  147 
.byte    55,  231,   36,  164,  203,   83,   10,  135 
.byte   217,   76,  131,  143,  206,   59,   74,  183 






