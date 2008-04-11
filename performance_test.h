#ifndef PERFORMANCE_TEST_H_
#define PERFORMANCE_TEST_H_

#include <stdint.h>

void calibrateTimer(void);
void startTimer(uint8_t granularity);
uint64_t stopTimer(void);
void getOverhead(uint16_t* constoh, uint16_t* intoh);

#endif /*PERFORMANCE_TEST_H_*/
