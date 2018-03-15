/*
 * stoneAssistantController.c
 *
 * Created: 05.03.2018 16:05:19
 * Author : Stone
 */ 

#define F_CPU  8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>


#include "rgbPwmTimerFan.h"
#include "uart.h"
#include "stoneProtocol.h"

//time in seconds to give the rpi to shutdown
#define RPI_SHUTDOWN_SECONDS	15

//time in seconds for button press to send soft shutdown request
#define RPI_SHUTDOWN_REQ_TIME	2

//time in seconds for button press to trigger hard shutdown
#define RPI_SHUTDOWN_HARD_TIME	10


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

void rpiHardReset(uint8_t shutdownSeconds)
{
	uint8_t i = 0; //iteration variable

	rgbPwm_setHexColor(0x00fffb26); //yellow light to indicate hard reset sequence has started

	for (i = 0; i < shutdownSeconds; i++)	//shutdown delay
		_delay_ms(1000);

	PORT_RPISUP |= 1<<RPISUP; //release raspberry pi power supply
	_delay_ms(500); //wait half a second to let the pi "cool down"
	PORT_RPISUP &= ~(1<<RPISUP); //turn on rpi power supply

	rgbPwm_setHexColor(0x00fffb26); //green light to indicate hard reset sequence has completed, rpi is on power again
}

void shutdownRequest()
{
	uint8_t requestMessage[2];

	requestMessage[0] = 'C';	//control message
	requestMessage[1] = 1;		//shutdown request

	stoneProtocol_transmit(requestMessage, 2);
}

int main(void)
{
	uint8_t inputChar = 0;
	uint8_t* payload = NULL;
	uint8_t protocolRet = 0;

	uint8_t button = 0;
	uint8_t buttonOld = 0;

	//init supply holding circuit (turned on)
	DDR_HOLD |= 1<<PIN_HOLD;
	PORT_HOLD &= ~(1<<PIN_HOLD);

	//init raspberry pi power control (turned on)
	DDR_RPISUP |= 1<<RPISUP;
	PORT_RPISUP &= ~(1<<RPISUP);

	//init button input with internal pull up enabled
	DDR_BUTTON &= ~(1<<BUTTON);
	PORT_BUTTON |= 1<<BUTTON;
	button = PIN_BUTTON & (1<<BUTTON);
	buttonOld = button;

	//init rgb and fan pwm timers (color: indigo, fan: 50%)
	rgbPwmTimerFan_init();
	rgbPwm_setHexColor(0);//x00b522f9);
	fan_setSpeed(127);

	//init leds
	DDR_LED  |= (1<<LED_SHDWN) | (1<<LED_RUN) | (1<<LED_UART);
	PORT_LED |= (1<<LED_SHDWN) | (1<<LED_RUN) | (1<<LED_UART);
	
	//init uart
	uart_init(9600,1,1);
	//init serial protocol
	stoneProtocol_init();

	//enable interrupts (needed for timer stuff)
	sei();

    while (1) 
    {
		//alive indication
		PORT_LED ^= (1<<LED_RUN);

		//button press handling
		button = PIN_BUTTON & (1<<BUTTON);
		if (buttonOld != button)
		{
			buttonOld = button;
			if (!button) //falling edge
			{
				timer_start();
			}
			else //rising edge
			{
				timer_stop();
			}
		}

		//button timer handling for shutdown
		if (timer_getTime() >= RPI_SHUTDOWN_REQ_TIME)
		{
			shutdownRequest();
		}
		if (timer_getTime() >= RPI_SHUTDOWN_HARD_TIME)
		{
			shutdown(1);
		}

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

							case 'f': //fan pwm - tested
							case 'F':
								fan_setSpeed(payload[1]);
							break;

							case 'c': //command
							case 'C':

								switch (payload[1])
								{
									case 0: //shutdown
										shutdown(RPI_SHUTDOWN_SECONDS);
									break;

									//case 1: //shutdown request - nothing to do on attiny, just here for protocol doc
									//break;

									case 6: //reboot
										rpiHardReset(RPI_SHUTDOWN_SECONDS);
									break;

									case 9: //hard reset
										rpiHardReset(0);
									break;

									case 10: //hard shutdown
										shutdown(0);
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

