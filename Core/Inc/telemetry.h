#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include <stdint.h>
#include "app.h"

char *telemetry_get_data(DriveState *state, size_t *bufLen);

#endif /* INC_TELEMETRY_H_ */
