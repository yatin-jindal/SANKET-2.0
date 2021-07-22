  
#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t recBuffer = 0x00;
volatile uint8_t hmDataADS[6];
volatile uint8_t i2cStatus = 0x00;
volatile int i2cTransCounter = 0;
volatile int i = 0;
volatile int collectionCount = 0;
volatile uint8_t currLimOC=0, currLimEN=0, DetSwtch1=0, DetSwtch2 = 0; 
volatile uint16_t ADCtemp = 0x0000;
volatile int timerCounter= 0;
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
void i2cInit(void){
	TWAR = 0xA0;
	TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
	
}

void setupTimer(void){
	//putting the value to compare counter with; real time = 0.3s
	OCR1A = 0x1E85;
	TIMSK = (1<<OCIE1A);
	TCNT1 = 0;
	//selecting clock, enabling CTC mode (Clear Timer on Compare)
	TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12);
	
	
}


void adcInit(void){
	//enabling ADC, setting frequency pre-scaler to 16
	
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADPS2);
	ADMUX |= (1<<REFS0)|(1<<ADLAR);
	
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
	ADCtemp = ADC;
	hmDataADS[k] = ADCH;
	ADMUX = (1<<REFS0)|(1<<ADLAR);
	//ADCSRA &= ~(1<<ADEN);
	}

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
		PORTB |= 0x80;
		timerCounter++;
		break;
		
		case 1:
		hmDataCollect();
		PORTB |= 0x80;
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
		if (recBuffer == 0x02){
			//start burner circuit test
			PORTB |= (1<<PB4);
			setupTimer();
		}
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
			TWCR |= (1<<TWINT);	
			
		}
		break;
		
		case 0xB8 :
		//Data transmitted, ACK received
		
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
			PORTB = 0x10;
			break;
			
		}
		TWCR |= (1<<TWINT);	
		break;
		
		default :
		TWCR |= (1<<TWINT);
		break;
		}
		
		
}
