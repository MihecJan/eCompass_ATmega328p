#include <stdint.h>

#include "btn_hal.h"

static uint8_t mode_pressed();
static uint8_t inc_pressed();
static uint8_t dec_pressed();
static uint8_t enter_pressed();

static button_hal_cb_t button_cb;

void button_hal_init(void)
{
	/* configure as inputs */
	BTN_DDR &= ~(
		(1 << BTN_MODE)  |
		(1 << BTN_INC)   |
		(1 << BTN_DEC)   |
		(1 << BTN_ENTER)
	);

	/* enable internal pull-ups */
	BTN_PORT |= (
		(1 << BTN_MODE)  |
		(1 << BTN_INC)   |
		(1 << BTN_DEC)   |
		(1 << BTN_ENTER)
	);
}

void button_hal_register_callback(button_hal_cb_t cb)
{
	button_cb = cb;
}

void button_hal_poll(void)
{
	if (!button_cb)
	{
		return;
	}

	if (mode_pressed()) {
		button_cb(BTN_EVENT_MODE);
	}
	else if (inc_pressed())
	{
		button_cb(BTN_EVENT_INC);
	}
	else if (dec_pressed())
	{
		button_cb(BTN_EVENT_DEC);
	}
	else if (enter_pressed())
	{
		button_cb(BTN_EVENT_ENTER);
	}
	else
	{
		button_cb(BTN_EVENT_NONE);
	}
}

static uint8_t mode_pressed()
{
	return !(PIND & (1 << BTN_MODE));
}

static uint8_t inc_pressed()
{
	return !(PIND & (1 << BTN_INC));
}

static uint8_t dec_pressed()
{
	return !(PIND & (1 << BTN_DEC));
}

static uint8_t enter_pressed()
{
	return !(PIND & (1 << BTN_ENTER));
}