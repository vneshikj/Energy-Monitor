/*
 * display.h
 *
 * Created: 23/09/2022 5:15:34 pm
 *  Author: vnesh
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

void init_display();
void send_next_character_to_display();
void seperate_and_load_voltage(float v);
void seperate_and_load_current(float i);
void seperate_and_load_power(float p);
#endif /* DISPLAY_H_ */