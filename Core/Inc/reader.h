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

void reader_init(UART_HandleTypeDef *huart, uint8_t *dmaBuf, uint16_t dmaBufSize);
void reader_read(uint8_t *readBuf, uint16_t rxSize);
bool reader_get_command(RawCmd * cmd);
void reader_reset();
uint32_t reader_dropped_count(void);

#endif /* INC_READER_H_ */
