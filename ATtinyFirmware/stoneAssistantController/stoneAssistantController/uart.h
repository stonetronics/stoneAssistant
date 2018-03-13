/*
 * uart.h
 *
 * Created: 05.03.2018 16:41:42
 *  Author: Stone
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

//init usart with baud and receiver and/or transmitter
void uart_init(uint16_t baud, uint8_t rxEn, uint8_t txEn);

//put c
//returns 1 on success, 0 if buffer is full
uint8_t uart_putc(uint8_t c);

//return true = new character loaded into c
//return false = now new character received
uint8_t uart_testAndGetC(uint8_t* c);




#endif /* UART_H_ */