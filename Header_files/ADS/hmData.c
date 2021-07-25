#include "adc.h"
#include "hmData.h"

extern uint8_t detSwtch1 = 0;
extern uint8_t detSwtch2 = 0;
extern uint8_t currLimOC = 0;
extern uint8_t currLimEN = 0;
extern hmDataADS[6];

void hmDataCollect(void){
	DetSwtch1 = PINA & (1<<PA3);    //0b00001000
	DetSwtch2 = PINA & (1<<PA5);	//0b00100000
	currLimOC = PINA & (1<<PA7);	//0b10000000
	currLimEN = PINA &(1<<PA6);		//0b01000000
	hmDataADS[0] =(DetSwtch1 >>3 )|(DetSwtch2 >>3)|(currLimEN >>2)|(currLimOC >>1) ;

	adcMeasure( 0x00 , 1 );
	adcMeasure( 0x01 , 2 );
	adcMeasure( 0x05 , 3 );
	adcMeasure( 0x06 , 4 );
	adcMeasure( 0x07, 5 );
}
