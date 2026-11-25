#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "app.h"
#include <errno.h>
#define MAX_CMD_PARTS 3
#define PWM_CMD "PWM"
#define PWM_CMD_LEN (sizeof(PWM_CMD) - 1)
#define VEL_CMD "VEL"
#define VEL_CMD_LEN (sizeof(VEL_CMD) - 1)
#define POS_CMD "POS"
#define POS_CMD_LEN (sizeof(POS_CMD) - 1)
#define STOP_CMD "STOP"
#define STOP_CMD_LEN (sizeof(STOP_CMD) - 1)
#define STATUS_CMD "STATUS"
#define STATUS_CMD_LEN (sizeof(STATUS_CMD) - 1)

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

command_t parse(const char *raw_cmd)
{
  Token tokens[MAX_CMD_PARTS] = {0};
  size_t token_count = 0;
  tokenize(tokens, &token_count, raw_cmd);

  command_t command = {
		.type = COMMAND_UNKNOWN,
		.motor_number = 0,
		.pwm = 0,
		.velocity = 0,
		.position = 0
  };

  if (strcmp((const char *)tokens[0].content, PWM_CMD) == 0)
	parse_pwm_cmd(tokens, token_count, &command);
  else if (strcmp((const char *)tokens[0].content, VEL_CMD) == 0)
	parse_vel_cmd(tokens, token_count, &command);
  else if (strcmp((const char *)tokens[0].content, POS_CMD) == 0)
	parse_pos_cmd(tokens, token_count, &command);
  else if (strcmp((const char *)tokens[0].content, STOP_CMD) == 0)
	parse_stop_cmd(tokens, token_count, &command);
  else if (strcmp((const char *)tokens[0].content, STATUS_CMD) == 0)
	command.type = token_count == 1 && tokens[0].length == STATUS_CMD_LEN ? COMMAND_STATUS : COMMAND_INVALID_STATUS;

  return command;
}

static void tokenize(Token * tokens, size_t * token_count, const char *raw_cmd)
{
	size_t i = 0;
	*token_count = 0;
	bool is_prev_space = false;

	while (*raw_cmd == ' ')
		raw_cmd++;

	while (*raw_cmd != '\0')
	{
		if (*raw_cmd == ' ' && !is_prev_space)
		{
			(*tokens).content[i] = '\0';
			tokens++;
			i = 0;
			is_prev_space = true;
		}
		else if (*raw_cmd != ' ')
		{
			if (i == 0)
			  (*token_count)++;

			if (*token_count > MAX_CMD_PARTS)
				return;

			if (i < MAX_CMD_LENGTH - 1)
			{
				(*tokens).content[i++] = *raw_cmd;
				(*tokens).length = i;
			}
			is_prev_space = false;
		}
		raw_cmd++;
	}

	if (*token_count != 0 && !is_prev_space)
		(*tokens).content[i] = '\0';
}

static void parse_pwm_cmd(Token * tokens, size_t token_count, command_t * command)
{
	command->type = COMMAND_INVALID_PWM;

	int32_t motor_number = 0;
	int32_t pwm = 0;
	if (!try_get_second_third_args(PWM_CMD_LEN, tokens, token_count, &motor_number, &pwm))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_PWM;
	command->pwm = pwm;
}

static void parse_vel_cmd(Token * tokens, size_t token_count, command_t * command)
{
	command->type = COMMAND_INVALID_VEL;

	int32_t motor_number = 0;
	int32_t velocity = 0;
	if (!try_get_second_third_args(VEL_CMD_LEN, tokens, token_count, &motor_number, &velocity))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_VEL;
	command->velocity = velocity;
}

static void parse_pos_cmd(Token * tokens, size_t token_count, command_t * command)
{
	command->type = COMMAND_INVALID_POS;

	int32_t motor_number = 0;
	int32_t position = 0;
	if (!try_get_second_third_args(POS_CMD_LEN, tokens, token_count, &motor_number, &position))
		return;

	command->motor_number = motor_number;
	command->type = COMMAND_POS;
	command->position = position;
}

static void parse_stop_cmd(Token * tokens, size_t token_count, command_t * command)
{
  command->type = COMMAND_INVALID_STOP;
  if (token_count > 2 || (*tokens).length != STOP_CMD_LEN)
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
  }
}

static bool try_get_second_third_args(uint8_t cmd_len, Token * tokens, size_t token_count, int32_t * second, int32_t * third)
{
	if (token_count != 3 || (*tokens).length != cmd_len || (*(tokens + 2)).length == 0)
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
