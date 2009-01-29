/* uart.c */
/*
    This file is part of the This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* USART-Init beim ATmegaXX */

#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "uart.h"

#ifdef ATMEGA128
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UDR UDR0
#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define URSEL UMSEL
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RXC_vect USART0_RX_vect
#define UCSRA UCSR0A
#endif

#ifdef ATMEGA644
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UDR UDR0
#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define URSEL UMSEL00
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RXC_vect USART0_RX_vect
#define UDRIE UDRIE0
#define TXEN TXEN0
#define UMSEL UMSEL0
#define RXEN RXEN0
#define RXCIE RXCIE0
#define UCSZ0 UCSZ00
#define UCSRA UCSR0A
#define UDRE UDRE0
#define RXC RXC0
#endif


#ifdef UART_XON_XOFF
 #ifdef UART_INTERRUPT
  void uart_insertc(char c);
 #else
  #define uart_insertc uart_putc
 #endif /* UART_INTERRUPT */
#endif

#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16L)-1)

#ifdef UART_XON_XOFF
	typedef enum{go=1,nogo=0} gonogo;
	static gonogo txon=go;
	static gonogo rxon=go;
#endif

#ifdef UART_INTERRUPT
volatile static char rxbuf[UART_RXBUFSIZE];
volatile static char txbuf[UART_TXBUFSIZE];
volatile static char *volatile rxhead, *volatile rxtail;
volatile static char *volatile txhead, *volatile txtail;

#ifdef UART_HOOK	
	 void (*uart_hook) (uint8_t) = (void*)0;	/* this is a pointer to a function ;-) */
#endif

ISR(USART_UDRE_vect) {
#ifdef UART_LEDS	
	PORTC ^= 0x01;
#endif
	
	if ( txhead == txtail ) {
		UCSRB &= ~(1 << UDRIE);		/* disable data register empty IRQ */
	} else {
		#ifdef UART_XON_XOFF
			while(txon==nogo)
				;
		#endif
		UDR = *txtail;			/* schreibt das Zeichen x auf die Schnittstelle */
		if (++txtail == (txbuf + UART_TXBUFSIZE)) txtail = txbuf;
	}
}

ISR(USART_RXC_vect) {
	int diff; 
	char c;
#ifdef UART_HOOK	
	static volatile uint8_t hook_running=0;
#endif
#ifdef UART_LEDS
	PORTC ^= 0x02;
#endif
	c=UDR;
	#ifdef UART_XON_XOFF
	 if (c==XON){
		  txon=go;
		  return;
	 }
	 if (c==XOFF){
		 txon=nogo;
		 return;
	 }
	#endif
	/* buffer full? */
	diff = rxhead - rxtail;
	if (diff < 0) diff += UART_RXBUFSIZE; /* diff is the amount of bytes in buffer */
	if (diff < UART_RXBUFSIZE -1) {
		// buffer NOT full
#ifdef UART_HOOK
		if(!hook_running && uart_hook){
			uint8_t t=c;	
			hook_running = 1;
			sei(); /* reenable interrupts, avoid recursion!!! */
			do {
				uart_hook(t);
			} while(uart_getc_nb((char*)&t));
			hook_running = 0;
		} else {
			*rxhead = c;
			++rxhead;
			if (rxhead == (rxbuf + UART_RXBUFSIZE)) rxhead = rxbuf;
		}
#else	
		*rxhead = c;
		++rxhead;
		if (rxhead == (rxbuf + UART_RXBUFSIZE))
			rxhead = rxbuf;
#endif
	} else {
						//reads the buffer to clear the interrupt condition
	}
#ifdef UART_XON_XOFF	
	if((diff > UART_XON_XOFF_THRESHOLD_1) && (rxon==go)){
		rxon=nogo;
		uart_insertc(XOFF);
	}
	if((diff < UART_XON_XOFF_THRESHOLD_2) && (rxon==nogo)){
		rxon=go;
		uart_insertc(XON);
	}
#endif
}

#endif // UART_INTERRUPT


void uart_init() {
	PORTD |= 0x01;				//Pullup an RXD an
	
	UCSRB |= (1<<TXEN);			//UART TX einschalten
#ifdef ATMEGA644
	UCSRA = 0;
	UCSRC = (3<<UCSZ0);		    //Asynchron 8N1
#else	
    UCSRA = 0;
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);		//Asynchron 8N1
#endif
	UCSRB |= ( 1 << RXEN );			//Uart RX einschalten

	UBRRH=(uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_CPU)>>8);
	UBRRL=(uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_CPU));

#ifdef UART_INTERRUPT
	// init buffers
	rxhead = rxtail = rxbuf;
	txhead = txtail = txbuf;

	// activate rx IRQ
	UCSRB |= _BV(RXCIE) | _BV(UDRIE);
	sei();
//	#ifdef ATMEGA644
//	UCSRB |= _BV(UDRIE);
//	#endif
#endif // UART_INTERRUPT
}

#ifdef UART_INTERRUPT
#ifdef UART_XON_XOFF

void uart_insertc(char c){
	volatile int diff;
	do {
		diff = txhead - txtail;
		if ( diff < 0 ) diff += UART_TXBUFSIZE;
	} while ( diff >= UART_TXBUFSIZE -1 );

	cli();
 	if (--txtail == (txbuf-1)) txtail += UART_TXBUFSIZE;
	*txtail = c;
	
	UCSRB |= (1 << UDRIE);		/* enable data register empty IRQ */
	sei();
}
#endif /* UART_XON_XOFF */
void uart_putc(char c) {
	volatile int diff;

	/* buffer full? */
	do {
		diff = txhead - txtail;
		if ( diff < 0 ) diff += UART_TXBUFSIZE;
	} while ( diff >= UART_TXBUFSIZE -1 );

	cli();
	*txhead = c;
 	if (++txhead == (txbuf + UART_TXBUFSIZE)) txhead = txbuf;

	UCSRB |= (1 << UDRIE);		/* enable data register empty IRQ */
	sei();
}
#else  // WITHOUT INTERRUPT
void uart_putc(char c) {
	while (!(UCSRA & (1<<UDRE))) /* warten bis Senden moeglich */
		; 
	#ifdef UART_XON_XOFF
	while (txon==nogo) 			 /* warte bis XON empfangen */
		;
	#endif
	UDR = c;                  /* schreibt das Zeichen x auf die Schnittstelle */
}
#endif // UART_INTERRUPT


void uart_putstr(char *str) {
	while(*str) {
		uart_putc(*str++);
	}
}

void uart_putstr_P(PGM_P str) {
	char tmp;
	while((tmp = pgm_read_byte(str))) {
		uart_putc(tmp);
		str++;
	}
}

void uart_hexdump(const void* buf, int len)
{
	unsigned char table[]={'0','1','2','3','4','5','6','7',
		                   '8','9','a','b','c','d','e','f'};
	while(len--){
		uart_putc(table[((*((char*)buf))>>4)&0xf]);
		uart_putc(table[(*((char*)buf))&0xf]);
		uart_putc(' ');
		buf=(uint8_t*)buf+1;
	}
}


#ifdef UART_INTERRUPT
char uart_getc(void)
{
	char val;

	while(rxhead==rxtail)
		;

	val = *rxtail;
 	++rxtail;
 	if (rxtail == (rxbuf + UART_RXBUFSIZE))
 		rxtail = rxbuf;

	return val;
}
#else  // WITHOUT INTERRUPT
char uart_getc(void)
{
	char t;
	while (!(UCSRA & (1<<RXC)))
		;	// warten bis Zeichen verfuegbar
	t=UDR;
	#ifdef UART_XON_XOFF
	if (t==XON)  txon=go;
	if (t==XOFF) txon=nogo;
	#endif
	return t;			// Zeichen aus UDR zurueckgeben
}
#endif // UART_INTERRUPT

// returns 1 on success
#ifdef UART_INTERRUPT
char uart_getc_nb(char *c)
{
	if (rxhead==rxtail) return 0;

	*c = *rxtail;
 	if (++rxtail == (rxbuf + UART_RXBUFSIZE)) rxtail = rxbuf;

	return 1;
}
#else  // WITHOUT INTERRUPT
char uart_getc_nb(char *c)
{
	if (UCSRA & (1<<RXC)) {		// Zeichen verfuegbar
		*c = UDR;
		#ifdef UART_XON_XOFF
			if (*c==XON)  txon=go;
			if (*c==XOFF) txon=nogo;
		#endif
		return 1;
	}

	return 0;
}
#endif // UART_INTERRUPT
