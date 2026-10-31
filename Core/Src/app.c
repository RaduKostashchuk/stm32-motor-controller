#include "app.h"
#include "controller.h"
#include "motor.h"
#include "reader.h"
#include "parser.h"
#include "encoder.h"
#include "estimator.h"
#include "telemetry.h"
#include "format.h"

#define MAX_PWM 255
#define MIN_PWM -255
#define MAX_RPM 95
#define MIN_RPM 0

static const char *success = "OK";
static const char *unknown = "ERROR UNKNOWN COMMAND";
static const char *none = "None";
static const char *invalid_motor = "ERROR USAGE MOTOR <motor number> <pwm OR rpm>";
static const char *invalid_stop = "ERROR USAGE STOP <motor number> OR STOP";
static const char *invalid_mode = "ERROR USAGE MODE <AUTO|MANUAL>";
static const char *invalid_status = "ERROR USAGE STATUS";
static ModeType app_mode = Auto;
static DriveState state;
static UART_HandleTypeDef * telemetry_uart;

void debug_print(const uint8_t *msg, size_t len)
{
	for (uint16_t i = 0; i < len; i++)
		ITM_SendChar(msg[i]);
}

void debug_print_str(const char *msg)
{
	while(*msg)
		ITM_SendChar(*msg++);
}

void debug_print_int16(int16_t num)
{
	bool isNegative = num < 0;
	uint16_t absNum;

	if (isNegative)
	{
		absNum = (uint16_t)(-(int32_t)num);
		ITM_SendChar('-');
	}
	else
	{
		absNum = (uint16_t)num;
	}

	uint16_t divider = 10000;
	bool allZeroes = true;
	while(divider > 0)
	{
		if (absNum >= divider)
		{
			uint8_t cipher = (uint8_t)(absNum / divider);
			allZeroes = false;
			ITM_SendChar((char)(cipher + '0'));
			absNum %= divider;
		}
		else if (!allZeroes)
		{
			ITM_SendChar('0');
		}
		divider /= 10;
	}

	if (allZeroes)
		ITM_SendChar('0');
}

void debug_print_int32(int32_t num)
{
    bool isNegative = num < 0;
    uint32_t absNum;

    if (isNegative)
    {
        absNum = (uint32_t)(-(int64_t)num);  // безопасно для INT32_MIN
        ITM_SendChar('-');
    }
    else
    {
        absNum = (uint32_t)num;
    }

    uint32_t divider = 1000000000;  // 10^9, тип uint32_t
    bool allZeroes = true;

    while (divider > 0)
    {
        if (absNum >= divider)
        {
            uint8_t cipher = (uint8_t)(absNum / divider);
            allZeroes = false;
            ITM_SendChar((char)(cipher + '0'));
            absNum %= divider;
        }
        else if (!allZeroes)
        {
            ITM_SendChar('0');
        }
        divider /= 10;
    }

    if (allZeroes)
        ITM_SendChar('0');
}

void debug_print_float(float num)
{
	const size_t bufSize = 20;
	char buf[bufSize];
	float_to_char_dec(buf, bufSize, num);
	debug_print_str(buf);
}

void execute_mode_cmd(Command *command)
{
  app_mode = command->modeType;
  debug_print_str(success);
}

void execute_motor_cmd(Command *command, DriveState *state)
{
  bool invalidParams = command->motorNumber != 1
    || (app_mode == Auto && (command->target > MAX_RPM || command->target < MIN_RPM))
    || (app_mode == Manual && (command->target > MAX_PWM || command->target < MIN_PWM));

  if (invalidParams)
  {
	debug_print_str(invalid_motor);
    return;
  }

  if (command->motorNumber == 1)
  {
    if (app_mode == Auto)
    {
    	state->app_desiredRpm = (float)(command->target);
    }
    else if (app_mode == Manual)
    {
    	motor_set_pwm(state->cont_pwm);
    }
  }
  debug_print_str(success);
}

void execute_stop_cmd(Command *command, DriveState *state)
{
  if (command->hasMotorNumber && command->motorNumber != 1)
  {
	debug_print_str(invalid_stop);
    return;
  }

  controller_reset();
  motor_set_pwm(0);
  state->app_desiredRpm = 0;

  debug_print_str(success);
}

void app_send_full_status(DriveState * state)
{
	debug_print_str("Application mode:  ");
	debug_print_str(app_mode == Auto ? "Auto" : "Manual");

	debug_print_str(";  Last motor cmd:  ");
  switch (app_mode)
  {
    case Auto:
      debug_print_str("Target Rpm: ");
      debug_print_float(state->app_desiredRpm);
      break;

    case Manual:
      debug_print_str("Pwm: ");
      debug_print_int16(state->cont_pwm);
  }

  debug_print_str("\n");

  debug_print_str("Encoder:  ");
  debug_print_str("Position: ");
  debug_print_int32(state->est_position);

  debug_print_str(" ||  Estimator:  ");
  debug_print_str(";  RPM: ");
  debug_print_float(state->est_currentRpm);

  debug_print_str("\n");
}

void app_execute_command(Command *command, DriveState *state)
{
	switch (command->type)
	{
	  case Status:
		app_send_full_status(state);
		break;

	  case AppMode:
		execute_mode_cmd(command);
		break;

	  case Motor:
		execute_motor_cmd(command, state);
		break;

	  case Stop:
		  execute_stop_cmd(command, state);
		break;

	  case InvalidMotor:
		debug_print_str(invalid_motor);
		break;

	  case InvalidMode:
		debug_print_str(invalid_mode);
		break;

	  case InvalidStop:
		debug_print_str(invalid_stop);
		break;

	  case InvalidStatus:
		debug_print_str(invalid_status);
		break;

	  case Unknown:
		debug_print_str(unknown);
		break;

	  case None:
		debug_print_str(none);
		break;
	}
}

void app_process_command(const uint8_t *readBuf, uint16_t rxSize)
{
	reader_read(readBuf, rxSize);
	RawCmd rawCmd;
	while(reader_get_command(&rawCmd))
	{
		Command command = parse(&rawCmd);

		// app_execute_command
	}
}

void app_init(void)
{
	encoder_init();

	motor_init();

	estimator_init(encoder_get_position());

	controller_init();
}

void app_do_control_action(void)
{
	estimator_run(encoder_get_position());
	controller_set_current_rpm(estimator_get_rpm());
	motor_set_pwm(controller_get_contol_value());
}

char * app_get_telemetry(size_t * buf_len)
{
	return telemetry_get_data(&state, buf_len);
}

