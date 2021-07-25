#include "i2c.h"

void i2cInit(void){
	TWAR = 0xA0;
	TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
	
}
