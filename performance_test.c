/*
 * 
 * 
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "performance_test.h"

uint32_t ovfcounter;

uint16_t const_overhead=0;
uint16_t int_overhead=0;

ISR(TIMER1_OVF_vect){
	ovfcounter++;
}

void calibrateTimer(void){
	startTimer(1);
	stopTimer();
	const_overhead = TCNT1;
	startTimer(1);
	TCNT1=0xFFFE;
	; ; ; ;
//	asm volatile("NOP\n"::); asm volatile("NOP\n"::);
	stopTimer();
	int_overhead = TCNT1;
}

void startTimer(uint8_t granularity){
	TCCR1B = 0; /* stop timer */
	TCNT1  = 0;
	ovfcounter = 0;
	TCCR1A = 0x00;
	TIMSK &= 0xC3;
	TIMSK |= _BV(2); /* enable TOIE1 */
	TCCR1B = granularity & 0x7;	/* start timer */
}

uint64_t stopTimer(void){
	TCCR1B = 0; /* stop timer */
	uint64_t ret;
	ret = (ovfcounter<<16) | TCNT1;
	ret -= const_overhead;
	ret -= ovfcounter * int_overhead;
	return ret;
}

void getOverhead(uint16_t* constoh, uint16_t* intoh){
	*constoh = const_overhead;
	*intoh   = int_overhead; 
}




