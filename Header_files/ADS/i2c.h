#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define MASK 0xF8
#define REPEATED_START_STOP 0xA0
#define GEN_CALL_ACK 0x70
#define GEN_CALL_ARBIT_LOST_ACK 0x78
#define GEN_CALL_DATA_REC_ACK 0x90
#define GEN_CALL_DATA_REC_NACK 0x98
#define SLA_W_ACK 0x60
#define SLA_W_ARBIT_LOST_ACK 0x68
#define SLA_R_ACK 0xA8
#define SLA_R_NACK 0x48
#define DATA_SENT_ACK 0xB8
#define DATA_SENT_NACK 0xC0
#define LAST_BYTE_ACK 0xC8
#define DATA_REC_ACK 0x80
#define DATA_REC_NACK 0x88

void i2cInit(void);



#endif
