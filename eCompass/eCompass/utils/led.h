#ifndef LED_H_
#define LED_H_

#define LED_DBG_PIN		PB5
#define LED_DBG			5

#define TOGGLE_LED() (PINB ^= (1 << PORTD6))

#include <avr/io.h>
#include <stdint.h>

void led_init(uint8_t led);
void led_on(uint8_t led);
void led_off(uint8_t led);
void led_toggle(uint8_t led);


#endif /* LED_H_ */