#include <stdbool.h>
#include <math.h>
#include "app.h"

#define MIN_CV -255
#define MAX_CV 255
#define I_D_DEADBAND 1.5f
#define MAX_RAW_DERIVATIVE 30 //d clamp
#define DELTA_TIME_SEC 0.05f // TIM4 configured for 20Hz

typedef uint32_t (*get_millis_from_start)(void);

static const float kp = 1;
static const float ki = 1;
static const float kd = 0;
static const float ffCoef1 = 2.64f;
static const float ffCoef2 = -1;
static float targetRpm;
static float error;
static float p;
static float i;
static float d;
static float rawDerivative;
static float previousRpm;
static float currentRpm;
static float uff;
static float iSum;
static int16_t control_value;

void calcI()
{
  float currentI = error * DELTA_TIME_SEC;
  float rawOutput = uff + p + (iSum + currentI) * ki + d;

  if ((rawOutput > MAX_CV && error > 0) || (rawOutput < MIN_CV && error < 0)) //anti windup
	return;

  if (fabsf(error) <= I_D_DEADBAND)
	return;

  iSum += currentI;
  i = iSum * ki;
}

void calcD()
{
  if (kd == 0)
  {
	d = 0;
	previousRpm = currentRpm;
	return;
  }

  if (fabsf(error) <= I_D_DEADBAND)
  {
	d = 0;
	previousRpm = currentRpm;
	return;
  }

  rawDerivative = (previousRpm - currentRpm) / DELTA_TIME_SEC;

  // d clamping
  if (rawDerivative > MAX_RAW_DERIVATIVE)
	  rawDerivative = MAX_RAW_DERIVATIVE;
  else if (rawDerivative < -MAX_RAW_DERIVATIVE)
	  rawDerivative = -MAX_RAW_DERIVATIVE;

  d = kd * rawDerivative;
  previousRpm = currentRpm;
}

void controller_init()
{
  targetRpm = 0;
  error = 0;
  p = 0;
  i = 0;
  d = 0;
  rawDerivative = 0;
  previousRpm = 0;
  currentRpm = 0;
  uff = 0;
  iSum = 0;
  control_value = 0;
}

void controller_set_target_rpm(float target)
{
  targetRpm = target;
}

void controller_set_current_rpm(float current)
{
  currentRpm = current;
}

void controller_fill_drive_state(DriveState *state)
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
  if (targetRpm == 0)
  {
      iSum = 0;
      i = 0;
      control_value = 0;
      previousRpm = currentRpm;
      return 0;
  }

  error = targetRpm - currentRpm;
  p = error * kp;
  uff = ffCoef1 * targetRpm + ffCoef2;

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
  targetRpm = 0;
  error = 0;
  p = 0;
  i = 0;
  d = 0;
  rawDerivative = 0;
  previousRpm = 0;
  uff = 0;
  iSum = 0;
  control_value = 0;
  currentRpm = 0;
}
