#include "critical.h"

static critical_state_t enter_count = 0;
static critical_state_t exit_count = 0;

critical_state_t enter_critical(void)
{
    enter_count++;
    return 0;
}

void exit_critical(critical_state_t state)
{
    (void)state;
    exit_count++;
}
