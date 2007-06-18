/* USART-Init beim ATmegaXX */

#include "config.h"

#include <avr/io.h>
//#include <avr/signal.h>
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
#endif


#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16L)-1)


#ifdef UART_INTERRUPT
volatile static char rxbuf[UART_RXBUFSIZE];
volatile static char txbuf[UART_TXBUFSIZE];
volatile static char *volatile rxhead, *volatile rxtail;
volatile static char *volatile txhead, *volatile txtail;


SIGNAL(SIG_UART_DATA) {
#ifdef UART_LEDS	
	PORTC ^= 0x01;
#endif
	
	if ( txhead == txtail ) {
		UCSRB &= ~(1 << UDRIE);		/* disable data register empty IRQ */
	} else {
		UDR = *txtail;			/* schreibt das Zeichen x auf die Schnittstelle */
		if (++txtail == (txbuf + UART_TXBUFSIZE)) txtail = txbuf;
	}
}

SIGNAL(SIG_UART_RECV) {
	int diff; 

#ifdef UART_LEDS
	PORTC ^= 0x02;
#endif
	
	/* buffer full? */
	diff = rxhead - rxtail;
	if ( diff < 0 ) diff += UART_RXBUFSIZE;
	if (diff < UART_RXBUFSIZE -1) {
		// buffer NOT full
		*rxhead = UDR;
		if (++rxhead == (rxbuf + UART_RXBUFSIZE)) rxhead = rxbuf;
	} else {
		UDR;				//reads the buffer to clear the interrupt condition
	}
}

#endif // UART_INTERRUPT


void uart_init(void) {
	PORTD |= 0x01;				//Pullup an RXD an

	UCSRB |= (1<<TXEN);			//UART TX einschalten
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);		//Asynchron 8N1

	UCSRB |= ( 1 << RXEN );			//Uart RX einschalten

	UBRRH=(uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_CPU)>>8);
	UBRRL=(uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_CPU));

#ifdef UART_INTERRUPT
	// init buffers
	rxhead = rxtail = rxbuf;
	txhead = txtail = txbuf;

	// activate rx IRQ
	UCSRB |= (1 << RXCIE);
#endif // UART_INTERRUPT
}

#ifdef UART_INTERRUPT
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
	while (!(UCSRA & (1<<UDRE))); /* warten bis Senden moeglich                   */
	UDR = c;                      /* schreibt das Zeichen x auf die Schnittstelle */
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

void uart_hexdump(void* buf, int len)
{
	unsigned char table[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	
	while(len--){
		uart_putc(table[((*((char*)buf))>>4)&0xf]);
		uart_putc(table[(*((char*)buf))&0xf]);
		uart_putc(' ');
		buf=(char*)buf+1;
	}
}


#ifdef UART_INTERRUPT
char uart_getc(void)
{
	char val;

	while(rxhead==rxtail) ;

	val = *rxtail;
 	if (++rxtail == (rxbuf + UART_RXBUFSIZE)) rxtail = rxbuf;

	return val;
}
#else  // WITHOUT INTERRUPT
char uart_getc(void)
{
	while (!(UCSRA & (1<<RXC)));	// warten bis Zeichen verfuegbar
	return UDR;			// Zeichen aus UDR zurueckgeben
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
		return 1;
	}

	return 0;
}
#endif // UART_INTERRUPT
