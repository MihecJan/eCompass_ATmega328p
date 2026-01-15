#include <avr/io.h>

#include "spi_hal.h"

void spi_init(void)
{
	//// Set MOSI, SCK, and CS as output
	//DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2);
	//
	//// Enable SPI, Master, set clock rate fck/128
	//SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}