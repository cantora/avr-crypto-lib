/***************************
*
*
*
****************************/
#include "config.h"

#if DEBUG == uart
 #include "uart.h"
#else
  #error "Your DEBUG methode is not suported!"
#endif

#ifdef DEBUG
 void debug_init(void){
 #if DBUG==uart
  uart_init();
 #else
  #error "Your DEBUG methode is not suported!"
 #endif
 }
 
 void debug_char(char c){
	static char initialised = 0;
	if (!initialised){
		uart_init();
		initialised=1;
	}	
	uart_putc(c);
 }
 
 void debug_str(char* s){
 	while (*s)
 		debug_char(*s++);
 }
 


 void debug_byte(char b){
 	char table[] = "0123456789ABCDEF";
 	debug_char(table[(b>>4) & 0xf]);
 	debug_char(table[b&0xf]);
 }

#endif //DEBUG

