#include <stdint.h>
#include "format.h"
#include "app.h"
#define TX_BUF_LEN 160

static const char telemetry_prefix[] = "T:";

static size_t telemetry_build(char *txBuf, DriveState *state)
{
	_Static_assert(TX_BUF_LEN >= 130, "TX_BUF_LEN too small for worst case");
	size_t txBufPos = 0;
	size_t copyLen = 0;

	if (!try_copy_str(telemetry_prefix, txBuf, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos = copyLen;

	txBufPos += uint32_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, HAL_GetTick());

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->app_desiredRpm);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->est_currentRpm);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_error);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_p);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_i);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_d);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += float_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_uff);

	if (!try_copy_str(",", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	txBufPos += copyLen;

	txBufPos += int32_to_char_dec(txBuf + txBufPos, TX_BUF_LEN - txBufPos, state->cont_pwm);

	if (!try_copy_str("\n", txBuf + txBufPos, TX_BUF_LEN, &copyLen))
		return 0;

	return txBufPos + copyLen;
}

char *telemetry_get_data(DriveState *state, size_t *buf_len)
{
	static char txBuf[TX_BUF_LEN];
   	*buf_len = telemetry_build(txBuf, state);
    return txBuf;
}
