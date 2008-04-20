#ifndef PERFORMANCE_TEST_H_
#define PERFORMANCE_TEST_H_

#include <stdint.h>
#include <avr/pgmspace.h>

void calibrateTimer(void);
void startTimer(uint8_t granularity);
uint64_t stopTimer(void);
void getOverhead(uint16_t* constoh, uint16_t* intoh);

void print_time_P(PGM_P s, uint64_t t);
void print_overhead(void);

#endif /*PERFORMANCE_TEST_H_*/
