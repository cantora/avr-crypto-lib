/**
 *
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 *
 */

#include <stdio.h>
#include <stdint.h>

uint8_t getnextrc(uint8_t a){
	if((a&0x80) != 0){
		return (a<<1) ^ 0x1B;
	} else {
		return (a<<1);
	}
}

#define N 32

int main(void){
	uint8_t c=0x80;
	uint32_t i;
	puts("\nNoekeon Round Constants:");
	for(i=0; i<N; ++i){
		printf(" 0x%2.2X,", c);
		if(i%8==7){
			puts("");
		}
		c=getnextrc(c);
	}
	return 0;
}

