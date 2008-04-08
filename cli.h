#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>
#include <avr/pgmspace.h>

int16_t findstring_d0(const char* str, const char* v);
int16_t findstring_d0_P(const char* str, PGM_P v);

int16_t execcommand_d0_P(const char* str, PGM_P v, void(*fpt[])(void) );
#endif /*CLI_H_*/
