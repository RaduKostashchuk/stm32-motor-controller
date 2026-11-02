#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include <stdint.h>
#include "app.h"

void controller_init(void);
void controller_set_target_rpm(float target);
void controller_set_current_rpm(float current);
int16_t controller_get_control_value();
void controller_fill_drive_state(DriveState * state);
void controller_reset();

#endif /* INC_CONTROLLER_H_ */
