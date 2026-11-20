#include <stdint.h>
#include <estimator.h>
#include "capture_diag.h"
#include "format.h"
#include "critical.h"
#include "app.h"

#define MAX_RPM 95
#define MAX_DELTA_POS ((int32_t)(MAX_RPM * TICKS_PER_REV / 60.0f * DELTA_TIME_SEC * 3))
#define T_METHOD_TIMEOUT_MS 100
#define SIGN_STABLE_THRESHOLD 3

static volatile float rpm_m;
static volatile float rpm_t;

static volatile uint32_t cycle_hist[AVG_WINDOW];
static volatile uint8_t cycle_ind = 0;
static volatile uint8_t current_ind = 0;
static volatile uint32_t sliding_sum = 0;
static volatile int32_t prev_pos_t = 0;
static volatile int32_t prev_pos_m = 0;
static volatile int8_t sign = 1;
static volatile int8_t sign_candidate = 1;
static int8_t sign_stable_ind = 0;
static volatile uint32_t tick_absense_start = 0;

static void clear_cycle_hist()
{
	cycle_ind = 0;
	current_ind = 0;
	rpm_t = 0;
	sliding_sum = 0;
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		cycle_hist[i] = 0;
}

void estimator_init(int32_t init_pos)
{
	prev_pos_m = init_pos;
	prev_pos_t = init_pos;
	rpm_m = 0;
	sign = 1;
	sign_candidate = 1;
	sign_stable_ind = 0;
	tick_absense_start = 0;
	clear_cycle_hist();
}

void estimator_run_m(int32_t current_pos)
{
	int32_t delta = current_pos - prev_pos_m;

	if (delta > MAX_DELTA_POS || delta < -MAX_DELTA_POS)
	{
		prev_pos_m = current_pos;
		rpm_m = 0;
		return;
	}

    float ticks_per_sec = (float)(delta) / DELTA_TIME_SEC;
    rpm_m = 60.0f * ticks_per_sec / TICKS_PER_REV;
    prev_pos_m = current_pos;
}

static void debounce_sign(int32_t delta)
{
	if ((delta < 0 && sign_candidate < 0) || (delta > 0 && sign_candidate > 0))
	{
		sign_stable_ind++;
	}
	else if (delta > 0 && sign_candidate < 0)
	{
		sign_candidate = 1;
		sign_stable_ind = 0;
	}
	else if (delta < 0 && sign_candidate > 0)
	{
		sign_candidate = -1;
		sign_stable_ind = 0;
	}

	if (sign_stable_ind >= SIGN_STABLE_THRESHOLD)
	{
		sign = sign_candidate;
		sign_stable_ind = 0;
	}
}

void estimator_update_t(uint32_t cpu_cycles, int32_t cur_pos, uint32_t now)
{
	if (cpu_cycles >= UPPER_CYCLES_BOUND)
		return;

	tick_absense_start = now;
	uint32_t primask = enter_critical();

    debounce_sign(cur_pos - prev_pos_t);
	prev_pos_t = cur_pos;

	sliding_sum += cpu_cycles - cycle_hist[cycle_ind];
	cycle_hist[cycle_ind] = cpu_cycles;
	cycle_ind = (uint8_t)((cycle_ind + 1) % AVG_WINDOW);

	if (current_ind < AVG_WINDOW)
		current_ind++;

	if (current_ind > 0)
	{
		float cycles_per_tick = (float)sliding_sum / current_ind;
		rpm_t = (float)sign * RPMT_T_COEF * 60.0f / cycles_per_tick;
	}

	exit_critical(primask);
}

void estimator_try_reset_t_method(uint32_t now, int32_t cur_pos)
{
	uint32_t primask = enter_critical();

	if (now - tick_absense_start >= T_METHOD_TIMEOUT_MS)
	{
		prev_pos_t = cur_pos;
		tick_absense_start = now;
	    sign_candidate = 1;
	    sign_stable_ind = 0;
		clear_cycle_hist();
	}

	exit_critical(primask);
}

float estimator_get_rpm_m(void)
{
	return rpm_m;
}

float estimator_get_rpm_t(void)
{
	return rpm_t;
}
