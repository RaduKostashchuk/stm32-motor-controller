#ifndef BATTERY_H
#define BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
	float (*get_vbat_raw)(void);
	float (*get_vdda)(void);
} battery_t;

float battery_calc_v(const battery_t *b);

#ifdef __cplusplus
}
#endif

#endif  /* BATTERY_H */
