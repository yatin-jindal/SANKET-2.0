#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define SLA_W 0xA0
#define SLA_R 0xA1

void i2cInit(void);
void i2cStart(void);
void i2cTransMode(void);
void i2cRecMode(void);
void i2cWrite(uint8_t data);
void i2cStop(void);


#endif