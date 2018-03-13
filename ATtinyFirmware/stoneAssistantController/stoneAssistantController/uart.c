/*
 * uart.c
 *
 * Created: 05.03.2018 16:40:38
 *  Author: Stone
 */ 

 #include <avr/interrupt.h>
 #include "uart.h"

 #define F_CPU 8000000UL

 #define DDR_UART    DDRD
 #define PORT_UART   PORTD
 #define PIN_TXD	PD1
 #define PIN_RXD	PD0

 void uart_init(uint16_t baud, uint8_t rxEn, uint8_t txEn)
 {
   uint16_t ubrr;

   //configure data direction
   DDR_UART |= 1<<PIN_TXD;
   DDR_UART &= ~(1<<PIN_RXD);

   //calculate baud generator register
   ubrr = F_CPU / 16 / baud - 1;
   //set baud generator register
   UBRRH = (ubrr & 0xFF00) >> 8;
   UBRRL = (ubrr & 0x00FF);

   //enable transmit/receive + interrupts
   if (rxEn)
   {
	 UCSRB |= 1<<RXEN;
   }
   if (txEn)
   {
     UCSRB |= 1<<TXEN;
   }   
 }

 uint8_t uart_putc(uint8_t c)
 {
	while (!(UCSRA & (1<<UDRE))); //wait until buffer is empty to send new character
	UDR = c; //send c
	return 1;
 }

 uint8_t uart_testAndGetC(uint8_t* c)
 {
	if (UCSRA & (1<<RXC)) //if there is a newly received character
	{					  //read the character and return 1
		(*c) = UDR;
		return 1;
	}
	return 0;
 }