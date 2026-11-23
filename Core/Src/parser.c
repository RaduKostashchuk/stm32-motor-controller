#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "app.h"
#include <errno.h>
#define MAX_CMD_PARTS 3

typedef struct
{
  char content[MAX_CMD_LENGTH];
  size_t length;
} Token;

static void tokenize(Token * tokens, size_t * tokenCount, const char *raw_cmd);
static void parse_pwm_cmd(Token * tokens, size_t tokenCount, command_t * command);
static void parse_vel_cmd(Token * tokens, size_t tokenCount, command_t * command);
static void parse_pos_cmd(Token * tokens, size_t tokenCount, command_t * command);
static void parse_stop_cmd(Token * tokens, size_t tokenCount, command_t * command);
static bool try_get_second_third_args(uint8_t cmd_len, Token * tokens, size_t tokenCount, int32_t * second, int32_t * third);
static bool try_get_int32(const char *arg, int32_t *number);

const char *pwm_cmd = "PWM";
const char *vel_cmd = "VEL";
const char *pos_cmd = "POS";
const char *stop_cmd = "STOP";
const char *status_cmd = "STATUS";

command_t parse(const char *raw_cmd)
{
  Token tokens[MAX_CMD_PARTS] = {0};
  size_t tokenCount = 0;
  tokenize(tokens, &tokenCount, raw_cmd);

  command_t command = {
		.type = COMMAND_UNKNOWN,
		.has_motor_number = false,
		.motor_number = 0,
		.pwm = 0,
		.velocity = 0,
		.position = 0
  };

  const uint8_t status_cmd_len = 6;
  if (strcmp((const char *)tokens[0].content, pwm_cmd) == 0)
	parse_pwm_cmd(tokens, tokenCount, &command);
  if (strcmp((const char *)tokens[0].content, vel_cmd) == 0)
	parse_vel_cmd(tokens, tokenCount, &command);
  if (strcmp((const char *)tokens[0].content, pos_cmd) == 0)
	parse_pos_cmd(tokens, tokenCount, &command);
  else if (strcmp((const char *)tokens[0].content, stop_cmd) == 0)
	parse_stop_cmd(tokens, tokenCount, &command);
  else if (strcmp((const char *)tokens[0].content, status_cmd) == 0)
	command.type = tokenCount == 1 && tokens[0].length == status_cmd_len ? COMMAND_STATUS : COMMAND_INVALID_STATUS;

  return command;
}

static void tokenize(Token * tokens, size_t * tokenCount, const char *raw_cmd)
{
	size_t i = 0;
	*tokenCount = 0;
	bool isPrevSpace = false;

	while (*raw_cmd == ' ')
		raw_cmd++;

	while (*raw_cmd != '\0')
	{
		if (*raw_cmd == ' ' && !isPrevSpace)
		{
			(*tokens).content[i] = '\0';
			tokens++;
			i = 0;
			isPrevSpace = true;
		}
		else if (*raw_cmd != ' ')
		{
			if (i == 0)
			  (*tokenCount)++;

			if (*tokenCount > MAX_CMD_PARTS)
				return;

			if (i < MAX_CMD_LENGTH - 1)
			{
				(*tokens).content[i++] = *raw_cmd;
				(*tokens).length = i;
			}
			isPrevSpace = false;
		}
		raw_cmd++;
	}

	if (*tokenCount != 0 && !isPrevSpace)
		(*tokens).content[i] = '\0';
}

static void parse_pwm_cmd(Token * tokens, size_t tokenCount, command_t * command)
{
	const uint8_t cmd_len = 3;
	command->type = COMMAND_INVALID_PWM;

	int32_t motor_number = 0;
	int32_t pwm = 0;
	if (!try_get_second_third_args(cmd_len, tokens, tokenCount, &motor_number, &pwm))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_PWM;
	command->pwm = pwm;
}

static void parse_vel_cmd(Token * tokens, size_t tokenCount, command_t * command)
{
	const uint8_t cmd_len = 3;
	command->type = COMMAND_INVALID_VEL;

	int32_t motor_number = 0;
	int32_t velocity = 0;
	if (!try_get_second_third_args(cmd_len, tokens, tokenCount, &motor_number, &velocity))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_VEL;
	command->velocity = velocity;
}

static void parse_pos_cmd(Token * tokens, size_t tokenCount, command_t * command)
{
	const uint8_t cmd_len = 3;
	command->type = COMMAND_INVALID_POS;

	int32_t motor_number = 0;
	int32_t position = 0;
	if (!try_get_second_third_args(cmd_len, tokens, tokenCount, &motor_number, &position))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_POS;
	command->position = position;
}

static void parse_stop_cmd(Token * tokens, size_t tokenCount, command_t * command)
{
  const uint8_t cmd_len = 4;
  command->type = COMMAND_INVALID_STOP;
  if (tokenCount > 2 || (*tokens).length != cmd_len)
	return;

  Token * secondArg = tokens + 1;
  int32_t motor_number = 0;
  if ((*secondArg).length == 0)
  {
	command->type = COMMAND_STOP;
  }
  else if (try_get_int32((*secondArg).content, &motor_number) && motor_number > 0)
  {
	command->type = COMMAND_STOP;
	command->motor_number = (uint8_t)motor_number;
	command->has_motor_number = true;
  }
}

static bool try_get_second_third_args(uint8_t cmd_len, Token * tokens, size_t tokenCount, int32_t * second, int32_t * third)
{
	if (tokenCount != 3 || (*tokens).length != cmd_len || (*(tokens + 2)).length == 0)
		return false;

	char * secondArg = (*(tokens + 1)).content;
	if (!try_get_int32(secondArg, second))
		return false;

	char * thirdArg = (*(tokens + 2)).content;
	return try_get_int32(thirdArg, third);
}

static bool try_get_int32(const char *arg, int32_t *number)
{
	if (arg == NULL || number == NULL)
		return false;

	char *end;
	errno = 0;

	long value = strtol(arg, &end, 10);

	if (end == arg || *end != '\0') // no numbers
		return false;

	if (errno == ERANGE) // overflow
		return false;

	if (value < INT32_MIN || value > INT32_MAX)
		return false;

	*number = (int32_t)value;
	return true;
}
