/*
 * timer0.c
 *
 * Created: 21/09/2022 6:43:00 pm
 *  Author: vnesh
 */ 

#include "timers.h"
#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

volatile uint16_t timer_counts = 0;
volatile uint8_t elapsed = 0;
volatile uint8_t count = 0;

// Configuration generates interrupt every 0.1ms (For triggering ADC) 
void timer0_init() {
	TCCR0A |= (1 << WGM01); // Sets to CTC mode
	TCCR0B |= (1 << CS01); // Sets prescaler to 8
	OCR0A = 24; // Output compare match register set to 24 (25 counts)
	TIMSK0 |= (1 << OCIE0A); // Enables interrupt for compare match A
	
	// Timer counter 1 for display refresh EVERY 10ms
	TCCR1B |= (1 << WGM12) | (1 << CS12); // set to CTC mode (top of OCR1A) and set prescaler to 256 to get event every ~10ms 
	TCCR1B |= (1 << CS12); // Set prescaler to 256
	OCR1A = 45; // 78 clock cycles for 10ms
	TIMSK1 |= (1 << OCIE1A); // Enable interrupt for compare match A
}


ISR(TIMER0_COMPA_vect) { 
}

// Will go off every ~10ms to refresh display, and after ~1s, change value to be displayed
ISR(TIMER1_COMPA_vect) {
	send_next_character_to_display(); // refresh display with loaded value
	if(count == 100) {
		elapsed = 1; // after 1 second, load next value (voltage, current, power)
		count = 0; // reset count
	}
	count++;
}




