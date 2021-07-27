#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uartInit(void);
void uart_transmit(uint8_t data);

#endif
