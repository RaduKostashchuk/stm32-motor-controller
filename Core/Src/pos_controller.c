#include "pos_controller.h"
#include <stdint.h>

static int32_t target_pos = 0;
static int32_t init_pos = 0;
static int32_t pos_error = 0;
static float p = 0;

void pos_controller_init(int32_t current_pos)
{
	pos_error = 0;
	target_pos = current_pos;
	init_pos = current_pos;
}

void pos_controller_set_target_pos(int32_t pos)
{
	target_pos = pos;
}

void pos_controller_calc_control_value(int32_t current_pos)
{
	pos_error = target_pos - current_pos;
	p = POS_KP * (float)pos_error;

	if (p > POS_CONTROL_MAX_RPM)
		p = POS_CONTROL_MAX_RPM;
	else if (p > POS_CONTROL_MAX_RPM)
		p = -POS_CONTROL_MAX_RPM;
}

void pos_controller_fill_drive_state(drive_state_t * state)
{
	state->cont_pos_error = pos_error;
	state->target_rpm = p;
}
