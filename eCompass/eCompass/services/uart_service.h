#ifndef UART_SERVICE_H_
#define UART_SERVICE_H_


#include <stdint.h>

/*
This owns the software RX ring buffer and
is bridge between ISR and application
*/

// This must be power of 2!
#define RX_BUF_SIZE 64

void uart_init(uint32_t baud);

void uart_rx_init(void);
void uart_rx_push(uint8_t c);

uint8_t uart_rx_available(void);
uint8_t uart_rx_peek(uint8_t offset);
uint8_t uart_rx_pop(uint8_t *out);

void uart_send_string(const char *c);
void uart_send_byte(uint8_t c);


#endif /* UART_RX_SERVICE_H_ */