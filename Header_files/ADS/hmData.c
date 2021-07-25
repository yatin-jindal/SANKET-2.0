#include "adc.h"
#include "hmData.h"

extern uint8_t detSwtch1;
extern uint8_t detSwtch2;
extern uint8_t currLimOC;
extern uint8_t currLimEN;
extern uint8_t hmDataADS[6];

void hmDataCollect(void){
	detSwtch1 = PINA & (1<<PA3);    //0b00001000
	detSwtch2 = PINA & (1<<PA5);	//0b00100000
	currLimOC = PINA & (1<<PA7);	//0b10000000
	currLimEN = PINA &(1<<PA6);		//0b01000000
	hmDataADS[0] =(detSwtch1 >>3 )|(detSwtch2 >>3)|(currLimEN >>2)|(currLimOC >>1) ;

	adcMeasure( 0x00 , 1 );
	adcMeasure( 0x01 , 2 );
	adcMeasure( 0x05 , 3 );
	adcMeasure( 0x06 , 4 );
	adcMeasure( 0x07, 5 );
}
