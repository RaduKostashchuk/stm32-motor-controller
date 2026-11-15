#ifndef __CAPTURE_DIAG_H__
#define __CAPTURE_DIAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CAPTURE_DRAIN_INTERVAL 25

typedef struct
{
	uint32_t value_i;
	float value_f;
} capture_diag_t;

void capture_diag_push(capture_diag_t value);
bool capture_diag_pop(capture_diag_t *value);
uint32_t capture_diag_get_dropped(void);
void debug_print_str(const char *msg);
void debug_print(char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __CAPTURE_DIAG_H__ */
