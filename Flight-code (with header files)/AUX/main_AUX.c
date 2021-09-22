/*
 * Flight_Code_New.c
 *
 * Created: 25-07-2021 15:44:29
 * Author : HP
 */ 

#include "avr_compiler.h"
#include "definitions.h"
#include "timer.h"
#include "i2c.h"
#include "uart.h"
#include "interrupts.h"
#include "adc.h"
#include "hmdata.h"

volatile uint8_t hmData[18];
volatile uint8_t opMode;
volatile uint16_t timeCounter1;
volatile uint8_t i2cStatus = 0x00;
volatile uint8_t i2cRec = 0x00;
volatile int i2cRecCounter = 0;
volatile int uartTransCounter = 0;
volatile uint8_t recBuffer = 0x00;
volatile int waitHM= 1;
volatile int waitI2C = 1;
volatile int reqdHM = 0;
volatile int HMCount = 0;
volatile uint8_t Uplink_Rx;
volatile uint8_t CC_status;

int main(void){
	interruptInit();
	sei();
	uartInit();
	i2cInit();
	startTimer();
	
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
			HM_data_check_send();
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
