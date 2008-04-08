/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * components to help implementing simple command based interaction
 * 
 **/
 
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>

int16_t findstring_d0(const char* str, const char* v){
	uint8_t i=0;
	while(*v){	
		if(!strcmp(str, v)){
			return i;
		}
		while(*v++) /* go to the next string */
		;
		++i;
	}
	return -1;
}
 
int16_t findstring_d0_P(const char* str, PGM_P v){
	uint8_t i=0;
	while(pgm_read_byte(v)){	
		if(!strcmp_P(str, v)){
			return i;
		}
		while(pgm_read_byte(v++)) /* go to the next string */
		;
		++i;
	}
	return -1;
} 

int16_t execcommand_d0_P(const char* str, PGM_P v, void(*fpt[])(void) ){
	uint8_t i=0;
	while(pgm_read_byte(v)){	
		if(!strcmp_P(str, v)){
			(fpt[i])();
			return i;
		}
		while(pgm_read_byte(v++)) /* go to the next string */
		;
		++i;
	}
	return -1;
}


