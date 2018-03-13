/*
 * rgbPwm.c
 *
 * Created: 05.03.2018 16:06:15
 *  Author: Stone
 */ 

 #include <avr/io.h>
 #include "rgbPwm.h"

 #define DDR_PWM	DDRB
 #define PORT_PWM	PORTB
 #define PIN_PWMB	PB4
 #define PIN_PWMR	PB3
 #define PIN_PWMG	PB2

 void rgbPwm_init() {
	//init rgb pwm channel output pins (turned off)
	DDR_PWM |= (1<<PIN_PWMR) | (1<<PIN_PWMG) | (1<<PIN_PWMB);
	PORT_PWM &= ~((1<<PIN_PWMR) | (1<<PIN_PWMG) | (1<<PIN_PWMB));

	//timer 0 initialization G - OC0A
	TCCR0A |= (2<<COM0A0) | (3<<WGM00); //Fast PWM mode with oc0a = 1 at TCNT0 < OCR0A
	TCCR0B |= (4<<CS00); //start timer with clk/256 = 31.25kHz
	TCNT0 = 0;
	OCR0A = 0;

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