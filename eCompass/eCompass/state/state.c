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