#include <stdint.h>
#include <stdbool.h>
#include "app.h"
#include "stm32g4xx_hal_uart_ex.h"
#define MAX_CMD_COUNT 16 // MUST be power of two. Actual capacity MAX_CMD_COUNT - 1
#define TERMINATOR1 '\n'
#define TERMINATOR2 '\r'

typedef enum
{
  Idle,
  Reading,
  Discarding
} State;

static bool push(RawCmd cmd);
static bool pop(RawCmd *value);
static void handleIdle(const uint8_t *readBuf, uint16_t rxSize);
static void handleReading(const uint8_t *readBuf, uint16_t rxSize);
static void handleDiscarding(const uint8_t *readBuf, uint16_t rxSize);
static bool isTerminator(uint8_t ch);

static const char * const cmd_too_long = "READER ERROR: COMMAND TOO LONG\n";
static const char * const ring_overflow = "READER ERROR: COMMAND RING OVERFLOW\n";

static RawCmd currCmd;
static RawCmd cmd_ring[MAX_CMD_COUNT];
static uint8_t head = 0;
static uint8_t tail = 0;
static uint32_t dropped = 0;
static State state = Idle;

void reader_init(UART_HandleTypeDef *huart, uint8_t *dmaBuf)
{
	 HAL_UARTEx_ReceiveToIdle_DMA(huart, dmaBuf, RX_BUFFER_SIZE);
}

// There are two modes:
// - one character per buffer
// - several commands per buffer
// state needed for single character mode only

void reader_read(const uint8_t *readBuf, uint16_t rxSize)
{
	if (rxSize == 0 || readBuf == NULL)
		return;

	switch (state)
	{
		case Idle:
			handleIdle(readBuf, rxSize);
			break;
		case Reading:
			handleReading(readBuf, rxSize);
			break;
		case Discarding:
			handleDiscarding(readBuf, rxSize);
			break;
	}
}

static void handleIdle(const uint8_t *readBuf, uint16_t rxSize)
{
	currCmd.len = 0;
	if (rxSize != 0)
	{
		state = Reading;
		handleReading(readBuf, rxSize);
	}
}

static void handleReading(const uint8_t *readBuf, uint16_t rxSize)
{
	uint8_t lastCh = 0;
	bool discarding = false; // for discarding long command within buffer only
	for (uint16_t i = 0; i < rxSize; i++)
	{
		lastCh = readBuf[i];
		if (isTerminator(lastCh))
		{
			if (discarding)
				discarding = false;
			else if (currCmd.len != 0 && !push(currCmd))
				debug_print_str(ring_overflow);

			currCmd.len = 0;
			continue;
		}

		if (discarding)
			continue;

		if (currCmd.len < MAX_CMD_LENGTH - 1)
		{
			currCmd.cmd[currCmd.len++] = lastCh;
		}
		else
		{
			discarding = true;
			debug_print_str(cmd_too_long);
		}
	}

	if (discarding)
		state = Discarding;
	else if (isTerminator(lastCh))
		state = Idle;
}

static bool isTerminator(uint8_t ch)
{
	return ch == TERMINATOR1 || ch == TERMINATOR2;
}

static void handleDiscarding(const uint8_t *readBuf, uint16_t rxSize)
{
    for (uint16_t i = 0; i < rxSize; i++)
    {
        if (isTerminator(readBuf[i]))
        {
        	state = Idle;
        	if (i + 1 < rxSize)
        		handleIdle(readBuf + i + 1, (uint16_t)(rxSize - i - 1));

            return;
        }
    }
}

bool reader_get_command(RawCmd * cmd)
{
	return pop(cmd);
}

void reader_reset()
{
    state = Idle;
    currCmd.len = 0;
    head = 0;
    tail = 0;
    dropped = 0;
}

uint32_t reader_dropped_count(void)
{
    return dropped;
}

static bool push(RawCmd cmd)
{
	uint8_t next = (head + 1) & (MAX_CMD_COUNT - 1);
	if (next == tail)
	{
	  dropped++;
	  return false;
	}

	cmd.cmd[cmd.len] = '\0';
	cmd_ring[head] = cmd;
	head = next;
	return true;
}

static bool pop(RawCmd *value)
{
  if (head == tail)
    return false;

  *value = cmd_ring[tail];
  tail = (tail + 1) & (MAX_CMD_COUNT - 1);
  return true;
}
