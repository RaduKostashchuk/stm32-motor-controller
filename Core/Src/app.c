#include "app.h"

void debug_print(const uint8_t *msg, size_t len)
{
	for (uint16_t i = 0; i < len; i++)
		ITM_SendChar(msg[i]);
}

void debug_print_str(const char *msg)
{
	while(*msg)
		ITM_SendChar(*msg++);
}
