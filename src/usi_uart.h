#ifndef USI_UART_H
#define USI_UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define RX_BUFFER_SIZE 64

void usi_uart_init(void);
uint8_t usi_uart_available(void);
uint8_t usi_uart_getc(void);

#endif
