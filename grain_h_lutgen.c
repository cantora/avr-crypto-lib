/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 *
 * this program generate a lookuptable for the h-function in grain 
 */

#include <stdint.h>
#include <stdio.h>

#define X(i) ((x)>>((i)))
uint8_t h(uint8_t x){
	uint8_t h;
	
	h = (X(1)) ^ (X(4)) ^ 
	    (X(0)&X(3)) ^ (X(2)&X(3)) ^ (X(3)&X(4)) ^ 
	    (X(0)&X(1)&X(2)) ^ (X(0)&X(2)&X(3)) ^ (X(0)&X(2)&X(4)) ^ 
	    (X(1)&X(2)&X(4)) ^ (X(2)&X(3)&X(4)) ;
	
	return h&1;
}

int main(void){
	uint8_t i; 
	uint32_t lut;
	puts(
	"/* \n"
	" * author: Daniel Otte \n"
	" * email:  daniel.otte@rub.de  \n"
	" * license: GPLv3 \n"
	" *  \n"
	" * this program generate a lookuptable for the h-function in grain  \n"
	" *  \n"
	" */ \n");
	puts("/* \n"
	     " * x0 x1 x2 x3 x4 - h");
	
	for(i=0; i<0x20; ++i){
		printf(" *  %c  %c  %c  %c  %c - %c\n",
			(i&0x01)?'1':'0',
			(i&0x02)?'1':'0',
			(i&0x04)?'1':'0',
			(i&0x08)?'1':'0',
			(i&0x10)?'1':'0',
			(h(i))?'1':'0' );
			lut >>=1;
			lut |= h(i)?0x80000000:0x00000000;
			if(i%4==3){	
				puts(" * --");
			}
	}
	puts(" */\n");
	printf(" uint8_t lut[4]= {0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X} \n",
		lut&0xFF, (lut>>8)&0xFF, (lut>>16)&0xFF, (lut>>24)&0xFF);
		
	return 0; 
}

