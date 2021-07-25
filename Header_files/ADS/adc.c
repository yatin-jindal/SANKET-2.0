#include "adc.h"

extern uint8_t hmData[6]
void adcInit(void){
	//enabling ADC, setting frequency pre-scaler to 16
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADPS2);
  //setting reference voltage and left adjust bit
	ADMUX |= (1<<REFS0)|(1<<ADLAR);
	}

void adcMeasure(uint8_t mux, int k){
	/*Read the ADC value and store in HM data array
  *
	*Parameters
	*mux     analog channel
  *k       HM data array index
  *
  *Return Value 
  *Void
	*/
	
	ADMUX |= mux;
	//start conversion
	ADCSRA |= (1<<ADSC);
	//wait for conversion to finish
	while(!(ADCSRA & (1<<ADIF)));
  //read the 10 bit digital output
	ADCtemp = ADC;
  //store the reuqired value in hm data array
	hmDataADS[k] = ADCH;
  //clear the ADMUX register for next conversion
	ADMUX = (1<<REFS0)|(1<<ADLAR);
	}
