#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include <stdint.h>

void telemetry_send(UART_HandleTypeDef *huart, DriveState *state);

#endif /* INC_TELEMETRY_H_ */
