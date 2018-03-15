/*
 * rgbPwm.h
 *
 * Created: 05.03.2018 16:05:57
 *  Author: Stone
 */ 


#ifndef RGBPWM_H_
#define RGBPWM_H_


//initialization
 void rgbPwmTimerFan_init();

//set red led intensity
 void rgbPwm_setR(uint8_t r);

//set green led intensity
 void rgbPwm_setG(uint8_t g);

//set blue led intensity
 void rgbPwm_setB(uint8_t b);

//set led strip color in hex code fashion
 void rgbPwm_setHexColor(uint32_t hexCode);

//set fan speed
 void fan_setSpeed(uint8_t f);

//reset and start 100ms timer
 void timer_start();

//stop 100ms timer
 void timer_stop();

//check timer overflow
//returns 1 if overflow has occurred, 0 else
 uint8_t timer_overflow();

//check how many 100ms chunks have elapsed since start of the timer
//returns the number of 100ms chunks that have elapsed
 uint16_t timer_getTime();

#endif /* RGBPWM_H_ */