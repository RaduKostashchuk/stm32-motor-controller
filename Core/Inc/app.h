#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>   // uint8_t и т.п.
#include <stddef.h>   // size_t
#include "stm32g4xx.h"
#include <stdbool.h>

#define MAX_CMD_LENGTH 12 // Extra 1 element for '\0' char

typedef struct
{
	uint8_t cmd[MAX_CMD_LENGTH];
	uint8_t len;
} RawCmd;

typedef enum
{
  Motor,
  Stop,
  Status,
  AppMode,
  InvalidMotor,
  InvalidStop,
  InvalidMode,
  InvalidStatus,
  Unknown,
  None
} CommandType;

typedef enum
{
  Auto,
  Manual
} ModeType;

typedef struct
{
  CommandType type;
  ModeType modeType;
  bool hasMotorNumber;
  long motorNumber;
  long target;
} Command;

void debug_print(const uint8_t *msg, size_t len);
void debug_print_str(const char *msg);

#ifdef __cplusplus
}
#endif

#endif
