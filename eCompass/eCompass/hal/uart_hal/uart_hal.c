#include "config.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart_hal.h"
#include "utils/led.h"
#include "utils/led.h"

static uart_rx_cb_t rx_cb;

ISR(USART_RX_vect)
{
    uint8_t c = UDR0;
	if (rx_cb)
	{
		rx_cb(c);
	}
	TOGGLE_LED();
}

void uart_hal_register_rx_callback(uart_rx_cb_t cb)
{
	rx_cb = cb;
}

/**
 * @brief Calculates the UBRR (USART Baud Rate Register) value for a given baud rate.
 * 
 * Calculates the best UBRR value to achieve the desired baud rate based on
 * the system's CPU frequency (F_CPU) in normal asynchronous mode (U2Xn = 0).
 * 
 * @param baud The target baud rate.
 * @return The 16-bit UBRR value that should be loaded into UBRR0H and UBRR0L registers.
 */
static uint16_t ubrr_from_baud(uint32_t baud)
{
    // Calculate UBRR value for normal mode (U2X0 = 0)
    //uint16_t ubrrFloor = (F_CPU / (16UL * baud)) - 1;
	
	uint16_t ubrrFloor = (F_CPU / (8UL * baud)) - 1;
    uint16_t ubrrCeil = ubrrFloor - 1;

    //uint32_t baudFromFloor  = F_CPU / (16 * (ubrrFloor + 1));
    //uint32_t baudFromCeil   = F_CPU / (16 * (ubrrCeil + 1));
	uint32_t baudFromFloor  = F_CPU / (8 * (ubrrFloor + 1));
	uint32_t baudFromCeil   = F_CPU / (8 * (ubrrCeil + 1));

    uint32_t errorFloor = (baud > baudFromFloor)
        ? baud - baudFromFloor
        : baudFromFloor - baud;
    uint32_t errorCeil  = (baud > baudFromCeil)
        ? baud - baudFromCeil
        : baudFromCeil  - baud;

    // Choose the UBRR with the smaller error
    uint16_t finalUbrr = (errorFloor <= errorCeil) ? ubrrFloor : ubrrCeil;

    return finalUbrr;
}

void HAL_uart_init(uint32_t baud)
{
	// Set U2X0 bit to enable Double Speed Mode
	UCSR0A = (1 << U2X0);
	  
    uint16_t ubrr = ubrr_from_baud(baud);

    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // Enable transmitter, receiver and receive interrupt
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // 8 data, 1 stop bit, no parity (8N1)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void HAL_uart_send_byte(uint8_t c)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Put data into buffer, sends the data
	UDR0 = c;
}