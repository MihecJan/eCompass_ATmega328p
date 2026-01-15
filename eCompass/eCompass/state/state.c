#include <string.h>

#include "state.h"

static system_state_t current_state = STATE_AUTO_CALIBRATION;
// static system_state_t current_state = STATE_MOTION_CAL;

system_state_t state_get(void)
{
	return current_state;
}

void state_set(system_state_t s)
{
	current_state = s;
}

const char* state_to_str(system_state_t s)
{
	if (s == STATE_AUTO_CALIBRATION)
	{
		return "Auto calibration";	
	}
	
	if (s == STATE_CALIBRATING)
	{
		return "Calibrating";
	}
	
	if (s == STATE_MANUAL_CALIBRATION)
	{
		return "Manual calibration";
	}
	
	if (s == STATE_OFFSET_X)
	{
		return "X offset";
	}
	
	if (s == STATE_OFFSET_Y)
	{
		return "Y offset";
	}
	
	if (s == STATE_OFFSET_Z)
	{
		return "Z offset";
	}
	
	if (s == STATE_DECLINATION)
	{
		return "Declination";
	}
	
	if (s == STATE_MOTION_CAL)
	{
		return "Motion cal";
	}
	
	return "Invalid state";
}