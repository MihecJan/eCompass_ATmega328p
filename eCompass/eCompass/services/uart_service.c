#include "uart_service.h"
#include "hal/uart_hal/uart_hal.h"
#include "utils/ring_buffer.h"

/*
This owns the software RX ring buffer and
is bridge between ISR and application
*/

static uint8_t rx_storage[RX_BUF_SIZE];
static ring_buffer_t rx_rb;

void uart_init(uint32_t baud)
{
	HAL_uart_init(baud);
}

void uart_rx_init(void)
{
	uart_hal_register_rx_callback(uart_rx_push);
	rb_init(&rx_rb, rx_storage, RX_BUF_SIZE);
}

void uart_rx_push(uint8_t c)
{
	rb_push_overwrite(&rx_rb, c);
}

uint8_t uart_rx_available(void)
{
    return rb_available(&rx_rb);
}

uint8_t uart_rx_peek(uint8_t offset)
{
	return rb_peek(&rx_rb, offset);
}

uint8_t uart_rx_pop(uint8_t *out)
{
	return rb_pop(&rx_rb, out);
}

void uart_send_string(const char *c)
{
	uint16_t i = 0;
	
	while (c[i] != '\0')
	{
		uart_send_byte(c[i]);
		i++;
	}
}

void uart_send_byte(uint8_t c)
{
	HAL_uart_send_byte(c);
}