#include "debug.h"

volatile uint8_t dbgFlags = DBG_TWI_RW;

void dbgPrint(dbgFlag_t msgFlag, const char *msg)
{
	#ifdef DEBUG_ENABLE
	
	// Only print if the level is enabled in dbgFlags
	if (!(msgFlag & dbgFlags)) return;

	
	// Prefix based on msgFlag
	const char *prefix = "";
	switch (msgFlag)
	{
		case DBG_TWI_RW:		prefix = "TWI RW: "; break;
		case DBG_TWI_OTHER:		prefix = "TWI other: "; break;
		default:				prefix = ""; break;
	}
	
	uart_send_string(prefix);
	uart_send_string(msg);
	uart_send_string("\n\r");
	
	#endif
}