#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <avr/io.h>

#define F_CPU 16000000         // Oszillator-Frequenz in Hz

#define DEBUG uart

//c uart.[ch] defines
#define UART_INTERRUPT 1
#define UART_BAUD_RATE 38400
#define UART_RXBUFSIZE 16
#define UART_TXBUFSIZE 16
#define UART_LINE_BUFFER_SIZE 40
#undef UART_LEDS
/*
#define UART_HWFLOWCONTROL
#define UART_RTS_PORT PORTA
#define UART_RTS_DDR DDRA
#define UART_CTS_PIN PINA
#define UART_CTS_DDR DDRA
#define UART_RTS_BIT 0
#define UART_CTS_BIT 1
*/

#endif

