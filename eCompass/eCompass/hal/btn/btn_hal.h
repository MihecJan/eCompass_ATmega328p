#ifndef BTN_HAL_H_
#define BTN_HAL_H_


#include <avr/io.h>

/* pin mapping */
#define BTN_PORT   PORTD
#define BTN_PIN    PIND
#define BTN_DDR    DDRD

#define BTN_MODE   PD2
#define BTN_ENTER  PD3
#define BTN_INC    PD4
#define BTN_DEC    PD5

typedef enum {
	BTN_EVENT_NONE = 0,
	BTN_EVENT_ENTER,
	BTN_EVENT_MODE,
	BTN_EVENT_INC,
	BTN_EVENT_DEC,
} button_event_t;

typedef void (*button_hal_cb_t)(button_event_t ev);

void button_hal_init(void);
void button_hal_register_callback(button_hal_cb_t cb);
void button_hal_poll(void);


#endif /* BTN_HAL_H_ */