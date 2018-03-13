/*
 * stoneAssistantController.c
 *
 * Created: 05.03.2018 16:05:19
 * Author : Stone
 */ 

#define F_CPU  8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


#include "rgbPwm.h"
#include "uart.h"
#include "stoneProtocol.h"

#define DDR_LED		DDRD
#define PORT_LED	PORTD
#define LED_SHDWN   PD4
#define LED_RUN		PD3
#define LED_UART    PD2

#define DDR_HOLD	DDRB
#define PORT_HOLD	PORTB
#define PIN_HOLD	PB0



#define DDR_BUTTON	DDRB
#define PORT_BUTTON PORTB
#define PIN_BUTTON	PINB
#define BUTTON		PB1

#define DDR_RPISUP	DDRD
#define PORT_RPISUP	PORTD
#define RPISUP		PD6

#define DDR_FAN		DDRA
#define PORT_FAN	PORTA
#define PIN_FAN		PINA
#define FAN_PWM		PA1
#define FAN_SIGNAL	PA0

void shutdown(uint8_t shutdownSeconds)
{
	uint8_t i = 0; //iteration variable

	PORT_LED &= ~(1<<LED_SHDWN);
	rgbPwm_setHexColor(0x00FF0000); //red light to indicate shutdown sequence has started

	for (i = 0; i < shutdownSeconds; i++)	//shutdown delay
		_delay_ms(1000);

	PORT_RPISUP |= 1<<RPISUP; //release raspberry pi power supply
	PORT_HOLD |= 1<<PIN_HOLD; //release mains power supply (turn off)
}

int main(void)
{
	uint8_t inputChar = 0;
	uint8_t* payload = NULL;
	uint8_t protocolRet = 0;

	//init supply holding circuit (turned on)
	DDR_HOLD |= 1<<PIN_HOLD;
	PORT_HOLD &= ~(1<<PIN_HOLD);

	//init raspberry pi power control (turned on)
	DDR_RPISUP |= 1<<RPISUP;
	PORT_RPISUP &= ~(1<<RPISUP);

	//init button input with internal pull up enabled
	DDR_BUTTON &= ~(1<<BUTTON);
	PORT_BUTTON |= 1<<BUTTON;

	//init rgb pwm timers (color: blue/red 220/255)
	rgbPwm_init();
	rgbPwm_setHexColor(0x00FF00DC);

	//init fan control pins (fan turned off)
	DDR_FAN |= 1<<FAN_PWM;
	PORT_FAN &= ~(1<<FAN_PWM);
	DDR_FAN &= ~(1<<FAN_SIGNAL);
	//PORT_FAN |= (1<<FAN_SIGNAL);

	//init leds
	DDR_LED  |= (1<<LED_SHDWN) | (1<<LED_RUN) | (1<<LED_UART);
	PORT_LED |= (1<<LED_SHDWN) | (1<<LED_RUN) | (1<<LED_UART);
	
	//init uart
	uart_init(9600,1,1);
	//init serial protocol
	stoneProtocol_init();

    while (1) 
    {
		//alive indication
		PORT_LED ^= (1<<LED_RUN);

		//usart protocol handling, command execution
		if (uart_testAndGetC(&inputChar))
		{
			PORT_LED ^= (1<<LED_UART);

			protocolRet = (stoneProtocol_loop(inputChar));
			switch (protocolRet)
			{
				case 0: //no new packet
				break;

				case 1: //new packet received
					payload = stoneProtocol_getMessage();
					if (payload != NULL)
					{
						switch (payload[0])
						{
							case 'r': //red led pwm - tested
							case 'R':
								rgbPwm_setR(payload[1]);
							break;

							case 'g': //green led pwm - tested
							case 'G':
								rgbPwm_setG(payload[1]);
							break;

							case 'b': //blue led pwm - tested
							case 'B':
								rgbPwm_setB(payload[1]);
							break;

							case 'x': //hexcode color - tested
							case 'X':
								rgbPwm_setHexColor(((uint32_t)payload[1]<<16) | ((uint32_t)payload[2]<<8) | ((uint32_t)payload[3])); //0x00RRGGBB
							break;

							case 'f': //fan pwm
							case 'F':
								//todo
							break;

							case 'c': //command
							case 'C':

								//todo: shutdown-handshake
								switch (payload[1])
								{
									case 's': //shutdown
									case 'S':
										shutdown(10);
									break;

									case 'a': //acknowledge
									case 'A':
									break;

									case 'h': //heartbeat
									case 'H':
									break;

									default:
									break;
								}
							break;

							default:
							break;
						}
						free(payload);
					}
				break;

				case 2: //protocol error
					PORT_LED |= 1<<LED_UART;
				break;
			}
		}
    }
}

