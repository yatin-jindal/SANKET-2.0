#include "timer.h"

void setupTimer(void){
	//putting the value to compare counter with; real time = 0.3s
	OCR1A = 0x1E85;
	TIMSK = (1<<OCIE1A);
	TCNT1 = 0;
	//selecting clock, enabling CTC mode (Clear Timer on Compare)
	TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12);
	
	
}
