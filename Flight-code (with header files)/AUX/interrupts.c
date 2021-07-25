#include "interrupts.h"
#include "avr_compiler.h"

void interruptInit(void){
	EICRB |= (1<<ISC71)|(1<<ISC70);
	EIMSK |= (1<<INT7);
}