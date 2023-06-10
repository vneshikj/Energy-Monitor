/*
 * display.c
 *
 * Created: 23/09/2022 5:14:17 pm
 *  Author: vnesh
 */ 

#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define DIGITS_OFF PORTD |= (1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD4)
#define SH_ST_ON PORTC |= (1 << PORTC5)
#define SH_ST_OFF PORTC &= ~(1 << PORTC5)

const uint8_t seg_pattern[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
static volatile uint8_t disp_characters[4] = {0, 0, 0, 0}; // stores characters to be displayed
volatile uint8_t disp_position = 0; // determines which display to enable

void init_display() {
	DDRC |= (1 << PORTC3) | (1 << PORTC4) | (1 << PORTC5); // Sets SH_CP, SH_DS, SH_ST as output
	DDRD |= (1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD4); // Setting Ds4, Ds3, Ds2, Ds1 as output
	DIGITS_OFF; // initialize digits to be off
}


void seperate_and_load_voltage(float v) {
	cli();
	uint16_t voltage = v * 10; // move everything left of decimal to separate digits
	disp_characters[0] = seg_pattern[(voltage / 100) % 10]; // assign digit to disp_characters
	disp_characters[1] = seg_pattern[(voltage / 10) % 10]; 
	disp_characters[1] |= (1 << 7); // set first bit to high, to enable decimal point
	disp_characters[2] = seg_pattern[voltage % 10]; // decimal point here
	disp_characters[3] = 0b00111110; // unit for voltage (volts)
	sei();
}

void seperate_and_load_current(float i) {
	cli();
	uint16_t current = i * 10000; // move everything left of decimal to separate digits
	disp_characters[0] = seg_pattern[current/1000]; // hundreds
	disp_characters[1] = seg_pattern[(current/100)%10]; // tens
	disp_characters[2] = seg_pattern[(current/10)%10]; // ones
	disp_characters[3] = 0b01110111; // unit for current (amps)
	sei();
}


void seperate_and_load_power(float p) {
	cli();
	uint16_t powerDigits = p * 100; // move everything left of decimal to separate digits
	disp_characters[0] = seg_pattern[(powerDigits/100)]; // decimal point here
	disp_characters[0] |= (1 << 7); // set first bit to high, to enable decimal point 
	disp_characters[1] = seg_pattern[(powerDigits/10) % 10]; 
	disp_characters[2] = seg_pattern[(powerDigits % 10)]; 
	disp_characters[3] = 0b01110011; // p, power symbol
	sei();
}

void send_next_character_to_display() {
	for(int8_t i = 7; i >= 0; i--) {
		PORTC &= ~(1 << PORTC3); // Toggle SH_CP OFF
		if (disp_characters[disp_position] & (1 << i)) { // Checks if the ith bit of the pattern is 1
			PORTC |= (1 << PORTC4); // Set SH_DS to 1
			} else {
			PORTC &= ~(1 << PORTC4); // Otherwise ith bit must be 0 so set SH_DS to 0
		}
		PORTC |= (1 << PORTC3); // Toggle SH_CP ON
	} // All bits clocked here 
	DIGITS_OFF;
	SH_ST_ON; // Toggle SH_ST ON
	SH_ST_OFF; // Toggle SH_ST OFF
	PORTD &= ~(1 << (disp_position + 4)); // Digit 4 for position 3
	disp_position++; 
	if (disp_position > 3) disp_position = 0; // increment disp_position, or reset it if it reaches 3
}

