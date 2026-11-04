#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>   // uint8_t и т.п.
#include <stddef.h>   // size_t
#include <stdbool.h>

#define RX_BUF_SIZE 64 // Must be several MAX_CMD_LENGTH
#define MAX_CMD_LENGTH 13 // Extra 1 element for '\0' char
#define TELEMETRY_BUF_SIZE 160
#define APP_REPLY_BUF_SIZE 400

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
} command_type;

typedef enum
{
  Auto,
  Manual
} mode_type;

// Modetype == Auto, target { -95. 95 } RPMs, Modetype == manual, target { -250. 250 } PWM
typedef struct
{
  command_type type;
  mode_type modeType;
  bool hasMotorNumber;
  int32_t motorNumber;
  int32_t target;
} Command;

typedef struct
{
	float cont_error;
	float cont_p;
	float cont_i;
	float cont_d;
	float cont_uff;
	int16_t cont_pwm;
	int32_t est_position;
	float est_currentRpm;
	float app_desiredRpm;
} drive_state;

uint16_t app_process_command(const char *read_buf, uint16_t received_len, char *reply_buf);
void app_init(void);
void app_do_control_action(void);
uint16_t app_get_telemetry(char *telemetry_buf, uint32_t now);

#ifdef __cplusplus
}
#endif

#endif
