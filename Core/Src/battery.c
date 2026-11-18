#include "battery.h"

#define R1 33000u
#define R2 10000u
#define VOLTAGE_DIVIDER_CF (float)(R1 + R2) / (float)R2

float battery_calc_v(const battery_t *b)
{
	float vdda = b->get_vdda();
	float v_bat = b->get_vbat_raw();

	return VOLTAGE_DIVIDER_CF * vdda * v_bat / 4095.0f;
}
