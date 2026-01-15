#include "../config.h"

#include <util/delay.h>
#include <stdio.h>

#include "mag_calibration.h"

/* internal storage */
static int16_t mag_min[3];
static int16_t mag_max[3];
static int16_t mag_offset[3] = {0};

void math_mag_get_offset(int16_t offset[3])
{
	offset[0] = mag_offset[0];
	offset[1] = mag_offset[1];
	offset[2] = mag_offset[2];
}

void math_mag_calib_hard_init(void)
{
	mag_min[0] = mag_min[1] = mag_min[2] =  32767;
	mag_max[0] = mag_max[1] = mag_max[2] = -32768;
}

void math_mag_calib_hard_update(const int16_t mag[3])
{
	for (uint8_t i = 0; i < 3; i++)
	{
		if (mag[i] < mag_min[i]) mag_min[i] = mag[i];
		if (mag[i] > mag_max[i]) mag_max[i] = mag[i];
	}
}

void math_mag_calib_hard_compute_offset()
{
	for (uint8_t i = 0; i < 3; i++)
	{
		mag_offset[i] = (int16_t)(((int32_t)mag_max[i] + mag_min[i]) / 2);
	}
}