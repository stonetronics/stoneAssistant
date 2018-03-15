/*
 * rgbPwm.c
 *
 * Created: 05.03.2018 16:06:15
 *  Author: Stone
 */ 

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "rgbPwmTimerFan.h"


 #define DDR_PWM	DDRB
 #define PORT_PWM	PORTB
 #define PIN_PWMB	PB4
 #define PIN_PWMR	PB3
 #define PIN_PWMG	PB2

 #define DDR_FAN		DDRA
 #define PORT_FAN	PORTA
 #define PIN_FAN		PINA
 #define FAN_PWM		PA1
 #define FAN_SIGNAL	PA0

 volatile uint8_t timerRunning;
 volatile uint16_t hundretMsCounter;
 volatile uint16_t overflowCounter;
 volatile uint8_t overflowFlag;

 void rgbPwmTimerFan_init() {
	timerRunning = 0;
	hundretMsCounter = 0;
	overflowCounter = 0;
	overflowFlag = 0;

	//init rgb pwm channel output pins (turned off)
	DDR_PWM |= (1<<PIN_PWMR) | (1<<PIN_PWMG) | (1<<PIN_PWMB);
	PORT_PWM &= ~((1<<PIN_PWMR) | (1<<PIN_PWMG) | (1<<PIN_PWMB));

	//init fan control pins (fan turned off)
	DDR_FAN |= 1<<FAN_PWM;
	PORT_FAN &= ~(1<<FAN_PWM);
	DDR_FAN &= ~(1<<FAN_SIGNAL);
	//PORT_FAN |= (1<<FAN_SIGNAL);

	//timer 0 initialization G - OC0A; interrupt used for Fan - OCR0B; also used for 100ms increment
	TCCR0A |= (2<<COM0A0) | (3<<WGM00); //Fast PWM mode with oc0a = 1 at TCNT0 < OCR0A
	TCCR0B |= (4<<CS00); //start timer with clk/256 = 31.25kHz
	TIMSK |= (1<<OCIE0B) | (1<<TOIE0); //enable interrupts for ocr0b and overflow
	TCNT0 = 0;
	OCR0A = 0;
	OCR0B = 0;

	//timer 1 initialization R - OC1A; B - OC1B;
	TCCR1A |= (2<<COM1A0) | (2<<COM1B0) | (1<<WGM10); //8-Bit Fast PWM with oc1a and oc1b pwm operation
	TCCR1B |= (1<<WGM12) | (4<<CS10); //start timer with clk/256 = 31.25kHz
	TCNT1H = 0;
	TCNT1L = 0;
	OCR1AH = 0;
	OCR1AL = 0;
	OCR1BH = 0;
	OCR1BL = 0;
 }

 void rgbPwm_setR(uint8_t r)
 {
	OCR1AL = r;
 }

 void rgbPwm_setG(uint8_t g)
 {
	OCR0A = g;
 }

 void rgbPwm_setB(uint8_t b)
 {
	OCR1BL = b;
 }

 void rgbPwm_setHexColor(uint32_t hexCode)
 {
	rgbPwm_setR((hexCode & 0x00FF0000)>>16);
	rgbPwm_setG((hexCode & 0x0000FF00)>>8);
	rgbPwm_setB((hexCode & 0x000000FF));
 }

 void fan_setSpeed(uint8_t f)
 {
	OCR0B = f;
 }

 void timer_start()
 {
	uint8_t temp_SREG = 0;

	temp_SREG = SREG;

	cli();

	hundretMsCounter = 0;
	overflowCounter = 0;
	overflowFlag = 0;
	timerRunning = 1;

	SREG = temp_SREG;
 }

 void timer_stop()
 {
	timerRunning = 0;
 }

 uint8_t timer_overflow()
 {
	return overflowFlag;
 }

 uint16_t timer_getTime()
 {
	return hundretMsCounter;
 }

 ISR(TIMER0_COMPB_vect)
 {
	PORT_FAN &= ~(1<<FAN_PWM); //set fan pin low at end of pulse
 }

 ISR(TIMER0_OVF_vect)
 {
	PORT_FAN |= (1<<FAN_PWM); //set fan pin high at start of pulse

	if (timerRunning) //increment timer if enabled
	{
		//counter for 1s
		if ((overflowCounter++) == 122) //999,424ms
		{
			overflowCounter = 0;

			//100ms chunks counter
			if ((hundretMsCounter++) == 0xFFFF)
			{
				hundretMsCounter = 0;
				overflowFlag = 1;
			}
		}
	}
 }
