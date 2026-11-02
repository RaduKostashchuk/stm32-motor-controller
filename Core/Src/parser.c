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
static void parseModeCmd(Token * tokens, size_t tokenCount, Command * command);
static void parseMotorCmd(Token * tokens, size_t tokenCount, Command * command);
static void parseStopCmd(Token * tokens, size_t tokenCount, Command * command);
static bool try_get_int32(const char *arg, int32_t *number);

const char *motorCmd = "MOTOR";
const char *stopCmd = "STOP";
const char *modeCmd = "MODE";
const char *autoModeCmd = "AUTO";
const char *manualModeCmd = "MANUAL";
const char *statusCmd = "STATUS";

Command parse(const char *raw_cmd)
{
  Token tokens[MAX_CMD_PARTS] = {0};
  size_t tokenCount = 0;
  tokenize(tokens, &tokenCount, raw_cmd);

  Command command = {
		.type = Unknown,
		.modeType = Auto,
		.hasMotorNumber = false,
		.motorNumber = 0,
		.target = 0
  };

  const uint8_t statusCmdLen = 6;
  if (strcmp((const char *)tokens[0].content, motorCmd) == 0)
	parseMotorCmd(tokens, tokenCount, &command);
  else if (strcmp((const char *)tokens[0].content, stopCmd) == 0)
	parseStopCmd(tokens, tokenCount, &command);
  else if (strcmp((const char *)tokens[0].content, modeCmd) == 0)
	parseModeCmd(tokens, tokenCount, &command);
  else if (strcmp((const char *)tokens[0].content, statusCmd) == 0)
	command.type = tokenCount == 1 && tokens[0].length == statusCmdLen ? Status : InvalidStatus;

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

static void parseModeCmd(Token * tokens, size_t tokenCount, Command * command)
{
  const uint8_t modeCmdLen = 4;
  command->type = InvalidMode;
  if (tokenCount != 2 || (*tokens).length != modeCmdLen)
	return;

  const uint8_t autoCmdLen = 4;
  const uint8_t manualCmdLen = 6;
  Token secondArg = *(tokens + 1);
  if (strcmp((const char *)secondArg.content, autoModeCmd) == 0 && secondArg.length == autoCmdLen)
  {
	command->modeType = Auto;
	command->type = AppMode;
  }
  else if (strcmp((const char *)secondArg.content, manualModeCmd) == 0 && secondArg.length == manualCmdLen)
  {
	command->modeType = Manual;
	command->type = AppMode;
  }
}

static void parseMotorCmd(Token * tokens, size_t tokenCount, Command * command)
{
  const uint8_t motorCmdLen = 5;
  command->type = InvalidMotor;
  if (tokenCount != 3 || (*tokens).length != motorCmdLen || (*(tokens + 2)).length == 0)
	return;

  char * secondArg = (*(tokens + 1)).content;
  int32_t motorNumber = 0;
  if (try_get_int32(secondArg, &motorNumber))
	command->motorNumber = motorNumber;
  else
	return;

  char * thirdArg = (*(tokens + 2)).content;
  int32_t target = 0;
  if (try_get_int32(thirdArg, &target))
  {
	command->type = Motor;
	command->target = target;
  }
}

static void parseStopCmd(Token * tokens, size_t tokenCount, Command * command)
{
  const uint8_t stopCmdLen = 4;
  command->type = InvalidStop;
  if (tokenCount > 2 || (*tokens).length != stopCmdLen)
	return;

  Token * secondArg = tokens + 1;
  int32_t motorNumber = 0;
  if ((*secondArg).length == 0)
  {
	command->type = Stop;
  }
  else if (try_get_int32((*secondArg).content, &motorNumber))
  {
	command->type = Stop;
	command->motorNumber = motorNumber;
	command->hasMotorNumber = true;
  }
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
