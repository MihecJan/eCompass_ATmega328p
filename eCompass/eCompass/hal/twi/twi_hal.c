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

#include "config.h"

#include <util/delay.h>

#include "twi_hal.h"


volatile uint8_t status = TWI_NO_STATE;

ISR(TWI_vect)
{
	status = (TWSR & 0xF8);
}

// Sends a TWI START condition and waits for it to complete.
static uint8_t twi_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	uint8_t i = 0;
	while (status != TWI_START)
	{
		i++;
		if (i >= TWI_TIMEOUT)
		{
			return TWI_ERROR_START;
		}
	}
		
	return TWI_OK;
}

static void twi_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
}

static uint8_t twi_RStart(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	uint16_t i = 0;
	while (status != TWI_RSTART)
	{
		i++;
		if (i >= TWI_TIMEOUT)
		{
			return TWI_ERROR_RSTART;
		}
	}
		
	return TWI_OK;
}

// Waits for ACK after transmitting SLA+W.
static uint8_t twi_wait_addr_write_ack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	
	uint16_t i = 0;
	while (status != TWI_T_ADDR_ACK)
	{
		i++;
		if (i >= TWI_TIMEOUT)
		{
			return TWI_ERROR_NACK;
		}
	}
		
	return TWI_OK;
}

// Waits for ACK after transmitting data.
static uint8_t twi_wait_data_write_ack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	
	uint16_t i = 0;
	while (status != TWI_T_DATA_ACK)
	{
		i++;
		if (i >= TWI_TIMEOUT)
		{
			return TWI_ERROR_NACK;
		}
	}
		
	return TWI_OK;
}

// Waits for ACK after transmitting SLA+R.
static uint8_t twi_wait_addr_read_ack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	
	uint16_t i = 0;
	while (status != TWI_R_ADDR_ACK)
	{
		i++;
		if (i >= TWI_TIMEOUT)
		{
			return TWI_ERROR_NACK;
		}
	}
		
	return TWI_OK;
}

// Optionally sends an ACK after receiving data byte
static uint8_t twi_wait_data_read_ack(uint8_t ack)
{
	uint8_t expected_status;
	
	// Configure TWCR based on ack option
	if (ack)
	{
		// Sends ACK
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		expected_status = TWI_R_DATA_ACK;
	}
	else
	{
		// Doesn't send ACK
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		expected_status = TWI_R_DATA_NACK;
	}
	
	// Wait for expected status
	uint16_t i = 0;
	while (status != expected_status)
	{
		if (++i >= TWI_TIMEOUT)
		{
			return TWI_ERROR;
		}
	}
		
	return TWI_OK;
}

uint8_t HAL_twi_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
	uint8_t err = TWI_OK;
	
	// Start
	err = twi_start();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// SLA+W, wait ACK
	TWDR = (addr << 1) | 0;
	err = twi_wait_addr_write_ack();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// Subaddr, wait ACK
	TWDR = reg;
	err = twi_wait_data_write_ack();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// Repeated start
	err = twi_RStart();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// SLA+R, wait ACK
	TWDR = (addr << 1) | 1;
	err = twi_wait_addr_read_ack();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// Send ACK for all bytes except last one
	uint16_t i;
	for (i = 0; i < (len - 1); i++)
	{
		err = twi_wait_data_read_ack(1);
		if (err != TWI_OK)
		{
			twi_stop();
			return err;
		}
		data[i] = TWDR;
	}
	
	// last byte, don't send ACK
	err = twi_wait_data_read_ack(0);
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	data[i] = TWDR;
	
	// Stop
	twi_stop();
	
	_delay_us(100);
		
	return TWI_OK;
}

uint8_t HAL_twi_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{	
	uint8_t err = TWI_OK;
	
	// Start
	err = twi_start();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// SLA+W, wait ACK
	TWDR = (addr << 1) | 0;
	err = twi_wait_addr_write_ack();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// Subaddr, wait ACK
	TWDR = reg;
	err = twi_wait_data_write_ack();
	if (err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	// Send data bytes, each byte wait for ACK
	for (uint16_t i = 0; i < len; i++)
	{
		TWDR = data[i];
		err = twi_wait_data_write_ack();
		if (err != TWI_OK)
		{
			twi_stop();
			return err;
		}
	}
	
	// Stop
	twi_stop();
	
	_delay_us(100);
		
	return TWI_OK;
}

void HAL_twi_init(uint32_t fSCL)
{
	// prescaler = 1
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
	
	TWBR = (((F_CPU / fSCL) - 16) / 2);
	
	// Enable TWI HW and TWI interrupt
	TWCR = (1 << TWEN) | (1 << TWIE);
}