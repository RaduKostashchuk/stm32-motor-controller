#ifndef INC_CRITICAL_H
#define INC_CRITICAL_H

#include <stdint.h>
#include "app.h"

typedef uint32_t critical_state_t;

critical_state_t enter_critical(void);
void exit_critical(critical_state_t state);

#endif /* INC_CRITICAL_H */
