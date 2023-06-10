/*
 * timer0.h
 *
 * Created: 21/09/2022 6:47:11 pm
 *  Author: vnesh
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/io.h>
#include <stdint.h> 
#include <stdbool.h>
#include <avr/interrupt.h>

volatile uint8_t counter;
volatile uint16_t timer_counts;
volatile bool measured;

void timer0_init();

#endif /* TIMER0_H_ */