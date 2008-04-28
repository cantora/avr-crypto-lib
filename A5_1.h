/* 
 * File:		A5_1.h
 * Author:	Daniel Otte
 * Date: 	24.06.2006
 * License: GPL
 * Description: Implementation of the A5/1 stream cipher algorithm, as used in GSM.
 * ! Warning, this is weak crypto !
 * 
 */
#ifndef A5_1_H_
#define A5_1_H_

#include <stdint.h>

#define R1_LENGTH 19
#define R2_LENGTH 22
#define R3_LENGTH 23
#define R1_CLK 11
#define R2_CLK 12
#define R3_CLK 13

/* 3-Bit word parity lookup table (Byte) 
 * 0: 0
 * 1: 1
 * 2: 1
 * 3: 0
 * 4: 1
 * 5: 0
 * 6: 0
 * 7: 1
 * => 1001.0110 = 0x96
 * 
 */

#define PARITY_LOOKUP 0x96

typedef struct {
	/* we are wasting one byte here but this allows a much faster implementation */
	uint8_t r1[3], r2[3], r3[3]; /* the three regs, 19,22,23 bit in length  */
} a5_1_ctx_t;
 

void		a5_1_init(a5_1_ctx_t *c, void* key, uint8_t keylength_b, void* iv, uint8_t ivlength_b);
uint8_t		a5_1_clock(a5_1_ctx_t *c);
uint8_t 	a5_1_gen(a5_1_ctx_t *c);

#endif
