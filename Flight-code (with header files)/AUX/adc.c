#include "adc.h"
#include "avr_compiler.h"

extern uint8_t hmData[18];

void adcInit(void){
	//enabling ADC, setting frequency pre-scaler to 16
	ADCSRA = (1<<ADEN)|(1<<ADPS2);
}

void adcMeasure(uint8_t mux, int k){
	/*Read the adc value
	*parameters
	*mux     analog channel
	*/
	
	ADMUX |= mux;
	//start conversion
	ADCSRA |= (1<<ADSC);
	//wait for conversion to finish
	while(!(ADCSRA & (1<<ADIF)));
	//ADCtemp = ADC;
	hmData[k] = ADCH;
	ADMUX = (1<<REFS0)|(1<<ADLAR);
	//ADCSRA &= ~(1<<ADEN);
	}