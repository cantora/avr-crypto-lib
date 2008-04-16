/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 *
 * this program generate a lookuptable for the nfsr-feedback-function in grain 
 */

#include <stdint.h>
#include <stdio.h>

#define X(i) ((x)>>((i)))
#define B63 X(0)
#define B60 X(3)
#define B52 X(5)
#define B45 X(6)
#define B37 X(4)
#define B33 X(8)
#define B28 X(2)
#define B21 X(9)
#define B15 X(1)
#define B09 X(7)

uint8_t g(uint16_t x){
	uint8_t a,b,d,e;
	uint8_t ret;

	ret = B60 ^ B52 ^ B45 ^ B37 ^ B33 ^ B28 ^ B21 ^ B09;
	ret ^= (a = B63 & B60);
	ret ^= (b = B37 & B33);
	ret ^= B15 & B09;
	ret ^= (d = B60 & B52 & B45);
	ret ^= (e = B33 & B28 & B21);
	ret ^= B63 & B45 & B28 & B09;
	ret ^= b & B60 & B52;
	ret ^= a & B21 & B15;
	ret ^= d & B63 & B37;
	ret ^= e & B15 & B09;
	ret ^= e & B52 & B45 & B37;
	
	return ret&1;
}

int main(void){
	uint16_t i; 
	uint8_t t, lut[128]={0}; /* 2**10 / 8 == 2**(10-3) == 2**7 == 128 */
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
	     " * b63 b15 b28 b60 b37 b52 b45 b09 b33 b21 - g");
	
	for(i=0; i<0x0400; ++i){
		t = g(i);
		printf(" *  %c   %c   %c   %c   %c   %c   %c   %c   %c   %c  - %c\n",
			(i&0x01)?'1':'0',
			(i&0x02)?'1':'0',
			(i&0x04)?'1':'0',
			(i&0x08)?'1':'0',
			(i&0x10)?'1':'0',
			(i&0x20)?'1':'0',
			(i&0x40)?'1':'0',
			(i&0x80)?'1':'0',
			(i&0x0100)?'1':'0',
			(i&0x0200)?'1':'0',
			t?'1':'0' );
		lut[i/8] |= t<<(i%8);
//		if(i%4==3){	
//			puts(" * --");
//		}
	}
	puts(" */\n");
	
	printf(" uint8_t g_lut[128]= {");
	for(i=0; i<128; ++i){
		if(i%16==0){
			printf("\n\t");
		}
		printf("0x%2.2X%c ", lut[i], (i!=127)?',':' ');
	}
	printf("};\n\n");
	return 0; 
}

