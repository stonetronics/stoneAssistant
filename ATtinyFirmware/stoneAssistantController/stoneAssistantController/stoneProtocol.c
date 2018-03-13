/*
 * stoneProtocol.c
 *
 * Created: 06.03.2018 17:50:05
 *  Author: Stone
 */ 

 #include "stoneProtocol.h"
 #include "uart.h"

 #include <stdlib.h>

 #define PORT_LED	PORTD
 #define LED_SHDWN   PD4
 #define LED_RUN		PD3
 #define LED_UART    PD2

 typedef enum {IDLE, WAIT_LEN, DATA, WAIT_STOP} stoneProtocolState;

 volatile stoneProtocolState state = IDLE;
 
 volatile uint8_t length = 0;
 volatile uint8_t i = 0;
 volatile uint8_t* pData = NULL;
 volatile uint8_t messageReady = 0;


 void stoneProtocol_init()
 {
	state = IDLE;
	messageReady = 0;
 }

 uint8_t stoneProtocol_loop(uint8_t in)
 {
	uint8_t retVal = 0;
	switch(state)
	{
	    case IDLE:
			if(in == STP_STARTBYTE)
			{
				state = WAIT_LEN;
			}
			break;
		case WAIT_LEN:
			length = in;
			pData = malloc(length + 1);
			i = 0;
			state = DATA;
			break;
		case DATA:
			*(pData + i) = in;
			i++;
			if (i==length)
			{
				*(pData + i) = '\0';
				state = WAIT_STOP;
			}
			break;
		case WAIT_STOP:
			if (in == STP_STOPBYTE)
				retVal = 1;
			else
				retVal = 2;
			state = IDLE;
			messageReady = 1;
			break;
		default:
			break;
	}

	return retVal;
 }

 uint8_t* stoneProtocol_getMessage()
 {
	uint8_t* pTemp = NULL;
	if (messageReady)
	{
		messageReady = 0;
		pTemp = (uint8_t*)pData;
		pData = NULL;
	}
	return pTemp;
 }

 void stoneProtocol_transmit(uint8_t* data, uint8_t length)
 {
	uint8_t i = 0; //iteration variable

	uart_putc(STP_STARTBYTE);
	uart_putc(length);
	i = 0;
	while (i < length)
		uart_putc(data[i]);
	uart_putc(STP_STOPBYTE);
 }