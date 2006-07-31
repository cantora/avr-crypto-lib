/**
 * 
 * Author:	Daniel Otte
 * Date:		16.05.2006
 * 
 * This tools should help to parse some input. 
 * 
 */

#include "config.h"
#include "uart.h"
#include <string.h>
#include <stdint.h>

int getnextwordn(char *s, int n){ /* words are seperated by spaces */
	char c = ' ';
	while ((c=uart_getc()) == ' ')
		;
	*s++ = c;
	while (n && (*s++=uart_getc())!=' ')
		;
	*(s-1) = '\0';
	return n;
}


void readhex2buffer(void* buffer, int n){
	char c;
	uint8_t i;
	
//	DEBUG_S("\r\nDBG: n="); DEBUG_B(n&0xff); DEBUG_S("\r\n");
	for(i=0; i<n; ++i){
		c = uart_getc();
		if ('0'<= c && '9'>=c){
			((uint8_t*)buffer)[i] = c - '0';
		} else {
			c &= ~('A' ^ 'a'); /* make all uppercase */ 
			if ('A'<= c && 'F'>=c){
				((uint8_t*)buffer)[i] = c - 'A' + 10;
			} else {
				/* oh shit, wrong char */
			}
		}
		
		((uint8_t*)buffer)[i] <<= 4;
		
		c = uart_getc();
		if ('0'<= c && '9'>=c){
			((uint8_t*)buffer)[i] |= c - '0';
		} else {
			c &= ~('A' ^ 'a'); /* make all uppercase */ 
			if ('A'<= c && 'F'>=c){
				((uint8_t*)buffer)[i] |= c - 'A' + 10;
			} else {
				/* oh shit, wrong char */
			}
		}
	} /* for i=0 .. n */
}

void uart_putptr(void* p){
	uart_hexdump((void*) &p,2);
}

