#include "battery.h"

#define R1 33000u
#define R2 10000u
#define VOLTAGE_DIVIDER_CF (float)(R1 + R2) / (float)R2
#define LPF_ALPHA 0.1f

static float filtered_voltage = 0.0f;

float battery_calc_v(const battery_t *b)
{
	float vdda = b->get_vdda();
	float v_bat = b->get_vbat_raw();
	float current_voltage = VOLTAGE_DIVIDER_CF * vdda * v_bat / 4095.0f;
	filtered_voltage = current_voltage * LPF_ALPHA + (1 - LPF_ALPHA) * filtered_voltage;
	return filtered_voltage;
}
