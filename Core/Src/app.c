#include "app.h"
#include "motor.h"
#include "reader.h"
#include "parser.h"
#include "encoder.h"
#include "estimator.h"
#include "telemetry.h"
#include "format.h"
#include <string.h>
#include "vel_controller.h"
#include "pos_controller.h"
#include "capture_diag.h"

#define MAX_PWM 255
#define MIN_PWM -255
#define MAX_RPM 95
#define MIN_RPM -95
#define READER_CMD_TOO_LONG "READER ERROR: COMMAND TOO LONG\n"
#define READER_RING_OVERFLOW "READER ERROR: COMMAND RING OVERFLOW\n"
#define SUCCESS "OK\n"
#define INVALID_PWM "ERROR USAGE PWM <motor_number> <pwm>\n"
#define INVALID_VEL "ERROR USAGE VEL <motor_number> <velocity_rpm>\n"
#define INVALID_POS "ERROR USAGE POS <motor_number> <position>\n"
#define INVALID_STOP "ERROR USAGE STOP <motor_number> OR STOP\n"
#define INVALID_STATUS "ERROR USAGE STATUS\n"
#define INVALID_MODE "ERROR USAGE MODE <AUTO|MANUAL>\n"
#define UNKNOWN "ERROR UNKNOWN COMMAND\n"
#define STATUS_SEPARATOR " || "
#define APP_REPLY_PREFIX "R:"
#define NONE "None\n"
#define CMD_BUF_SIZE 200

typedef enum
{
	APP_MODE_VELOCITY,
	APP_MODE_POSITION,
	APP_MODE_MANUAL,
} app_mode_t;

static const char reader_cmd_too_long[] = READER_CMD_TOO_LONG;
static const char reader_ring_overflow[] = READER_RING_OVERFLOW;
static const char success[] = SUCCESS;
static const char unknown[] = UNKNOWN;
static const char none[] = NONE;
static const char invalid_pwm[] = INVALID_PWM;
static const char invalid_vel[] = INVALID_VEL;
static const char invalid_pos[] = INVALID_POS;
static const char invalid_stop[] = INVALID_STOP;
static const char invalid_status[] = INVALID_STATUS;
static const char status_separator[] = STATUS_SEPARATOR;
static const char app_reply_prefix[] = APP_REPLY_PREFIX;
static app_mode_t app_mode = APP_MODE_MANUAL;
static char cmd_buf[CMD_BUF_SIZE] = {0};
drive_state_t state;

static uint16_t execute_pwm_cmd(command_t *command, char *reply_buf)
{
	bool invalidParams = command->motor_number != 1 || command->pwm > MAX_PWM || command->pwm < MIN_PWM;

	uint16_t size = 0;
	if (invalidParams)
	{
		size = sizeof(invalid_pwm) - 1;
		memcpy(reply_buf, invalid_pwm, size);
		return size;
	}

	app_mode = APP_MODE_MANUAL;
	state.cont_vel_pwm = (int16_t)command->pwm;
	motor_set_pwm((int16_t)command->pwm);

	size = sizeof(success) - 1;
	memcpy(reply_buf, success, size);
	return size;
}

static uint16_t execute_vel_cmd(command_t *command, char *reply_buf)
{
	bool invalidParams = command->motor_number != 1 || command->velocity > MAX_RPM || command->velocity < MIN_RPM;

	uint16_t size = 0;
	if (invalidParams)
	{
		size = sizeof(invalid_vel) - 1;
		memcpy(reply_buf, invalid_vel, size);
		return size;
	}

	app_mode = APP_MODE_VELOCITY;
	state.target_rpm = (float)(command->velocity);
	vel_controller_set_target_rpm(state.target_rpm);

	size = sizeof(success) - 1;
	memcpy(reply_buf, success, size);
	return size;
}

static uint16_t execute_pos_cmd(command_t *command, char *reply_buf)
{
	bool invalidParams = command->motor_number != 1;

	uint16_t size = 0;
	if (invalidParams)
	{
		size = sizeof(invalid_pos) - 1;
		memcpy(reply_buf, invalid_pos, size);
		return size;
	}

	app_mode = APP_MODE_POSITION;
	state.app_target_pos = command->position;
	pos_controller_set_target_pos(state.app_target_pos);

	size = sizeof(success) - 1;
	memcpy(reply_buf, success, size);
	return size;
}

static uint16_t execute_stop_cmd(command_t *command, char *reply_buf)
{
  if (command->has_motor_number && command->motor_number != 1)
  {
	  uint16_t size = sizeof(invalid_stop) - 1;
	  memcpy(reply_buf, invalid_stop, size);
	  return size;
  }

  app_mode = APP_MODE_MANUAL;
  motor_set_pwm(0);
  vel_controller_reset();
  vel_controller_fill_drive_state(&state);
  state.cont_vel_pwm = 0;
  state.target_rpm = 0;

  uint16_t size = sizeof(success) - 1;
  memcpy(reply_buf, success, size);
  return size;
}

static uint16_t app_get_full_status(char *reply_buf)
{
	size_t out_len = 0;
	size_t pos = 0;
	if (!try_copy_str(reply_buf, CMD_BUF_SIZE, "Application mode:  ", &out_len))
		return 0;

	pos = out_len;

	char * app_mode_desc = "Manual";
	if (app_mode == APP_MODE_VELOCITY)
		app_mode_desc = "Velocity control";
	else if (app_mode == APP_MODE_POSITION)
		app_mode_desc = "Position control";

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, app_mode_desc, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, status_separator, &out_len))
		return 0;

	pos += out_len;

	switch (app_mode)
	{
	case APP_MODE_VELOCITY:
		if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "Target rpm: ", &out_len))
			return 0;

		pos += out_len;

		if (!try_float_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.target_rpm, &out_len))
			return 0;

		pos += out_len;
		break;

	case APP_MODE_POSITION:
		if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "Target position: ", &out_len))
			return 0;

		pos += out_len;

		if (!try_int32_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.app_target_pos, &out_len))
			return 0;

		pos += out_len;
		break;

	case APP_MODE_MANUAL:
		if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "Pwm: ", &out_len))
			return 0;

		pos += out_len;

		if (!try_int32_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.cont_vel_pwm, &out_len))
			return 0;

		pos += out_len;
	}

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, status_separator, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "Position: ", &out_len))
		return 0;

	pos += out_len;

	if (!try_int32_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.enc_current_pos, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, status_separator, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "RPM: ", &out_len))
		return 0;

	pos += out_len;

	if (!try_float_to_str(reply_buf + pos, CMD_BUF_SIZE - pos, state.est_current_rpm, &out_len))
		return 0;

	pos += out_len;

	if (!try_copy_str(reply_buf + pos, CMD_BUF_SIZE - pos, "\n", &out_len))
		return 0;

	pos += out_len;

	return (uint16_t)pos;
}

static uint16_t app_execute_command(command_t *command, char *reply_buf)
{
	uint16_t size = 0;
	switch (command->type)
	{
	  case COMMAND_STATUS:
		return app_get_full_status(reply_buf);

	  case COMMAND_PWM:
		return execute_pwm_cmd(command, reply_buf);

	  case COMMAND_INVALID_PWM:
		size = sizeof(invalid_pwm) - 1;
		memcpy(reply_buf, invalid_pwm, size);
		return size;

	  case COMMAND_VEL:
		return execute_vel_cmd(command, reply_buf);

	  case COMMAND_INVALID_VEL:
		size = sizeof(invalid_vel) - 1;
		memcpy(reply_buf, invalid_vel, size);
		return size;

	  case COMMAND_POS:
		return execute_pos_cmd(command, reply_buf);

	  case COMMAND_INVALID_POS:
		size = sizeof(invalid_pos) - 1;
		memcpy(reply_buf, invalid_pos, size);
		return size;

	  case COMMAND_STOP:
		return execute_stop_cmd(command, reply_buf);

	  case COMMAND_INVALID_STOP:
		size = sizeof(invalid_stop) - 1;
		memcpy(reply_buf, invalid_stop, size);
		return size;

	  case COMMAND_INVALID_STATUS:
		 size = sizeof(invalid_status) - 1;
		memcpy(reply_buf, invalid_status, size);
		return size;

	  case COMMAND_UNKNOWN:
		size = sizeof(unknown) - 1;
		memcpy(reply_buf, unknown, size);
		return size;

	  case COMMAND_NONE:
		size = sizeof(none) - 1;
		memcpy(reply_buf, none, size);
		return size;
	}
	return size;
}

uint16_t app_process_command(const char *read_buf, uint16_t received_len, char *reply_buf)
{
	_Static_assert(APP_REPLY_BUF_SIZE >= 400, "APP_REPLY_BUF_SIZE too small for worst case");
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
			command_t command = parse(raw_cmd.cmd);
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
	int32_t current_pos = encoder_get_position();
	encoder_init();
	motor_init();
	estimator_init(current_pos);
	vel_controller_init();
	pos_controller_init(current_pos);
}

void app_do_control_action(void) // ISR context
{
    state.enc_current_pos = encoder_get_position();

    state.est_current_rpm = estimator_get_rpm_t();

    if (app_mode == APP_MODE_VELOCITY)
    {
        vel_controller_set_current_rpm(state.est_current_rpm);
        vel_controller_calc_control_value();
        vel_controller_fill_drive_state(&state);
        motor_set_pwm(state.cont_vel_pwm);
    }
    else if (app_mode == APP_MODE_POSITION)
    {
    	pos_controller_calc_control_value(state.enc_current_pos);
    	pos_controller_fill_drive_state(&state);
    	vel_controller_set_current_rpm(state.est_current_rpm);
    	vel_controller_set_target_rpm(state.target_rpm);
    	vel_controller_calc_control_value();
        vel_controller_fill_drive_state(&state);
        motor_set_pwm(state.cont_vel_pwm);
    }
}

void app_run_t_method(uint32_t cpu_cycles, uint32_t now)
{
	estimator_update_t(cpu_cycles, encoder_get_position(), now);
}

void app_reader_reset(void)
{
	reader_reset();
}

void app_try_reset_t_method(uint32_t now)
{
	estimator_try_reset_t_method(now, encoder_get_position());
}

uint16_t app_get_telemetry(char *telemetry_buf, uint32_t now)
{
	return telemetry_get_data(&state, telemetry_buf, now);
}

void app_do_silence_action()
{
	vel_controller_reset();
	vel_controller_set_target_rpm(0);
	motor_set_pwm(0);
	app_mode = APP_MODE_MANUAL;
	state.target_rpm = 0;
}

void app_calc_v_bat(const battery_t *b)
{
	state.bat_voltage = battery_calc_v(b);
}

