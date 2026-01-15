#include "state_machine.h"
#include "services/button_service.h"
#include "services/eCompass_service.h"

#include "services/uart_service.h"

button_event_t state_machine_step()
{
	button_event_t ev;
	button_service_get(&ev);

	
	uart_send_byte('A' + ev);
	
	switch(state_get())
	{
		case STATE_AUTO_CALIBRATION:
			if (ev == BTN_EVENT_MODE)
			{
				mag_apply_offset(MAG_OFFSET_PROFILE_MANUAL);
				state_set(STATE_MANUAL_CALIBRATION);
			}
			else if (ev == BTN_EVENT_ENTER)
			{
				state_set(STATE_CALIBRATING);
			}
			break;
			
		case STATE_CALIBRATING:
			state_set(STATE_AUTO_CALIBRATION);
			break;
		
		case STATE_MANUAL_CALIBRATION:
			if (ev == BTN_EVENT_MODE)
			{
				state_set(STATE_DECLINATION);
			}
			else if (ev == BTN_EVENT_ENTER)
			{
				state_set(STATE_OFFSET_X);
			}
			break;
		
		case STATE_OFFSET_X:
			if (ev == BTN_EVENT_ENTER)
			{
				state_set(STATE_OFFSET_Y);
			}
			break;
		
		case STATE_OFFSET_Y:
			if (ev == BTN_EVENT_ENTER)
			{
				state_set(STATE_OFFSET_Z);
			}
			break;
		
		case STATE_OFFSET_Z:
			if (ev == BTN_EVENT_ENTER)
			{
				mag_apply_offset(MAG_OFFSET_PROFILE_MANUAL);
				state_set(STATE_MANUAL_CALIBRATION);
			}
			break;
		
		case STATE_DECLINATION:
			if (ev == BTN_EVENT_MODE)
			{
				mag_apply_offset(MAG_OFFSET_PROFILE_AUTO);
				state_set(STATE_AUTO_CALIBRATION);
			}
			break;
			
		case STATE_MOTION_CAL:
			break;
	}
	
	return ev;
}