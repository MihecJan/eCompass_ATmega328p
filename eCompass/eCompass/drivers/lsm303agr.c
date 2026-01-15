#include "../config.h"

#include <util/delay.h>

#include "lsm303agr.h"
#include "hal/twi/twi_hal.h"

static const uint8_t mag_odr_hz_lut[4] = {
	10,   // MAG_ODR_10_HZ
	20,   // MAG_ODR_20_HZ
	50,   // MAG_ODR_50_HZ
	100   // MAG_ODR_100_HZ
};

static const char *mag_mode_lut[4] = {
	"Continuous",
	"Single",
	"Idle",
	"Idle"
};

static const uint16_t acc_odr_hz_lut[10] = {
	0,
	1,
	10,
	25,
	50,
	100,
	200,
	400,
	1620,
	1344	// HR / Normal (1.344 kHz)
};

static const uint8_t acc_range_g_lut[4] = {
	2,
	4,
	8,
	16
};

uint8_t LSM303AGR_init(uint8_t odr)
{
	uint8_t err;
	
	// ODR, default: 10 Hz
	uint8_t odr1_val = 0;
	uint8_t odr0_val = 0;
	
	if (odr == MAG_ODR_20_HZ)
	{
		odr0_val = 1;
	}
	else if (odr == MAG_ODR_50_HZ)
	{
		odr1_val = 1;
	}
	else if (odr == MAG_ODR_100_HZ)
	{
		odr1_val = 1;
		odr0_val = 1;
	}
	
	// MAGNETOMETER
	uint8_t cfga =
		(0 << COMP_TEMP_EN) |
		(0 << REBOOT) |
		(0 << SOFT_RST) |
		(0 << LP) |
		(odr1_val << ODR1) |
		(odr0_val << ODR0) |
		(0 << MD1) |
		(0 << MD0);
	err = HAL_twi_write(LSM303AGR_MAG_ADDR, CFG_REG_A_M, &cfga, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	_delay_ms(100);
		
	uint8_t cfgb;
	HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_B_M, &cfgb, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	_delay_ms(100);
			
	cfgb |= (1 << OFF_CANC);
	cfgb |= (1 << LPF);
	HAL_twi_write(LSM303AGR_MAG_ADDR, CFG_REG_B_M, &cfgb, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	_delay_ms(100);
						
	// ACCELEROMETER
	uint8_t ctrl1 = (1 << ODR2) | (1 << ODR0) | (1 << ZEN) | (1 << YEN) | (1 << XEN);
	err = HAL_twi_write(LSM303AGR_ACC_ADDR, CTRL_REG1_A, &ctrl1, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	_delay_ms(100);
				
	// High resolution
	uint8_t ctrl4 = (1 << HR);
	err = HAL_twi_write(LSM303AGR_ACC_ADDR, CTRL_REG4_A, &ctrl4, 1);
		
	if (err != TWI_OK)
	{
		return err;
	}
	
	_delay_ms(100);
			
	return 0;
}

uint8_t LSM303AGR_get_odr_mag_hz(uint8_t *odr_hz)
{
	uint8_t cfga;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_A_M, &cfga, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t odr_code = (cfga >> 2) & 0b00000011;
	
	*odr_hz = mag_odr_hz_lut[odr_code];
	
	return 0;
}

uint8_t LSM303AGR_get_mode_mag(char **mode)
{
	uint8_t cfga;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_A_M, &cfga, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t mode_code = cfga & 0b00000011;
	
	*mode = (char *)mag_mode_lut[mode_code];
	
	return 0;
}

uint8_t LSM303AGR_get_power_mode_mag(char **pwr_mode)
{
	uint8_t cfga;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_A_M, &cfga, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t pwr_mode_code = (cfga >> 4) & 0x01;
	
	*pwr_mode = pwr_mode_code == 0 ? "High resolution" : "Low power";
	
	return 0;
}

uint8_t LSM303AGR_get_lowpass_filter_mag(uint8_t *lp_filter)
{
	uint8_t cfgb;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_B_M, &cfgb, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t lp_filter_code = cfgb & 0x01;
	
	*lp_filter = lp_filter_code;
	
	return 0;
}

uint8_t LSM303AGR_get_offset_canc_mag(uint8_t *offset_canc)
{
	uint8_t cfgb;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_MAG_ADDR, CFG_REG_B_M, &cfgb, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t offset_canc_code = (cfgb >> 1) & 0x01;
	
	*offset_canc= offset_canc_code;
	
	return 0;
}


uint8_t LSM303AGR_get_odr_acc_hz(uint16_t *odr_hz)
{
	uint8_t reg1;
	uint8_t err;
	
	err = HAL_twi_read(LSM303AGR_ACC_ADDR, CTRL_REG1_A, &reg1, 1);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	uint8_t odr_code = (reg1 >> 4) & 0b00000111;
	
	*odr_hz = acc_odr_hz_lut[odr_code];
	
	return 0;
}

uint8_t LSM303AGR_get_power_mode_acc(char **pwr_mode)
{
		uint8_t reg1;
		uint8_t reg4;
		uint8_t err;
				
		err = HAL_twi_read(LSM303AGR_ACC_ADDR, CTRL_REG1_A, &reg1, 1);
		
		if (err != TWI_OK)
		{
			return err;
		}
				
		err = HAL_twi_read(LSM303AGR_ACC_ADDR, CTRL_REG4_A, &reg4, 1);
		
		if (err != TWI_OK)
		{
			return err;
		}
			
		uint8_t pwr_mode_code = (((reg1 >> LPEN) & 0x01) << 1) | ((reg4 >> HR) & 0x01);
		
		switch (pwr_mode_code)
		{
			case 0x02: *pwr_mode = "Low-power"; break;
			case 0x00: *pwr_mode = "Normal"; break;
			case 0x01: *pwr_mode = "High-resolution"; break;
			default: *pwr_mode = "Not allowed"; 
		}
				
		return 0;
}

uint8_t LSM303AGR_get_range_acc(uint8_t *measurement_range_g)
{
	uint8_t reg4;
	uint8_t err;
		
	err = HAL_twi_read(LSM303AGR_ACC_ADDR, CTRL_REG4_A, &reg4, 1);
		
	if (err != TWI_OK)
	{
		return err;
	}
		
	uint8_t measurement_range_code = (reg4 >> 4) & 0b00000011;
		
	*measurement_range_g = acc_range_g_lut[measurement_range_code];
		
	return 0;
}

uint8_t LSM303AGR_read_acc(int16_t data[3])
{
	// X_L, X_H, Y_L, Y_H, Z_L, Z_H
	uint8_t acc[6];
	uint8_t err = HAL_twi_read(LSM303AGR_ACC_ADDR, OUT_X_L_A | 0x80, acc, 6);
	
	if (err != TWI_OK)
	{
		return err;
	}

	/* 
	 * Combine High and Low bytes into a 16-bit signed integer.
	 * Then shift right by 4 to align the 12-bit left justified data.
	 * Shifting a signed int16_t right will preserve the sign (arithmetic shift).
	 */
    data[0] = ((int16_t)((acc[1] << 8) | acc[0])) >> 4; // X
    data[1] = ((int16_t)((acc[3] << 8) | acc[2])) >> 4; // Y
    data[2] = ((int16_t)((acc[5] << 8) | acc[4])) >> 4; // Z
		
	return TWI_OK;
}

uint8_t LSM303AGR_read_acc_unshifted(int16_t data[3])
{
	// X_L, X_H, Y_L, Y_H, Z_L, Z_H
	uint8_t acc[6];
	uint8_t err = HAL_twi_read(LSM303AGR_ACC_ADDR, OUT_X_L_A | 0x80, acc, 6);
	
	if (err != TWI_OK)
	{
		return err;
	}

	/* Combine High and Low bytes into a 16-bit signed integer. */
    data[0] = ((int16_t)((acc[1] << 8) | acc[0])); // X
    data[1] = ((int16_t)((acc[3] << 8) | acc[2])); // Y
    data[2] = ((int16_t)((acc[5] << 8) | acc[4])); // Z
		
	return TWI_OK;
}

// HR mode, ±2g: 0.98 mg / LSB
int32_t lsm303agrAccRawToMg(int16_t raw)
{
	// raw data is 12 bits shifted to left
	raw >>= 4;

	// Conversion to mg
	return ((int32_t)raw * 98) / 100;
}

uint8_t LSM303AGR_read_mag(int16_t data[3])
{
	// X_L, X_H, Y_L, Y_H, Z_L, Z_H
	uint8_t mag[6];
	uint8_t err = HAL_twi_read(LSM303AGR_MAG_ADDR, OUTX_L_REG_M | 0x80, mag, 6);
	
	if (err != TWI_OK)
	{
		return err;
	}
	
	data[0] = (int16_t)((mag[1] << 8) | mag[0]); // X
	data[1] = (int16_t)((mag[3] << 8) | mag[2]); // Y
	data[2] = (int16_t)((mag[5] << 8) | mag[4]); // Z
	
	return TWI_OK;
}

int32_t lsm303agrMagRawToNT(int16_t raw)
{
	return ((int32_t)raw * 150);
}

int16_t lsm303agrMagNTToRaw(int32_t ntValue)
{
	if (ntValue >= 0)
	{
		return (int16_t)((ntValue + 75) / 150);
	}
	else
	{
		return (int16_t)((ntValue - 75) / 150);
	}
}

uint8_t LSM303AGR_set_hard_iron_offset(int16_t xOffset, int16_t yOffset, int16_t zOffset)
{
	uint8_t buf[6];
	uint8_t err;
	
	buf[0] = xOffset & 0xFF;
	buf[1] = (xOffset >> 8) & 0xFF;
	buf[2] = yOffset & 0xFF;
	buf[3] = (yOffset >> 8) & 0xFF;
	buf[4] = zOffset & 0xFF;
	buf[5] = (zOffset >> 8) & 0xFF;

	err = HAL_twi_write(LSM303AGR_MAG_ADDR, OFFSET_X_L_REG_M | 0x80, buf, 6);
	if (err != TWI_OK)
	{
		return err;
	}
	
	return TWI_OK;
}

int32_t LSM303AGR_mag_raw_to_nT(int16_t raw)
{
	return raw * 150;
}











void lsm303agrCollectMagSamples(MagSamples_t *samples, uint16_t duration_ms, uint16_t sampleRate_hz)
{
	uint16_t interval_ms = 1000 / sampleRate_hz;
	samples->count = 0;

	uint16_t maxSamples = duration_ms / interval_ms;
	if (maxSamples > MAG_SAMPLE_COUNT)
	{
		maxSamples = MAG_SAMPLE_COUNT;
	}
	
	timer1Init(interval_ms);
	timer1Start();

	for (uint16_t i = 0; i < maxSamples; i++)
	{
		if (timer1Flag())
		{
			timer1ClearFlag();
			
			int16_t mag[3];
			if (LSM303AGR_read_mag(mag) == TWI_OK)
			{
				samples->x[i] = mag[0];
				samples->y[i] = mag[1];
				samples->z[i] = mag[2];
				samples->count++;
			}

			i++;
		}
		
		timer1Stop();
	}
}

void lsm303agrComputeHardIron(MagSamples_t *samples, int16_t *offset_x, int16_t *offset_y, int16_t *offset_z)
{
	int16_t min_x = samples->x[0], max_x = samples->x[0];
	int16_t min_y = samples->y[0], max_y = samples->y[0];
	int16_t min_z = samples->z[0], max_z = samples->z[0];

	for (uint16_t i = 1; i < samples->count; i++)
	{
		if (samples->x[i] < min_x) min_x = samples->x[i];
		if (samples->x[i] > max_x) max_x = samples->x[i];

		if (samples->y[i] < min_y) min_y = samples->y[i];
		if (samples->y[i] > max_y) max_y = samples->y[i];

		if (samples->z[i] < min_z) min_z = samples->z[i];
		if (samples->z[i] > max_z) max_z = samples->z[i];
	}

	*offset_x = (max_x + min_x) / 2;
	*offset_y = (max_y + min_y) / 2;
	*offset_z = (max_z + min_z) / 2;
}