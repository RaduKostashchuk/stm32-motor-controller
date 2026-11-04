#include <stdint.h>
#include "format.h"
#include "app.h"

static const char telemetry_prefix[] = "T:";

uint16_t telemetry_get_data(drive_state *state, char *telemetry_buf, uint32_t now)
{
	_Static_assert(TELEMETRY_BUF_SIZE > 130, "TX_BUF_LEN too small for worst case");
	size_t buf_pos = 0;
	size_t copy_len = 0;

	if (!try_copy_str(telemetry_prefix, telemetry_buf, TELEMETRY_BUF_SIZE, &copy_len))
		return 0;

	buf_pos = copy_len;

	if (!try_uint32_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, now, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->app_desiredRpm, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->est_currentRpm, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_error, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_p, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_i, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_d, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_float_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_uff, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str(",", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_int32_to_str(telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, state->cont_pwm, &copy_len))
		return 0;

	buf_pos += copy_len;

	if (!try_copy_str("\n", telemetry_buf + buf_pos, TELEMETRY_BUF_SIZE - buf_pos, &copy_len))
		return 0;

	buf_pos += copy_len;

	return (int16_t)buf_pos;
}
