#include "timer.h"
#include "avr_compiler.h"

void startTimer(void){
	//selecting clock, enabling CTC mode (Clear Timer on Compare)
	TCCR1B = (1<<CS12)|(1<<CS10);
	//putting the value to compare counter with
	OCR1C = 0x3D09;
	//enabling output compare interrupt
	ETIMSK = (1<<OCIE1C);
	TCNT1 = 0;
	
}