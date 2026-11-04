#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include <stdint.h>
#include "app.h"

uint16_t telemetry_get_data(drive_state *state, char *telemetry_buf, uint32_t now);

#endif /* INC_TELEMETRY_H_ */
