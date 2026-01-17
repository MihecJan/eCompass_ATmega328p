#include <stdint.h>

#include "util.h"


int16_t MATH_get_approx_total_field(int16_t mag[3])
{
	if (mag[0] < 0) mag[0] = -mag[0];
	if (mag[1] < 0) mag[1] = -mag[1];
	if (mag[2] < 0) mag[2] = -mag[2];
		
	if (mag[0] > mag[1] && mag[0] > mag[2])
	{
		if (mag[1] > mag[2])
		{
			return mag[0] + 0.5 * mag[1] + 0.25 * mag[2];
		}
		else
		{
			return mag[0] + 0.5 * mag[2] + 0.25 * mag[1];
		}
	}
	else if (mag[1] > mag[2])
	{
		if (mag[0] > mag[2])
		{
			return mag[1] + 0.5 * mag[0] + 0.25 * mag[2];
		}
		else
		{
			return mag[1] + 0.5 * mag[2] + 0.25 * mag[0];
		}
	}
	else
	{
		if (mag[0] > mag[1])
		{
			return mag[2] + 0.5 * mag[0] + 0.25 * mag[1];
		}
		else
		{
			return mag[2] + 0.5 * mag[1] + 0.25 * mag[0];
		}
	}
}