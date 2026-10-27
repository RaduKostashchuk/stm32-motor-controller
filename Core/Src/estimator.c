#include <stdint.h>
#include <estimator.h>

#define DELTA_TIME_SEC 0.05f // TIM4 configured for 20Hz
#define TICKS_PER_REV 1980
#define MAX_DELTA_POS 160 // encoder failure protection

static int32_t lastPosition;
static volatile float rpm;
static const float deltaTimeSec = 0.05f;

void estimator_init(int32_t initialPosition)
{
	lastPosition = initialPosition;
	rpm = 0;
}

void estimator_run(int32_t currentPosition)
{
	int32_t deltaPos = currentPosition - lastPosition;

	if (deltaPos >= MAX_DELTA_POS || deltaPos <= -deltaPos)
		return;

    float ticksPerSec = (float)(deltaPos) / deltaTimeSec;
    rpm = 60.0f * ticksPerSec / TICKS_PER_REV;

    lastPosition = currentPosition;
}

float estimator_get_rpm(void)
{
	return rpm;
}
