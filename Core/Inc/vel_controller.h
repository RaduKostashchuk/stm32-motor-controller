#ifndef INC_CONTROLLER_H
#define INC_CONTROLLER_H

#include <stdint.h>
#include "app.h"

#define VEL_KP 0.25f
#define VEL_KI 16.0f
#define VEL_KD 0.0f

void vel_controller_init(void);
void vel_controller_set_target_rpm(float target);
void vel_controller_set_current_rpm(float current);
int16_t vel_controller_get_control_value();
void vel_controller_fill_drive_state(drive_state * state);
void vel_controller_reset();

#endif /* INC_CONTROLLER_H */
