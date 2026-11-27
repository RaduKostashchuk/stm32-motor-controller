#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "battery.h"

#define RX_BUF_SIZE 64 // Must be several MAX_CMD_LENGTH
#define MAX_CMD_LENGTH 18 // "POS 1 -2147483648" plus '\0' char
#define TELEMETRY_BUF_SIZE 160
#define APP_REPLY_BUF_SIZE 400
#define UART_MAX_SILENCE_DURATION_MS 60000
#define TELEMETRY_INTERVAL_MS 50
#define BATTERY_CHECK_INTERVAL_MS 500
#define DELTA_TIME_SEC 0.01f // controller and estimator freq
#define MIN_MOTOR_RPM 9.0f

typedef enum
{
    COMMAND_PWM,
	COMMAND_VEL,
	COMMAND_POS,
    COMMAND_STOP,
    COMMAND_STATUS,
    COMMAND_INVALID_PWM,
	COMMAND_INVALID_VEL,
	COMMAND_INVALID_POS,
    COMMAND_INVALID_STOP,
    COMMAND_INVALID_STATUS,
    COMMAND_UNKNOWN,
    COMMAND_NONE
} command_type_t;

typedef struct
{
	command_type_t type;
	int32_t motor_number;
	int32_t pwm;
	int32_t velocity;
	int32_t position;
} command_t;

typedef struct
{
	float target_rpm;
	float est_current_rpm;
	float cont_vel_error;
	float cont_vel_p;
	float cont_vel_i;
	float cont_vel_d;
	float cont_vel_uff;
	int32_t app_target_pos;
	int32_t enc_current_pos;
	int32_t cont_pos_error;
	int16_t cont_vel_pwm;
	float bat_voltage;
} drive_state_t;

uint16_t app_process_command(const char *read_buf, uint16_t received_len, char *reply_buf);
void app_init(void);
void app_do_control_action(void);
uint16_t app_get_telemetry(char *telemetry_buf, uint32_t now);
void app_do_silence_action();
void app_run_t_method(uint32_t cpu_cycles, uint32_t now);
void app_reader_reset(void);
void app_try_reset_t_method(uint32_t now);
void app_calc_v_bat(const battery_t *b);


#ifdef __cplusplus
}
#endif

#endif
