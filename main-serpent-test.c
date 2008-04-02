/*
 * serpent test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "serpent.h"

#include <stdint.h>
#include <string.h>


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void dumpctx(serpent_ctx_t * ctx){
	uint8_t i;
	uart_putstr("\r\n --ctx dump--\r\n");
	for(i=0; i<33; ++i){
		uart_putstr("  K["); uart_putc('0'+i/10); uart_putc('0'+i%10); uart_putstr("] = ");	
	    uart_hexdump(ctx->k[i],16);
	    uart_putstr("\r\n");
	}
}

void testencrypt(uint8_t* block, uint8_t* key){
	serpent_ctx_t ctx;
	uart_putstr("\r\n==testy-encrypt==\r\n key: ");
	uart_hexdump(key,32);
	serpent_genctx(key, &ctx);	
//	dumpctx(&ctx);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,16);
	serpent_enc(block, &ctx);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,16);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	serpent_ctx_t ctx;
	uart_putstr("\r\n==testy-decrypt==\r\n key: ");
	uart_hexdump(key,32);
	serpent_genctx(key, &ctx);
//	dumpctx(&ctx);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,16);
	serpent_dec(block, &ctx);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,16);
}

/**
 Test vectors -- set 4
=====================

Set 4, vector#  0:
                           key=000102030405060708090A0B0C0D0E0F
                               101112131415161718191A1B1C1D1E1F
                         plain=00112233445566778899AABBCCDDEEFF
                        cipher=2868B7A2D28ECD5E4FDEFAC3C4330074
                     decrypted=00112233445566778899AABBCCDDEEFF
            Iterated 100 times=8BF56992354F3F1A0F4E49DCBA82CBC0
           Iterated 1000 times=9B1D8B34845DF9BFD36AAAD0CDA1C8FE

Set 4, vector#  1:
                           key=2BD6459F82C5B300952C49104881FF48
                               2BD6459F82C5B300952C49104881FF48
                         plain=EA024714AD5C4D84EA024714AD5C4D84
                        cipher=3E507730776B93FDEA661235E1DD99F0
                     decrypted=EA024714AD5C4D84EA024714AD5C4D84
            Iterated 100 times=3B5462E5D87A40C4BE745E3994D5E373
           Iterated 1000 times=99D5D067EF7C787E6A764EB47DAC59AD


Set 1, vector#  0:
                           key=80000000000000000000000000000000
                               00000000000000000000000000000000
                         plain=00000000000000000000000000000000
                        cipher=A223AA1288463C0E2BE38EBD825616C0
                     decrypted=00000000000000000000000000000000
            Iterated 100 times=739E0148971FD975B585EAFDBD659E2C
           Iterated 1000 times=BEFD00E0D6E27E56951DC6614440D286
 
*/

void testrun_serpent(void){
	
	uint8_t key[]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
		            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F };
                    
	uint8_t data[]={ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		             0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
/* * /
	uint8_t key[]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 
		            
	uint8_t data[]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
*/
	testencrypt(data,key);
	testdecrypt(data,key);	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (serpent)\r\nloaded and running\r\n");

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "test")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_serpent();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
	
}

