#include "button_service.h"
#include "hal/btn/btn_hal.h"

/* Hardware button event */
static volatile button_event_t hw_event = BTN_EVENT_NONE;

static uint8_t previous_was_pressed = 0;

static void button_hal_event_handler(button_event_t ev)
{
	hw_event = ev;
}

void button_service_init(void)
{
	hw_event = BTN_EVENT_NONE;
	button_hal_register_callback(button_hal_event_handler);
}

void button_service_hw_post(button_event_t ev)
{
	hw_event = ev;
}

void button_service_get(button_event_t *ev)
{
	if (hw_event == BTN_EVENT_NONE)
	{
		previous_was_pressed = 0;
		return;
	}
	
	if (previous_was_pressed)
	{
		*ev = BTN_EVENT_NONE;
	}
	else
	{
		*ev = hw_event;
		previous_was_pressed = 1;
	}
}