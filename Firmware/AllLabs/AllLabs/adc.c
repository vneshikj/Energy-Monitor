/*
 * adc.c
 *
 * Created: 14/09/2022 1:38:22 pm
 *  Author: vnesh
 */ 

//#include "common.h"
#include "adc.h"
#include <math.h>
#include <stdbool.h>

volatile uint16_t voltage[80];
volatile uint16_t current[80];
volatile uint8_t pos = 0;
volatile bool calc = false;

volatile float convVoltage[80]; // Actual values from ADC for voltage and current
volatile float convCurrent[80];
volatile float power = 0;
volatile float Vrms = 0;
volatile float Irms = 0;

// Stores values after ADC conversion is completed and switches channels
ISR(ADC_vect) { 
	
	if(ADMUX & (1 << 0)) { // If currently reading voltage
		ADMUX = 
		current[pos] = ADC;
	} else { // If currently reading current, voltage conversion has completed
		voltage[pos] = ADC;
		pos++; // Increment position
	}
	
	if(pos > 79) { // When all samples are taken
		ADCSRA &= (1 << ADATE) | (1 << ADIE); // Disable free running mode and ADC interrupts
		calc = true; // Start calculation in main
	}
 	ADMUX ^= (1 << MUX0); // Toggles to read between current and voltage channels on each read
}

ISR(INT0_vect) {
	
	if(ADCSRA & (1 << ADSC)) { // If conversion in process, exit the ISR
		return;
	}
	
	ADCSRA |= (1 << ADIE); // Enables ADC complete interrupt
	ADMUX |= (1 << 1); // Set to read current first
	ADCSRB = 0; // Sets ADC to free running mode	
	 
}


// Initializes ADC
void adc_init() {
	ADMUX |= (1 << REFS0); // Sets AVCC, reads from ADC0 initially (voltage waveform)
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADATE) | (1 << ADSC); // Enable ADC, set prescaler to 16, enable ADC interrupt, enable auto trigger, start conversion
	ADCSRB |= (1 << ADTS1); // Sets trigger source to INT0
	EIMSK |= (1 << INT0); // Enable interrupt on INT0
	EICRA |= (1 << ISC01) | (1 << ISC00); // Set rising edge to trigger interrupt
}

// Converts ADC to processed values
float adc_convert_v(uint16_t adc_result) {
	return ((float) adc_result * 5) / 1023; 
}

// Converts values from ADC to actual values
void convertAdcToVoltages() {
	for (uint8_t i = 0; i < 80; i++) {
		convVoltage[i] = (adc_convert_v(voltage[i]) - 2.1) * 21.8333; // Taking into account voltage divider and 2.1V offset
		convCurrent[i] = (adc_convert_v(current[i]) - 2.1) / 0.9984; // taking into account 0.56 Ohm shunt, OPAMP and 2.1V offset
	}
}

void rmsCalculator() { // (Riemann Sum repeated)
	for(uint8_t i = 0; i < 80; i++) {
		Vrms += pow(convVoltage[i], 2); // Taking the sum of all values, squaring them (Riemann Sum)
		Irms += pow(convCurrent[i], 2);
	}
	Vrms = sqrt(Vrms / 80); // Diving by number of samples taken
	Irms = sqrt(Irms / 80);
}


void linearApproxPower() {
	float approxVoltage;
	float approxCurrent;
	
	for (uint8_t i = 1; i < 79; i++) { // Using linear approximation to estimate missing data points
		approxVoltage = (convVoltage[i] + convVoltage[i + 1]) / 2;
		approxCurrent = (convCurrent[i - 1] + convCurrent[i]) / 2;
		
		power += convVoltage[i] * approxCurrent + approxVoltage * convCurrent[i];
	}
	power /= 130;
}
