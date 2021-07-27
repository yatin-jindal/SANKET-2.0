#define F_CPU 8000000
#define SLEEP 0b00000001
#define PREDEPL_INIT 0b00000010
#define PREDEPL_HM 0b00000100
#define DEPL 0b00001000
#define OUTPUT_COMPARE 0x3D09
#define SLA_W 0xA0
#define SLA_R 0xA1
#define COMMAND_I2C_CHECK 0x10
#define COMMAND_BURNER_CIRC_CHECK 0x02

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t hmData[18];
volatile uint8_t opMode;
volatile int timeCounter1;
volatile uint8_t i2cStatus = 0x00;
volatile uint8_t i2cRec = 0x00;
volatile int i2cRecCounter = 0;
volatile int uartTransCounter = 0;
volatile uint8_t recBuffer = 0x00;
volatile int waitHM= 1;
volatile int waitI2C = 1;
volatile int reqdHM = 0;
volatile int HMCount = 0;

void startTimer(void){
	//selecting clock, enabling CTC mode (Clear Timer on Compare)
	TCCR1B = (1<<CS12)|(1<<CS10);
	//putting the value to compare counter with
	OCR1C = 0x3D09;
	//enabling output compare interrupt
	ETIMSK = (1<<OCIE1C);
	TCNT1 = 0;
	
}

void i2cInit(void){
	//setting prescaler bits
	TWSR = (1<<TWPS1)|(1<<TWPS0);
	//bitrate
	TWBR = 0x20;
	//enabling twi
	TWCR = (1<<TWEN);
}
void i2cStart(void){
	//sending start condition
	TWCR = (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	

}
void i2cTransMode(void){
	//load slave address and write bit
	TWDR = SLA_W;
	//set flag
	TWCR |= (1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
}
void i2cRecMode(void){
	//load slave address and read bit
	TWDR = SLA_R;
	//set flag
	TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT);
	//while(!(TWCR&(1<<TWINT)));
	}
void i2cWrite(uint8_t data){
	TWDR = data;
	TWCR |= (1<<TWEN)|(1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
}
void i2cStop(void){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void uartInit(void){
	//setting baud rate to 9600
	UBRR0H=0x00;
	UBRR0L=0x33;
	//setting no. of data bits to 8
	UCSR0C=0x06;
	//enable receive complete interrupt,transmit complete interrupt, receiver enable, transmitter enable
	UCSR0B=0xC8;
}

void uart_transmit(uint8_t data)
{
  while(!(UCSR0A & 0x20));
	UDR0=data;	
}

void adcInit(void){
	//enabling ADC, setting frequency pre-scaler to 16
	ADCSRA = (1<<ADEN)|(1<<ADPS2);
}

void adcCheck(uint8_t mux, int i, uint8_t idealValue){
	/*Read the adc value
	*parameters
	*mux     analog channel
	*/
	//setting external voltage reference, left adjust result
	ADMUX = 0x20;
	ADMUX |= mux;
	//start conversion
	ADCSRA |= (1<<ADSC);
	//wait for conversion to finish
	while(!(ADCSRA & (1<<ADIF)));
	
	hmData[i] = ADCH;
	if(abs(hmData[i]-idealValue) > 0.5) opMode=EMERGENCY;
	}

void HM_data_check&send(void){
	hmData[0] = (seconds & 0x00FF);
	hmData[1] = (seconds>>8);
	hmData[2] = (Uplink_Rx<<7) | (CC_STATUS<<4) | opMode;   // the different HM data are collected and stored at different places 
	hmData[3] |= (PINA & 0x0F);                             // here the EN and OC of the current limiters are connected in the 4 LSB of Port A
	adcCheck(xx,4,ideal1);
	adcCheck(xx,5,ideal2);
	adcCheck(xx,6,ideal3);
  
  for(int j=0;j<18;j++){
  	uart_transmit(hmData[j]);
  }
}

void interruptInit(void){
	EICRB |= (1<<ISC71)|(1<<ISC70);
	EIMSK |= (1<<INT7);
}
int main(void){
	interruptInit();
	sei();
	uartInit();
	i2cInit();
	startTimer();
	adcInit();
	
	opMode=SLEEP;
	DDRA=0xFF;
	DDRB = 0xFF;
	DDRC = 0x00;
	while(1){
		switch(opMode){
			case SLEEP:
			PORTA = SLEEP;
			//microcontroller goes into sleep for 50 min
			if(timeCounter1 == 1){
				timeCounter1 = 0;
				opMode = PREDEPL_INIT;
				}
			break;
			
			case PREDEPL_INIT:
			//enable ADS current limiter
			PORTA = 0x01;
			//Send start condition, enter transmit mode
			i2cStart();
			i2cTransMode();
			//Transmit pre-defined byte to ADS uC
			i2cWrite(COMMAND_I2C_CHECK);
			i2cStop();
			//Repeated start, Enter Receive mode
			i2cStart();
			i2cRecMode();
			//wait for ADS to send all data
			while(waitI2C == 1);
			opMode = PREDEPL_HM;
			uart_transmit(0x55);
			break;
			
			case PREDEPL_HM:
			//send command to ADS to start burner circuit test
			i2cStart();
			i2cTransMode();
			i2cWrite(COMMAND_BURNER_CIRC_CHECK);
			i2cStop();
			//Receive HM data from AUX board
			reqdHM = 2;
			while(HMCount<reqdHM){
				while(waitHM==1);
				i2cStart();
				//Enable acknowledgement of received data
				//I2C ISR will be storing all data received in HM data array
				i2cRecCounter = 0;
				i2cRecMode();
				TWCR |= (1<<TWEA);
				while(waitI2C==1);
				HMCount++;
				waitHM = 1;
			}
			while(waitHM==1);
			//check HM data received
			//send HM data to computer
			HM_data_check&send();
			opMode = DEPL;
			break;
			
			case DEPL:
			PORTA = 0xFF;
			
			
		}
	}
	return 0;
}

ISR(INT7_vect){
	waitHM = 0;
}

ISR(TIMER1_COMPC_vect){
	
	timeCounter1++;
}

ISR(TWI_vect){
	i2cStatus = (TWSR & 0xF8);
	switch(i2cStatus){
		case 0x08 :
		//Start condition transmitted
		TWCR &= ~(1<<TWSTA);
		TWCR |= (1<<TWINT);
		
		break;
		
		case 0x18 :
		TWCR |= (1<<TWINT);
		//SLA_W transmitted, ACK received
		break;
		
		case 0x20 :
		TWCR |= (1<<TWINT);
		//SLA_W transmitted, NACK received
		break;
		
		case 0x28 :
		TWCR |= (1<<TWINT);
		//Data transmitted, ACK received
		break;
		
		case 0x30 :
		TWCR |= (1<<TWINT);
		//Data transmitted, NACK received
		break;
		
		case 0x10 :
		TWCR |= (1<<TWINT);
		//Repeated start transmitted
		break;
		
		case 0x40 :
		//SLA_R transmitted, ACK received
		TWCR |= (1<<TWINT);
		break;
		
		case 0x58:
		//Data received, NACK returned
		waitI2C = 0;
		recBuffer = TWDR;
		i2cStop();
		TWCR |= (1<<TWINT);
		
		case 0x50 :
		//Data received, ACK returned
		switch(i2cRecCounter){
			case 0 :
			i2cRec = TWDR;
			i2cRecCounter++;
			break;
			
			case 1 :
			hmData[10] = TWDR;
			i2cRecCounter++;
			break;
			
			case 2 :
			hmData[11] = TWDR;
			i2cRecCounter++;
			break;
			
			case 3 :
			hmData[12] = TWDR;
			i2cRecCounter++;
			break;
			
			case 4 :
			hmData[13] = TWDR;
			i2cRecCounter++;
			break;
			
			case 5 :
			hmData[14] = TWDR;
			i2cRecCounter++;
			break;
			
			case 6 :
			hmData[15] = TWDR;
			i2cRecCounter = 0;
			TWCR &= ~(1<<TWEA);
			break;
		}
		TWCR |= (1<<TWINT);
		break;
		
		default : 
		TWCR |= (1<<TWINT);
		break;
	}
}









