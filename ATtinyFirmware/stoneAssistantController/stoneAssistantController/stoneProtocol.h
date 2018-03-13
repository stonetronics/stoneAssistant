/*
 * stoneProtocol.h
 *
 * Created: 06.03.2018 17:49:50
 *  Author: Stone
 */ 


#ifndef STONEPROTOCOL_H_
#define STONEPROTOCOL_H_

#include <avr/io.h>

#define STP_STARTBYTE 0xAA
#define STP_STOPBYTE  0x55

//initialization
void stoneProtocol_init();

//to be called with a newly received uart byte
//returns 1 if a full packet is received, 2 if there happened a error with the stopbyte, else 0
//in the newly received uart byte
uint8_t stoneProtocol_loop(uint8_t in);

//to retreive a message, saved in a dynamic byte array, terminated with \0 => string
//returns NULL if no message is to be retreived
uint8_t* stoneProtocol_getMessage();

//to transmit a message, one must pass an array containing the data in little endian format + lenth of the array
//data = data to be transmitted
//length = length of the data array
void stoneProtocol_transmit(uint8_t* data, uint8_t length);





#endif /* STONEPROTOCOL_H_ */