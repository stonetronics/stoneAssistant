/*
 * rgbPwm.h
 *
 * Created: 05.03.2018 16:05:57
 *  Author: Stone
 */ 


#ifndef RGBPWM_H_
#define RGBPWM_H_

 void rgbPwm_init();

 void rgbPwm_setR(uint8_t r);

 void rgbPwm_setG(uint8_t g);

 void rgbPwm_setB(uint8_t b);

 void rgbPwm_setHexColor(uint32_t hexCode);



#endif /* RGBPWM_H_ */