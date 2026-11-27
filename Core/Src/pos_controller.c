#include "pos_controller.h"
#include "app.h"
#include <stdint.h>
#include <stdlib.h>

static int32_t target_pos = 0;
static int32_t pos_error = 0;
static float p = 0;

void pos_controller_reset(int32_t current_pos)
{
	pos_error = 0;
	p = 0;
	target_pos = current_pos;
}

void pos_controller_set_target_pos(int32_t pos)
{
	target_pos = pos;
}

void pos_controller_calc_control_value(int32_t current_pos)
{
	pos_error = target_pos - current_pos;
	if (labs(pos_error) <= POS_DEADBAND)
	{
		p = 0;
		return;
	}

	p = POS_KP * (float)pos_error;

	if (p > POS_CONTROL_MAX_RPM)
		p = POS_CONTROL_MAX_RPM;
	else if (p < POS_CONTROL_MIN_RPM)
		p = POS_CONTROL_MIN_RPM;

	if (p > 0 && p < MIN_MOTOR_RPM)
		p = MIN_MOTOR_RPM;
	else if (p < 0 && p > -MIN_MOTOR_RPM)
		p = -MIN_MOTOR_RPM;
}

void pos_controller_fill_drive_state(drive_state_t * state)
{
	state->cont_pos_error = pos_error;
	state->target_rpm = p;
}
