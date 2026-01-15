#ifndef UART_HAL_H_
#define UART_HAL_H_


#include <stdint.h>

typedef void (*uart_rx_cb_t)(uint8_t byte);

void uart_hal_register_rx_callback(uart_rx_cb_t cb);

/**
 * @brief Initializes the UART with specified baud rate.
 * 
 * Enables transmission,
 * reception,
 * the receive complete interrupt,
 * sets the data format (8N1)
 * 
 * @param baud The desired baud rate for UART communication.
 */
void HAL_uart_init(uint32_t baud);

/**
 * @brief Send byte over UART.
 * 
 * @param c Byte to send.
 */
void HAL_uart_send_byte(uint8_t c);

#endif /* UART_HAL_H_ */