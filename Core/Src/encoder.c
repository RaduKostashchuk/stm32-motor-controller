#include <stdint.h>
#include "tim.h"

void encoder_init(void)
{
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	TIM2->CNT = 0;
}

int32_t encoder_get_position(void)
{
	return (int32_t)TIM2->CNT;
}
