/*
 * AllLabs.c
 *
 * Created: 21/09/2022 6:34:42 pm
 * Author : vnesh
 */ 

#include "timers.h"
#include "UART.h"
#include "adc.h"
#include "display.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define F_CPU 2000000UL

extern volatile uint16_t voltage[80]; 
extern volatile uint16_t current[80]; // ADC values for voltage and current
extern volatile uint8_t pos;
extern volatile bool elapsed; // for display interrupt
extern volatile float Vrms;
extern volatile float Irms;
extern volatile float power; // store Vrms, Irms and power
int8_t iteration = 0; // to choose to send Vrms, Irms or power to display
extern volatile bool calc;

void ddr_init() {
	DDRC &= ~((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2)); // Set as input: Vs, Is, DCs (DC Offset)
	DDRD &= ~(1 << PORTD2); // Set as input voltage zero crossing detector
	DDRB |= (1 << PORTB5); // Setting LED as output for debugging
}

int main(void)
{
	ddr_init(); 
	timer0_init(); 
	adc_init(); 
	usart_init(12); 
	init_display(); 
	sei(); 
	elapsed = false;
	
	while(1) {
		
		if (calc) { // Once all 80 samples are taken, we can stop taking samples from ADC
			
			TCCR0B &= ~(1 << CS01); // stop timer0
			TCNT0 = 0; // reset timer counter 0
			convertAdcToVoltages(); // convert from ADC values to voltage
			rmsCalculator(); // Calculate Vrms and Irms
			linearApproxPower(); // perform power calculation
			transmit_Vrms(Vrms);
			transmit_Irms(Irms);
			transmit_power(power); 
			usart_transmit_array("\r\n");
			pos = 0; // Reset position for ADC
			calc = false; // Reset 
			ADCSRA |= (1 << ADIE);
			
		}
		// Load values to display
		if(elapsed) {
			TCCR0B |= (1 << CS01); // start timer0
			switch(iteration) {
				case 0:
				PORTB |= (1 << PINB5);
				seperate_and_load_voltage(Vrms); // load voltage value to be displayed
				break;
				case 1:
				seperate_and_load_current(Irms); // load current value to be displayed
				break;
				case 2:
				seperate_and_load_power(power); // load power value to be displayed
				iteration = -1; // reset iteration to display voltage after power
				break;
			}
			iteration++; // increment iteration
			elapsed = false; // reset elapsed
		}
	}
}

