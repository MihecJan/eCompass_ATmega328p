#include <stdio.h>

#include "utils.h"
#include "globals.h"
#include "services/uart_service.h"
#include "format.h"

void report(uint8_t err, const char *prefix, const char *label, const char *value, const char *unit)
{
	if (err)
	{
		snprintf(buff, sizeof(buff), "ERROR: %s (Code: %d)\r\n", label, err);
	}
	else
	{
		snprintf(buff, sizeof(buff), "%s%s: %s %s\r\n",
			prefix ? prefix : "",
			label,
			value,
			unit ? unit : "");
	}
	uart_send_string(buff);
}

void report_uint16(uint8_t err, const char *prefix, const char *label, uint16_t value, const char *unit)
{
	if (err)
	{
		snprintf(buff, sizeof(buff), "ERROR: %s (Code: %d)\r\n", label, err);
	}
	else
	{
		snprintf(buff, sizeof(buff), "%s%s: %u %s\r\n",
		prefix ? prefix : "",
		label,
		value,
		unit ? unit : "");
	}
	uart_send_string(buff);
}

void report_int16(uint8_t err, const char *prefix, const char *label, int16_t value, const char *unit)
{
	if (err)
	{
		snprintf(buff, sizeof(buff), "ERROR: %s (Code: %d)\r\n", label, err);
	}
	else
	{
		snprintf(buff, sizeof(buff), "%s%s: %d %s\r\n",
		prefix ? prefix : "",
		label,
		value,
		unit ? unit : "");
	}
	uart_send_string(buff);
}

void report_decimal(uint8_t err, const char *label, int16_t value, const char *unit)
{
	if (err)
	{
		snprintf(buff, sizeof(buff), "ERROR: %s (Code: %d)\r\n", label, err);
	}
	else
	{
		char formatted_val[16];
		format_milli_to_decimal(value, formatted_val, sizeof(formatted_val));
		
		snprintf(buff, sizeof(buff), "%s: %s %s\r\n",
			label,
			formatted_val,
			unit ? unit : "");
	}
	uart_send_string(buff);
}