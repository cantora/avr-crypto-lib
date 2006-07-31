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
	uint32_t r1,r2,r3; /* the three regs, 19,22,23 bit in length  */
} a5_1_ctx_t;
 

void		a5_1_init(a5_1_ctx_t *c, uint8_t *key, uint8_t length);
bool		a5_1_clock(a5_1_ctx_t *c);
uint8_t a5_1_gen(a5_	1_ctx_t *c);

#endif
