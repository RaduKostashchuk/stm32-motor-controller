#include <stdint.h>
#include <stdbool.h>
#include "app.h"
#include "reader.h"
#define MAX_CMD_COUNT 16 // MUST be power of two. Actual capacity MAX_CMD_COUNT - 1
#define TERMINATOR1 '\n'
#define TERMINATOR2 '\r'

typedef enum
{
  Idle,
  Reading,
  Discarding
} State;

static bool push(reader_raw_cmd cmd);
static bool pop(reader_raw_cmd *value);
static reader_status handleIdle(const char *readBuf, uint16_t rxSize);
static reader_status handleReading(const char *readBuf, uint16_t rxSize);
static void handleDiscarding(const char *readBuf, uint16_t rxSize);
static bool isTerminator(uint8_t ch);

static reader_raw_cmd currCmd;
static reader_raw_cmd cmd_ring[MAX_CMD_COUNT];
static uint8_t head = 0;
static uint8_t tail = 0;
static uint32_t dropped = 0;
static State state = Idle;

// There are two modes:
// - one character per buffer
// - several commands per buffer
// state needed for single character mode only

reader_status reader_read(const char *readBuf, uint16_t rxSize)
{
	reader_status status = READER_OK;

	if (rxSize == 0 || readBuf == NULL)
		return status;

	switch (state)
	{
		case Idle:
			status = handleIdle(readBuf, rxSize);
			break;
		case Reading:
			status = handleReading(readBuf, rxSize);
			break;
		case Discarding:
			handleDiscarding(readBuf, rxSize);
			break;
	}

	return status;
}

static reader_status handleIdle(const char *readBuf, uint16_t rxSize)
{
	reader_status status = READER_OK;
	currCmd.len = 0;

	if (rxSize != 0)
	{
		state = Reading;
		status = handleReading(readBuf, rxSize);
	}

	return status;
}

static reader_status handleReading(const char *readBuf, uint16_t rxSize)
{
	reader_status status = READER_OK;
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
				status = READER_ERR_RING_OVERFLOW;

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
			status = READER_ERR_CMD_TOO_LONG;
		}
	}

	if (discarding)
		state = Discarding;
	else if (isTerminator(lastCh))
		state = Idle;

	return status;
}

static bool isTerminator(uint8_t ch)
{
	return ch == TERMINATOR1 || ch == TERMINATOR2;
}

static void handleDiscarding(const char *readBuf, uint16_t rxSize)
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

bool reader_try_get_command(reader_raw_cmd *(cmd))
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

static bool push(reader_raw_cmd cmd)
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

static bool pop(reader_raw_cmd *value)
{
  if (head == tail)
    return false;

  *value = cmd_ring[tail];
  tail = (tail + 1) & (MAX_CMD_COUNT - 1);
  return true;
}
