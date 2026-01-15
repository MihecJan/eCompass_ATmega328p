#ifndef TWI_SERVICE_H_
#define TWI_SERVICE_H_


#include <stdint.h>

void twi_init();
uint8_t twi_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t twi_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);


#endif /* TWI_SERVICE_H_ */