/*]ki=
 * UART Implementation.c
 *
 * Created: 22/08/2022 9:19:43 pm
 * Author : vnesh
 */ 

// Including libraries

#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Defining parameters
#define  RMSVotlage 14.5
#define  RMSCurrent 125
#define  Power 1.60

void usart_init(uint8_t ubrr);
void usart_transmit(char byte);
void usart_transmit_array(char* msg);

// Initializing UART
void usart_init(uint8_t ubrr) {
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) (ubrr); 
	UCSR0B |= (1 << TXEN0); // Enabling transmit, and setting to 8 data bits
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Enables UART (async) mode, no parity, 1 stop bit, 8 data bits
	
	
}

void usart_transmit_array(char* msg) { // Transmitting array of characters
	for (uint8_t i = 0; i < strlen(msg); i++) 
	usart_transmit(msg[i]);
} 


void usart_transmit(char byte) {
	while (!(UCSR0A & (1 << UDRE0))); // Checking if UDR0 is empty
	UDR0 = byte; // Putting data to be sent by UDR0 register
	
}

int main(void)
{
	usart_init(12); // Setting baud rate
	char voltage[8];
	char current[8];
	char power[8];
	
    while (1) 
    {
		usart_transmit_array("RMS voltage is: "); 
		dtostrf(RMSVotlage, 3, 1, voltage); // Converting float to string	
		usart_transmit_array(voltage); // Transmitting string
		usart_transmit_array("\r"); // Printing new line
		
		usart_transmit_array("RMS current is: ");
		sprintf(current, "%d", RMSCurrent); // Converting integer to string
		usart_transmit_array(current);
		usart_transmit_array("\r");
		
		usart_transmit_array("Power is: ");
		dtostrf(Power, 1, 2, power);
		usart_transmit_array(power);
		usart_transmit_array("\r");
		
		usart_transmit_array("\r");
		_delay_ms(1000);
    }
}

