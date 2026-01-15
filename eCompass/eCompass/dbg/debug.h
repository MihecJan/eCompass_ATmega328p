#ifndef DEBUG_H_
#define DEBUG_H_


#include <stdint.h>

#include "../uart_hal/uart_hal.h"

#define DEBUG_ENABLE

typedef enum {
	DBG_TWI_RW		= 1 << 0,
	DBG_TWI_OTHER	= 1 << 1
} dbgFlag_t;

extern volatile uint8_t dbgFlags;

void dbgPrint(dbgFlag_t msgFlag, const char *msg);


#endif /* DEBUG_H_ */