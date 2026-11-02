/*
 * reader.h
 *
 *  Created on: 15 окт. 2026 г.
 *      Author: user
 */

#ifndef INC_READER_H_
#define INC_READER_H_

#include <stdbool.h>
#include "app.h"

typedef enum {
    READER_OK,
    READER_ERR_RING_OVERFLOW,
    READER_ERR_CMD_TOO_LONG,
} reader_status;

typedef struct
{
	uint8_t cmd[MAX_CMD_LENGTH];
	uint8_t len;
} reader_raw_cmd;

reader_status reader_read(const char *readBuf, uint16_t rxSize);
bool reader_try_get_command(reader_raw_cmd *raw_cmd);
void reader_reset(void);
uint32_t reader_dropped_count(void);

#endif /* INC_READER_H_ */
