#include "twi_service.h"
#include "hal/twi/twi_hal.h"

void twi_init(uint32_t baud)
{
	HAL_twi_init(baud);
}

uint8_t twi_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
	return HAL_twi_write(addr, reg, data, len);
}

uint8_t twi_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
	return HAL_twi_read(addr, reg, data, len);
}