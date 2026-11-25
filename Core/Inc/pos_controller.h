#ifndef POS_CONTROLLER_H
#define POS_CONTROLLER_H

#include <stdint.h>
#include "app.h"

#define POS_CONTROL_MAX_RPM 50
#define POS_KP (float)0.5f
#define POS_K_ACCEL (float)0.5f

void pos_controller_init(int32_t current_pos);
void pos_controller_set_target_pos(int32_t pos);
void pos_controller_calc_control_value(int32_t current_pos);
void pos_controller_fill_drive_state(drive_state_t * state);

#endif /* POS_CONTROLLER_H */
