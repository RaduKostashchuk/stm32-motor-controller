#include <stdbool.h>
#include <math.h>
#include "app.h"

#define MIN_CV -255
#define MAX_CV 255
#define I_D_DEADBAND 0.3f
#define MAX_RAW_DERIVATIVE 30 //d clamp
#define DELTA_TIME_SEC 0.05f // TIM4 configured for 20Hz

typedef uint32_t (*get_millis_from_start)(void);

static const float kp = 0.25f;
static const float ki = 2.0f;
static const float kd = 0.0f;
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
  float rawOutput = uff + p + (i_sum + current_i) * ki + d;

  if ((rawOutput > MAX_CV && error > 0) || (rawOutput < MIN_CV && error < 0)) //anti windup
	return;

  if (fabsf(error) <= I_D_DEADBAND)
	return;

  if (error < 0)
	  i_sum += current_i * i_discharge;
  else
	  i_sum += current_i;

  i_sum += current_i;
  i = i_sum * ki;
}

void calcD()
{
  if (kd == 0)
  {
	d = 0;
	previous_rpm = current_rpm;
	return;
  }

  if (fabsf(error) <= I_D_DEADBAND)
  {
	d = 0;
	previous_rpm = current_rpm;
	return;
  }

  raw_derivative = (previous_rpm - current_rpm) / DELTA_TIME_SEC;

  // d clamping
  if (raw_derivative > MAX_RAW_DERIVATIVE)
	  raw_derivative = MAX_RAW_DERIVATIVE;
  else if (raw_derivative < -MAX_RAW_DERIVATIVE)
	  raw_derivative = -MAX_RAW_DERIVATIVE;

  d = kd * raw_derivative;
  previous_rpm = current_rpm;
}

void controller_init()
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

void controller_set_target_rpm(float target)
{
	target_rpm = target;
}

void controller_set_current_rpm(float current)
{
	current_rpm = current;
}

void controller_fill_drive_state(drive_state *state)
{
	if (state == NULL)
		return;

	state->cont_pwm = control_value;
	state->cont_error = error;
	state->cont_p = p;
	state->cont_i = i;
	state->cont_d = d;
	state->cont_uff = uff;
}

int16_t controller_get_control_value()
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
  p = error * kp;
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

void controller_reset()
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
