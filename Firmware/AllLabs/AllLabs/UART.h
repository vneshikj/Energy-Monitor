/*
 * UART0.h
 *
 * Created: 25/08/2022 1:11:18 pm
 *  Author: vnesh
 */ 


#ifndef UART0_H_
#define UART0_H_

#include <stdint.h>
void usart_init(uint8_t ubrr);
void usart_transmit_array(char* msg);
void usart_transmit(char byte);
void transmit_value(uint16_t value);
void transmit_Vrms(float value);
void transmit_Irms(float value);
void transmit_power(float w);
#endif /* UART0_H_ */