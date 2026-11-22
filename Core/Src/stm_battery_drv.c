#include "main.h"

static ADC_HandleTypeDef *hadc;
static uint32_t vref_channel = 0;
static uint32_t adc_channel = 0;

void stm_bat_drv_init(ADC_HandleTypeDef *adc, uint32_t v_chan, uint32_t a_chan)
{
	hadc = adc;
	vref_channel = v_chan;
	adc_channel = a_chan;
}

static uint32_t get_v_from_channel(uint32_t channel, uint32_t sampling_time)
{
    ADC_ChannelConfTypeDef s = {0};
    s.Channel = channel;
    s.Rank = ADC_REGULAR_RANK_1;
    s.SamplingTime = sampling_time;
    s.SingleDiff = ADC_SINGLE_ENDED;
    s.OffsetNumber = ADC_OFFSET_NONE;
    s.Offset = 0;

    HAL_ADC_ConfigChannel(hadc, &s);
    HAL_ADC_Start(hadc);
    uint32_t v = 0;
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK)
        v = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);
    return v;
}

float stm_bat_drv_get_vdda()
{
	uint32_t v_refint_raw = get_v_from_channel(vref_channel, ADC_SAMPLETIME_247CYCLES_5);
	if (v_refint_raw == 0.0f)
		return 0.0f;

	return ((float)VREFINT_CAL_VREF / 1000.0f) * (float)(*VREFINT_CAL_ADDR) / (float)v_refint_raw;
}

float stm_bat_drv_get_vbat_raw()
{
	return (float)get_v_from_channel(adc_channel, ADC_SAMPLETIME_92CYCLES_5);
}

