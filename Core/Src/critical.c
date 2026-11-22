#include "critical.h"
#include "cmsis_gcc.h"

critical_state_t enter_critical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void exit_critical(critical_state_t primask)
{
	__set_PRIMASK(primask);
}
