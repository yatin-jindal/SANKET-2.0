#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "adc.h"
#include "i2c.h"
#include "hmData.h"
#include "timer.h"
#include "uart.h"

volatile uint8_t recBuffer = 0x00;
volatile uint8_t hmDataADS[6];
volatile uint8_t i2cStatus = 0x00;
volatile int i2cTransCounter = 0;
volatile int i = 0;
volatile int collectionCount = 0;
volatile uint8_t currLimOC=0, currLimEN=0, DetSwtch1=0, DetSwtch2 = 0; 
volatile uint16_t ADCtemp = 0x0000;
volatile int timerCounter= 0;

int main(void){
	
	sei();
	USART0_INIT();
	adcInit();
	DDRA = 0x00;
	DDRB = 0xFF;
	DDRF = 0x00;
	i2cInit();
	while(1);
	return 0;
	
}
ISR(TIMER1_COMPA_vect){

	switch(timerCounter){
		case 0:
		hmDataCollect();
		timerCounter++;
		break;
		
		case 1:
		hmDataCollect();
		timerCounter++;
		break;
		
		case 2:
		PORTB = 0x00;
		timerCounter = 0;
		break;
	}
}
ISR(TWI_vect){
	i2cStatus = TWSR & 0xF8;
	
	switch(i2cStatus){
		case 0x60:
		//SLA_W received, ACK sent
		TWCR |= (1<<TWINT);	
		break;
		
		case 0x80 :
		//data has been received, ACK sent
		recBuffer = TWDR;
		//USART0_TRANS(recBuffer);
		TWCR |= (1<<TWINT);
		break;
		
		case 0xA8 :
		//SLA_R has been received; ACK sent
		if(recBuffer == 0x10){
			//send pre-defined byte as acknowledgement 
			TWDR = 0x0F;
			TWCR |= (1<<TWINT);	
		}
		else if(recBuffer == 0x02){
			//send acknowledgement of receiving burner circuit test command
			TWDR = 0x03;
			if(timerCounter == 0){
				//start burner circuit test
				PORTB |= (1<<PB4);
				setupTimer();
			}
			TWCR |= (1<<TWINT);	
			
		}
		break;
		
		case 0xB8 :
		//Data transmitted, ACK received
		
		while(timerCounter==collectionCount);
		collectionCount++;
		switch(i2cTransCounter){
			
			case 0:
			TWDR = hmDataADS[0];
			i2cTransCounter++;
			break;
			
			case 1:
			TWDR = hmDataADS[1];
			i2cTransCounter++;
			break;
			
			case 2:
			TWDR =hmDataADS[2];
			i2cTransCounter++;
			break;
			
			case 3:
			TWDR = hmDataADS[3];
			i2cTransCounter++;
			break;
			
			case 4:
			TWDR = hmDataADS[4];
			i2cTransCounter++;
			break;
			
			case 5:
			TWDR = hmDataADS[5];
			i2cTransCounter++;
			break;
			
			case 6:
			//sending pre-defined byte signifying end of HM Data
			TWDR = 0x05;
			i2cTransCounter = 0;
			
			break;
			
		}
		TWCR |= (1<<TWINT);	
		break;
		
		default :
		TWCR |= (1<<TWINT);
		break;
		}
		
		
}
