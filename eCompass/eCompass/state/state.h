#ifndef STATE_H_
#define STATE_H_


#define SEND_GRAPHIC_UART

#include <stdint.h>

typedef enum {
	STATE_AUTO_CALIBRATION,
	STATE_CALIBRATING,
	STATE_MANUAL_CALIBRATION,
	STATE_OFFSET_X,
	STATE_OFFSET_Y,
	STATE_OFFSET_Z,
	STATE_DECLINATION,
	STATE_MOTION_CAL
} system_state_t;

system_state_t state_get(void);
void state_set(system_state_t s);

#endif /* STATE_H_ */