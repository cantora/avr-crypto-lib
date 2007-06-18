/**
 * \file	main-shabea-test.c
 * \author	Daniel Otte 
 * \date	2007-06-07
 * \brief	test suit for SHABEA
 * \par License	
 * GPL
 * 
 */
#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "shabea.h"

#include <stdint.h>
#include <string.h>
#include <util/delay.h>

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	uart_putstr("\r\n==testy-encrypt==\r\n key: ");
	uart_hexdump(key,16);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,16);
	_delay_ms(50);
	shabea128(block,key,128,1,16);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,16);
}

void testdecrypt(uint8_t* block, uint8_t* key){

	uart_putstr("\r\n==testy-decrypt==\r\n key: ");
	uart_hexdump(key,16);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,16);
	_delay_ms(50);
	shabea128(block,key,128,0,16);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,16);
}

void testrun_shabea(void){
	uint8_t keys[4][16]=
		{ {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		  {	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
		  { 0x47, 0x06, 0x48, 0x08, 0x51, 0xE6, 0x1B, 0xE8,
		  	0x5D, 0x74, 0xBF, 0xB3, 0xFD, 0x95, 0x61, 0x85 },
		  { 0x28, 0xDB, 0xC3, 0xBC, 0x49, 0xFF, 0xD8, 0x7D,
		  	0xCF, 0xA5, 0x09, 0xB1, 0x1D, 0x42, 0x2B, 0xE7,}
		};
	uint8_t datas[4][16]=
		{ {	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
		  {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		  { 0x83, 0xA2, 0xF8, 0xA2, 0x88, 0x64, 0x1F, 0xB9, 
		  	0xA4, 0xE9, 0xA5, 0xCC, 0x2F, 0x13, 0x1C, 0x7D },
		  { 0xB4, 0x1E, 0x6B, 0xE2, 0xEB, 0xA8, 0x4A, 0x14, 
		  	0x8E, 0x2E, 0xED, 0x84, 0x59, 0x3C, 0x5E, 0xC7 }
		};
	uint8_t i=0;
	for(i=0; i<4; ++i){
		testencrypt(datas[i],keys[i]);
		testdecrypt(datas[i],keys[i]);	
	}
//	testdecrypt(data,key);	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (shabea)\r\nloaded and running\r\n");

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "test")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_shabea();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
	
}

