/*
Copyright (c) 2021 Devilbinder (Binder Tronics) https://bindertronics.com/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

// Modified by Miha Jan, 2025

#ifndef TWI_HAL_H_
#define TWI_HAL_H_

#include "config.h"

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TWI_TIMEOUT				(F_CPU / 10000)

// Status codes for master transmitter mode (TWSR)
#define TWI_START				0x08
#define TWI_RSTART				0x10

#define TWI_T_ADDR_ACK			0x18
#define TWI_T_ADDR_NACK			0x20
#define TWI_T_DATA_ACK			0x28
#define TWI_T_DATA_NACK			0x30

#define TWI_ARBITRATION_LOST	0x38

#define TWI_R_ADDR_ACK			0x40
#define TWI_R_ADDR_NACK			0x48
#define TWI_R_DATA_ACK			0x50
#define TWI_R_DATA_NACK			0x58

// Miscellaneous states
#define TWI_NO_STATE			0xF8
#define TWI_BUS_ERROR			0x00

// Custom
enum
{
	TWI_OK,
	TWI_ERROR_START,
	TWI_ERROR_RSTART,
	TWI_ERROR_NACK,
	TWI_ERROR
};

void HAL_twi_init(uint32_t fSCL);
uint8_t HAL_twi_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t HAL_twi_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);


#endif /* TWI_HAL_H_ */