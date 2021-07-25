#include "avr_compiler.h"
#include "i2c.h"

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