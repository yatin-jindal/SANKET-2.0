#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <avr/io.h>

void adcInit(void);
void adcMeasure(uint8_t mux, int k);

#endif