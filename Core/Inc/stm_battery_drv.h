#ifndef STM_BATTERY_DRV_H
#define STM_BATTERY_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void stm_bat_drv_init(ADC_HandleTypeDef *adc, uint32_t v_chan, uint32_t a_chan);
float stm_bat_drv_get_vdda(void);
float stm_bat_drv_get_vbat_raw(void);

#ifdef __cplusplus
}
#endif

#endif  /* STM_BATTERY_DRV_H */
