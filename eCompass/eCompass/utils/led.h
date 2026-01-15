#ifndef LED_H_
#define LED_H_

#define LED_DBG_PORT	PORTD
#define LED_DBG_DDR		DDRD
#define LED_DBG_PIN		PORTD6
#define LED_DBG_DD		DDD6

#define TOGGLE_LED() (LED_DBG_PORT ^= (1 << PORTD6))

#include <avr/io.h>
#include <stdint.h>

#define LED_DBG		1

void led_init(uint8_t led);
void led_on(uint8_t led);
void led_off(uint8_t led);
void led_toggle(uint8_t led);


#endif /* LED_H_ */