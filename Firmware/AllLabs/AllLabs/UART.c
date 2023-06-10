/*
 * UART0.c
 *
 * Created: 25/08/2022 1:07:57 pm
 *  Author: vnesh
 */ 

#include "UART.h"
#include "adc.h"

#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


void usart_init(uint8_t ubrr) {
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) (ubrr);
	UCSR0B |= (1 << TXEN0); // Enabling transmit
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Enables UART (async) mode, no parity, 1 stop bit, 8 data bits
}

void usart_transmit_array(char* msg) { // Transmitting array of characters
	for (uint8_t i = 0; i < strlen(msg); i++)
	usart_transmit(msg[i]); // transmits every character in string message
}

void usart_transmit(char byte) { // Function transmits one byte/ char
	while (!(UCSR0A & (1 << UDRE0))); // Checking if UDR0 is empty
	UDR0 = byte; // Putting data to be sent by UDR0 register
}

void transmit_Vrms(float Vrms) {
	char voltage[8];
	dtostrf(Vrms,-4,1,voltage); // convert float to string
	usart_transmit_array("RMS Voltage is: ");
	usart_transmit_array(voltage); // transmit to terminal
	usart_transmit_array("Vrms"); // units
	usart_transmit_array("\r\n");
}

void transmit_Irms(float Irms) {
	Irms *= 1000; // convert to mA
	char current[8];
	dtostrf(Irms,-3,0,current); // convert float to string, store in array
	usart_transmit_array("RMS Current is: ");
	usart_transmit_array(current); // transmit to terminal
	usart_transmit_array("mArms");
	usart_transmit_array("\r\n");
}

void transmit_power(float w) {
	char power[8];
	dtostrf(w,-3,2,power); // convert float to string, store in array
	usart_transmit_array("Power is: ");
	usart_transmit_array(power); // transmit to terminal
	usart_transmit('W');
	usart_transmit_array("\r\n");
}