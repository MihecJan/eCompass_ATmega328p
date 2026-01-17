#include "led.h"

void led_init(uint8_t led)
{
	if (led == LED_DBG) DDRB |= (1 << LED_DBG_PIN); // configure as output
	if (led == LED_DBG) PINB &= ~(1 << LED_DBG_PIN); // turn off initially
}

void led_on(uint8_t led)
{
	if (led == LED_DBG) PINB |= (1 << LED_DBG_PIN);
}

void led_off(uint8_t led)
{
	if (led == LED_DBG) PINB &= ~(1 << LED_DBG_PIN);
}

void led_toggle(uint8_t led)
{
	if (led == LED_DBG) PINB ^= (1 << LED_DBG_PIN);
}
