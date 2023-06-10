
#ifndef _ADC_H
#define _ADC_H

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

void adc_init();
float adc_convert_v(uint16_t adc_result);
void convertAdcToVoltages(); // Converts values from ADC to actual values
void rmsCalculator();
void linearApproxPower();
#endif