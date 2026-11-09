#include "app.h"
#include "controller.h"
#include "motor.h"
#include "reader.h"
#include "parser.h"
#include "encoder.h"
#include "estimator.h"
#include "telemetry.h"
#include "format.h"
#include <string.h>

#define MAX_PWM 255
#define MIN_PWM -255
#define MAX_RPM 95
#define MIN_RPM -95
#define READER_CMD_TOO_LONG "READER ERROR: COMMAND TOO LONG\n"
#define READER_RING_OVERFLOW "READER ERROR: COMMAND RING OVERFLOW\n"
#define SUCCESS "OK\n"
#define INVALID_MOTOR "ERROR USAGE MOTOR <motor number> <pwm OR rpm>\n"
#define INVALID_STOP "ERROR USAGE STOP <motor number> OR STOP\n"
#define INVALID_STATUS "ERROR USAGE STATUS\n"
#define INVALID_MODE "ERROR USAGE MODE <AUTO|MANUAL>\n"
#define UNKNOWN "ERROR UNKNOWN COMMAND\n"
#define STATUS_SEPARATOR " || "
#define APP_REPLY_PREFIX "R:"
#define NONE "None\n"
#define CMD_BUF_SIZE 200

static const char reader_cmd_too_long[] = READER_CMD_TOO_LONG;
static const char reader_ring_overflow[] = READER_RING_OVERFLOW;
static const char success[] = SUCCESS;
static const char unknown[] = UNKNOWN;
static const char none[] = NONE;
static const char invalid_motor[] = INVALID_MOTOR;
static const char invalid_stop[] = INVALID_STOP;
static const char invalid_mode[] = INVALID_MODE;
static const char invalid_status[] = INVALID_STATUS;
static const char status_separator[] = STATUS_SEPARATOR;
static const char app_reply_prefix[] = APP_REPLY_PREFIX;
static mode_type app_mode = Auto;
static char cmd_buf[CMD_BUF_SIZE] = {0};
drive_state state;

static uint16_t execute_mode_cmd(Command *command, char *reply_buf)
{
  if (app_mode != command->modeType) // When mode changed stop motor
  {
	  app_mode = command->modeType;
	  controller_reset();
	  controller_set_target_rpm(0);
	  motor_set_pwm(0);
	  state.app_desiredRpm = 0;
  }

  uint16_t size = sizeof(success) - 1;
  memcpy(reply_buf, success, size);
  return size;
}

static uint16_t execute_motor_cmd(Command *command, char *reply_buf)
{
  bool invalidParams = command->motorNumber != 1
    || (app_mode == Auto && (command->target > MAX_RPM || command->target < MIN_RPM))
    || (app_mode == Manual && (command->target > MAX_PWM || command->target < MIN_PWM));

  if (invalidParams)
  {
	uint16_t size = sizeof(invalid_motor) - 1;
	memcpy(reply_buf, invalid_motor, size);
    return size;
  }

  if (command->motorNumber == 1)
  {
    if (app_mode == Auto)
    {
    	state.app_desiredRpm = (float)(command->target);
    	controller_set_target_rpm((float)command->target);
    }
    else if (app_mode == Manual)
    {
    	state.cont_pwm = (int16_t)command->target;
    	motor_set_pwm((int16_t)command->target);
    }
  }

  uint16_t size = sizeof(success) - 1;
  memcpy(reply_buf, success, size);
  return size;
}

static uint16_t execute_stop_cmd(Command *command, char *reply_buf)
{
  if (command->hasMotorNumber && command->motorNumber != 1)
  {
	  uint16_t size = sizeof(invalid_stop) - 1;
	  memcpy(reply_buf, invalid_stop, size);
	  return size;
  }

  controller_reset();
  controller_set_target_rpm(0);
  motor_set_pwm(0);
  state.app_desiredRpm = 0;

  uint16_t size = sizeof(success) - 1;
  memcpy(reply_buf, success, size);
  return size;
}

static uint16_t app_get_full_status(char *reply_buf)
{
	size_t out_len = 0;
	size_t pos = 0;
	if (!try_copy_str("Application mode:  ", reply_buf, CMD_BUF_SIZE, &out_len))
		return 0;

	pos = out_len;

	if (!try_copy_str(app_mode == Auto ? "Auto" : "Manual", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(status_separator, reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	switch (app_mode)
	{
	case Auto:
		if (!try_copy_str("Target Rpm: ", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
			return 0;

		pos += out_len;

		if (!try_float_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.app_desiredRpm, &out_len))
			return 0;

		pos += out_len;
		break;

	case Manual:
		if (!try_copy_str("Pwm: ", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
			return 0;

		pos += out_len;

		if (!try_int32_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.cont_pwm, &out_len))
			return 0;

		pos += out_len;
	}

	if (!try_copy_str(status_separator, reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str("Encoder:  Position: ", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_int32_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.est_position, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(status_separator, reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str("Estimator:  RPM: ", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_float_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.est_currentRpm, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str("\n", reply_buf + pos, CMD_BUF_SIZE - pos, &out_len))
		return 0;

	pos += out_len;

	return (uint16_t)pos;
}

static uint16_t app_execute_command(Command *command, char *reply_buf)
{
	uint16_t size = 0;
	switch (command->type)
	{
	  case Status:
		return app_get_full_status(reply_buf);

	  case AppMode:
		return execute_mode_cmd(command, reply_buf);

	  case Motor:
		return execute_motor_cmd(command, reply_buf);

	  case Stop:
		return execute_stop_cmd(command, reply_buf);

	  case InvalidMotor:
		size = sizeof(invalid_motor) - 1;
		memcpy(reply_buf, invalid_motor, size);
		return size;

	  case InvalidMode:
		size = sizeof(invalid_mode) - 1;
		memcpy(reply_buf, invalid_mode, size);
		return size;

	  case InvalidStop:
		size = sizeof(invalid_stop) - 1;
		memcpy(reply_buf, invalid_stop, size);
		return size;

	  case InvalidStatus:
		 size = sizeof(invalid_status) - 1;
		memcpy(reply_buf, invalid_status, size);
		return size;

	  case Unknown:
		size = sizeof(unknown) - 1;
		memcpy(reply_buf, unknown, size);
		return size;

	  case None:
		size = sizeof(none) - 1;
		memcpy(reply_buf, none, size);
		return size;
	}
	return size;
}

uint16_t app_process_command(const char *read_buf, uint16_t received_len, char *reply_buf)
{
	_Static_assert(APP_REPLY_BUF_SIZE >= 400, "APP_REPLY_BUF_SIZE too small for worst case");
	_Static_assert(sizeof(reader_cmd_too_long) - 1 <= APP_REPLY_BUF_SIZE, "reader_cmd_too_long larger than reply_buf");
	_Static_assert(sizeof(reader_ring_overflow) - 1 <= APP_REPLY_BUF_SIZE, "reader_ring_overflow larger than reply_buf");
	_Static_assert(CMD_BUF_SIZE >= 128, "CMD_BUF_SIZE too small for STATUS reply");

	if (reply_buf == NULL || read_buf == NULL) return 0;

	uint16_t entire_reply_len = 2;
	memcpy(reply_buf, "R:", entire_reply_len); // Reply prefix

	reader_status status = reader_read(read_buf, received_len);
	if (status == READER_ERR_CMD_TOO_LONG)
	{
		uint16_t reply_len = sizeof(reader_cmd_too_long) - 1;
		memcpy(reply_buf + entire_reply_len, reader_cmd_too_long, reply_len);
		entire_reply_len += reply_len;
	}
	else if (status == READER_ERR_RING_OVERFLOW)
	{
		uint16_t reply_len = sizeof(reader_ring_overflow) - 1;
		memcpy(reply_buf + entire_reply_len, reader_ring_overflow, reply_len);
		entire_reply_len += reply_len;
	}
	else
	{
		entire_reply_len = 0;
		reader_raw_cmd raw_cmd;
		while(reader_try_get_command(&raw_cmd))
		{
			Command command = parse(raw_cmd.cmd);
			uint16_t cmd_reply_len = app_execute_command(&command, cmd_buf);
			if (entire_reply_len + cmd_reply_len > APP_REPLY_BUF_SIZE) // discard oversized reply
				break;

			uint8_t app_reply_prefix_len = sizeof(app_reply_prefix) - 1;
			memcpy(reply_buf + entire_reply_len, app_reply_prefix, app_reply_prefix_len);
			entire_reply_len += app_reply_prefix_len;
			memcpy(reply_buf + entire_reply_len, cmd_buf, cmd_reply_len);
			entire_reply_len += cmd_reply_len;
		}
	}

	return entire_reply_len;
}

void app_init(void)
{
	encoder_init();
	motor_init();
	estimator_init(encoder_get_position());
	controller_init();
}

void app_do_control_action(void) // ISR context
{
    int32_t position = encoder_get_position();
    state.est_position = position;

    estimator_run(position);
    state.est_currentRpm = estimator_get_rpm();

    controller_set_current_rpm(state.est_currentRpm);

    if (app_mode == Auto)
    {
        state.cont_pwm = controller_get_control_value();
        controller_fill_drive_state(&state);
        motor_set_pwm(state.cont_pwm);
    }
}

uint16_t app_get_telemetry(char *telemetry_buf, uint32_t now)
{
	return telemetry_get_data(&state, telemetry_buf, now);
}

void app_do_silence_action()
{
	controller_reset();
	controller_set_target_rpm(0);
	motor_set_pwm(0);
	state.app_desiredRpm = 0;
}

