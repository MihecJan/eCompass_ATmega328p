#ifndef BUTTON_SERVICE_H_
#define BUTTON_SERVICE_H_

#include <stdint.h>

#include "hal/btn/btn_hal.h"

void button_service_init(void);

/* Called by HAL callback */
void button_service_hw_post(button_event_t ev);

/* Called by application */
void button_service_get(button_event_t *ev);


#endif /* BUTTON_SERVICE_H_ */