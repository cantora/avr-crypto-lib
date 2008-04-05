/**
 * \file	main-des-test.c
 * \author	Daniel Otte 
 * \date	2007-06-17
 * \brief	test suit for DES
 * \par License	
 * GPL
 * 
 */
#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "des.h"

#include <stdint.h>
#include <string.h>


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	uart_putstr("\r\n==testy-encrypt==\r\n key: ");
	uart_hexdump(key,8);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,8);
	des_encrypt(block,block,key);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,8);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	uart_putstr("\r\n==testy-decrypt==\r\n key: ");
	uart_hexdump(key,8);
	uart_putstr("\r\n crypt: ");
	uart_hexdump(block,8);
	des_decrypt(block,block,key);
	uart_putstr("\r\n plain: ");
	uart_hexdump(block,8);
}

/******************************************************************************/

void testrun_des(void){
	
/*	uint8_t key[]= { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; */
/*	uint8_t data[]={ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; */

	
	uint8_t key[] = { 0x3b, 0x38, 0x98, 0x37, 0x15, 0x20, 0xf7, 0x5e };
	uint8_t data[]= { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
	testencrypt(data,key);
	testdecrypt(data,key);	
}

/******************************************************************************/

void nessie_testenc(uint8_t* data, uint8_t* key){
	uint16_t i;
	uart_putstr("\r\n\t                key = \t"); uart_hexdump(key, 8);
	uart_putstr("\r\n\t              plain = \t"); uart_hexdump(data, 8);
	des_encrypt(data,data,key);
	uart_putstr("\r\n\t             cipher = \t"); uart_hexdump(data, 8);
	des_decrypt(data,data,key);
	uart_putstr("\r\n\t          decrypted = \t"); uart_hexdump(data, 8);
	
	for(i=0;i<100; ++i)
		des_encrypt(data,data,key);
	uart_putstr("\r\n\tIterated  100 times = \t"); uart_hexdump(data, 8);
	for(;i<1000; ++i)
		des_encrypt(data,data,key);
	uart_putstr("\r\n\tIterated 1000 times = \t"); uart_hexdump(data, 8);
	
}

/******************************************************************************/
/*
Set 8, vector#  0:
                           key=0001020304050607
                        cipher=0011223344556677
                         plain=41AD068548809D02
                     encrypted=0011223344556677
*/
void nessie_testdec(uint8_t* data, uint8_t* key){
	uart_putstr("\r\n\t                key = \t"); uart_hexdump(key, 8);
	uart_putstr("\r\n\t             cipher = \t"); uart_hexdump(data, 8);
	des_decrypt(data,data,key);
	uart_putstr("\r\n\t             plain = \t"); uart_hexdump(data, 8);
	des_encrypt(data,data,key);
	uart_putstr("\r\n\t          encrypted = \t"); uart_hexdump(data, 8);	
}

/******************************************************************************/

void nessie_testrun(void){
	/* 
		Set 1, vector#  0:
                           key=8000000000000000
                         plain=0000000000000000
                        cipher=95A8D72813DAA94D
                     decrypted=0000000000000000
            Iterated 100 times=F749E1F8DEFAF605
           Iterated 1000 times=F396DD0B33D04244
	 */
	uint8_t key[8]; 
	uint8_t data[8];
	uint16_t set=1, vector;
	/* set 1 */
	for(vector=0;vector<64;++vector){
		memset(key, 0, 8);
		memset(data, 0, 8);
		key[vector/8] = 1<<(7-vector%8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 1);
		nessie_testenc(data, key);
	}
	/* set 2 */
	set = 2;
	for(vector=0;vector<64;++vector){
		memset(key, 0, 8);
		memset(data, 0, 8);
		data[vector/8] = 1<<(7-vector%8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 1);
		nessie_testenc(data, key);
	}
	/* set 3 */
	set = 3;
	for(vector=0;vector<256;++vector){
		memset(key, vector, 8);
		memset(data, vector, 8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 2);
		nessie_testenc(data, key);
	}
	/* set 4 */
	set = 4;
	{	uint8_t lk[2][8] = {	/* local keys */
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 },
			{ 0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00 } };
        uint8_t ld[2][8] = {  /* local data */
            { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 }, 
            { 0xEA, 0x02, 0x47, 0x14, 0xAD, 0x5C, 0x4D, 0x84 } };
          
		for(vector=0;vector<2;++vector){
			uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
			uart_putstr(", vector# "); uart_hexdump(&vector, 2);
			nessie_testenc(ld[vector], lk[vector]);
		}	
	}
	/* set 5 */
	set = 5;
	for(vector=0;vector<64;++vector){
		memset(key, 0, 8);
		memset(data, 0, 8);
		key[vector/8] = 1<<(7-vector%8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 1);
		nessie_testdec(data, key);
	}
	/* set 6 */
	set = 6;
	for(vector=0;vector<64;++vector){
		memset(key, 0, 8);
		memset(data, 0, 8);
		data[vector/8] = 1<<(7-vector%8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 1);
		nessie_testdec(data, key);
	}
	/* set 7 */
	set = 7;
	for(vector=0;vector<256;++vector){
		memset(key, vector, 8);
		memset(data, vector, 8);
		uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
		uart_putstr(", vector# "); uart_hexdump(&vector, 2);
		nessie_testdec(data, key);
	}
	/* set 8 */
	set = 8;
	{	uint8_t lk[2][8] = {	/* local keys */
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 },
			{ 0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00 } };
        uint8_t ld[2][8] = {  /* local data */
            { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 }, 
            { 0xEA, 0x02, 0x47, 0x14, 0xAD, 0x5C, 0x4D, 0x84 } };
          
		for(vector=0;vector<2;++vector){
			uart_putstr("\r\n Set "); uart_hexdump(&set, 1);
			uart_putstr(", vector# "); uart_hexdump(&vector, 2);
			nessie_testdec(ld[vector], lk[vector]);
		}	
	}	
	
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (DES; ");
	uart_putstr(__DATE__);uart_putstr(", ");uart_putstr(__TIME__);
	uart_putstr(")\r\nloaded and running\r\n");

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (!strcmp(str, "test")) 	{ testrun_des(); }
		else if (!strcmp(str, "nessie")) { nessie_testrun();}
		else {DEBUG_S("DBG: 1b\r\n"); goto error;}
			
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
	
}

