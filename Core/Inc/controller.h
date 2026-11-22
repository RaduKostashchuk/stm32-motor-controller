#ifndef INC_CONTROLLER_H
#define INC_CONTROLLER_H

#include <stdint.h>
#include "app.h"

#define KP 0.25f
#define KI 16.0f
#define KD 0.0f

void controller_init(void);
void controller_set_target_rpm(float target);
void controller_set_current_rpm(float current);
int16_t controller_get_control_value();
void controller_fill_drive_state(drive_state * state);
void controller_reset();

#endif /* INC_CONTROLLER_H */
