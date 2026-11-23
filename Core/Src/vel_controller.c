#include <stdbool.h>
#include <math.h>
#include <vel_controller.h>
#include "app.h"

#define MIN_CV -255
#define MAX_CV 255
#define I_DEADBAND 0.3f
#define D_DEADBAND 3.0f
#define MAX_RAW_DERIVATIVE 30 //d clamp

typedef uint32_t (*get_millis_from_start)(void);

static const float ff_coef1 = 2.64f;
static const float ff_coef2 = -1.0f;
static const float i_discharge = 2.0f;
static float target_rpm;
static float error;
static float p;
static float i;
static float d;
static float raw_derivative;
static float previous_rpm;
static float current_rpm;
static float uff;
static float i_sum;
static int16_t control_value;

void calcI()
{
  float current_i = error * DELTA_TIME_SEC;
  float rawOutput = uff + p + (i_sum + current_i) * VEL_KI + d;

  if ((rawOutput > MAX_CV && error > 0) || (rawOutput < MIN_CV && error < 0)) //anti windup
	return;

  if (fabsf(error) <= I_DEADBAND)
	return;

  if (fabsf(target_rpm) - fabsf(current_rpm) < 0)
	  i_sum += current_i * i_discharge;
  else
	  i_sum += current_i;

  i = i_sum * VEL_KI;
}

void calcD()
{
  if (VEL_KD == 0)
  {
	d = 0;
	previous_rpm = current_rpm;
	return;
  }

  if (fabsf(error) <= D_DEADBAND)
  {
	d = 0;
	previous_rpm = current_rpm;
	return;
  }

  raw_derivative = (current_rpm - previous_rpm) / DELTA_TIME_SEC;

  // d clamping
  if (raw_derivative > MAX_RAW_DERIVATIVE)
	  raw_derivative = MAX_RAW_DERIVATIVE;
  else if (raw_derivative < -MAX_RAW_DERIVATIVE)
	  raw_derivative = -MAX_RAW_DERIVATIVE;

  d = VEL_KD * raw_derivative;
  previous_rpm = current_rpm;
}

void vel_controller_init()
{
	target_rpm = 0;
  error = 0;
  p = 0;
  i = 0;
  d = 0;
  raw_derivative = 0;
  previous_rpm = 0;
  current_rpm = 0;
  uff = 0;
  i_sum = 0;
  control_value = 0;
}

void vel_controller_set_target_rpm(float target)
{
	target_rpm = target;
}

void vel_controller_set_current_rpm(float current)
{
	current_rpm = current;
}

void vel_controller_fill_drive_state(drive_state *state)
{
	if (state == NULL)
		return;

	state->cont_pwm = control_value;
	state->cont_vel_error = error;
	state->cont_vel_p = p;
	state->cont_vel_i = i;
	state->cont_vel_d = d;
	state->cont_vel_uff = uff;
}

int16_t vel_controller_get_control_value()
{
  if (target_rpm == 0)
  {
      i_sum = 0;
      i = 0;
      control_value = 0;
      previous_rpm = current_rpm;
      return 0;
  }

  error = target_rpm - current_rpm;
  p = error * VEL_KP;
  uff = ff_coef1 * target_rpm + ff_coef2;

  calcD();
  calcI();

  float output = uff + p + i + d;
  // control value clamping
  if (output > MAX_CV)
	  output = MAX_CV;
  else if (output < MIN_CV)
	  output = MIN_CV;

  if (!isfinite(output)) // Nan protection
  {
      control_value = 0;
      return 0;
  }

  control_value = (int16_t)output;
  return control_value;
}

void vel_controller_reset()
{
  target_rpm = 0;
  error = 0;
  p = 0;
  i = 0;
  d = 0;
  raw_derivative = 0;
  previous_rpm = 0;
  uff = 0;
  i_sum = 0;
  control_value = 0;
  current_rpm = 0;
}
