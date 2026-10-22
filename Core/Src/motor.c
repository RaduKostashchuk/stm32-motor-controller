#include <stdint.h>
#include "tim.h"

void motor_set_pwm(int16_t pwm);

void motor_init(void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	motor_set_pwm(0);
}

void motor_set_pwm(int16_t pwm)
{
	if (pwm > 255)
		pwm = 255;
	else if (pwm < -255)
		pwm = -255;

	if (pwm > 0)
	{
		HAL_GPIO_WritePin(GPIOA, M1_D1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, M1_D2_Pin, GPIO_PIN_RESET);
	}
	else if (pwm < 0)
	{
		HAL_GPIO_WritePin(GPIOA, M1_D1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, M1_D2_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, M1_D1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, M1_D2_Pin, GPIO_PIN_RESET);
	}

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm < 0 ? -pwm : pwm);
}
