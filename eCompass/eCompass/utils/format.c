#include <stdio.h>

#include "format.h"

char *format_milli_to_decimal(int32_t value, char *buf, uint8_t bufSize)
{
	int32_t integer;
	int32_t frac;
	uint8_t negative = 0;

	if (value < 0)
	{
		negative = 1;
		value = -value;
	}

	integer = value / 1000;
	frac    = value % 1000;

	if (negative)
	{
		snprintf(buf, bufSize, "-%ld,%02ld",
		(long)integer, (long)frac);
	}
	else
	{
		snprintf(buf, bufSize, "%ld,%03ld",
		(long)integer, (long)frac);	
	}

	return buf;
}
