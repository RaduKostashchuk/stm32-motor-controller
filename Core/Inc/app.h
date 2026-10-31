#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>   // uint8_t и т.п.
#include <stddef.h>   // size_t
#include <stdbool.h>

#define RX_BUFFER_SIZE 64 // Must be several MAX_CMD_LENGTH
#define MAX_CMD_LENGTH 13 // Extra 1 element for '\0' char

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

// Modetype == Auto, target { -95. 95 } RPMs, Modetype == manual, target { -250. 250 } PWM
typedef struct
{
  CommandType type;
  ModeType modeType;
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
} DriveState;

void debug_print(const uint8_t *msg, size_t len);
void debug_print_str(const char *msg);
void debug_print_int16(int16_t num);
void debug_print_int32(int32_t num);
void debug_print_float(float num);
void app_process_command(const uint8_t *readBuf, uint16_t rxSize);
void app_init(void);
void app_do_control_action(void);
char *app_get_telemetry(size_t * buf_len);

#ifdef __cplusplus
}
#endif

#endif
