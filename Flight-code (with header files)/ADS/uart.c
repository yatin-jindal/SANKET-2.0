#include "uart.h"

void USART0_INIT(void){
	/*baud rate*/
	UBRR0H = 0x00;
	UBRR0L = 0x33;
	UCSR0C = 0x06;
	/*enabling receiver and transmitter*/
	UCSR0B = 0x18;
	/*setting frame format*/
}

void USART0_TRANS(uint8_t data){
	while(!(UCSR0A & 0x20));
	UDR0 = data;
}
