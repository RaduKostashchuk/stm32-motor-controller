#include <stdint.h>
#include <stdbool.h>
#include <capture_diag.h>
#include <main.h>

#define CAPTURE_BUF_SIZE 512

_Static_assert((CAPTURE_BUF_SIZE & (CAPTURE_BUF_SIZE - 1)) == 0, "CAPTURE_BUF_SIZE must be power of 2");
_Static_assert(CAPTURE_BUF_SIZE <= 65536, "CAPTURE_BUF_SIZE too large for uint16_t head/tail");

static volatile capture_diag_t capture_buf[CAPTURE_BUF_SIZE];
static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;
static volatile uint32_t dropped = 0;

void capture_diag_push(capture_diag_t value)
{
	uint8_t next = (uint8_t)((head + 1) & (CAPTURE_BUF_SIZE - 1));
	if (next == tail)
	{
	  dropped++;
	  return;
	}

	capture_buf[head] = value;
	head = next;
}

bool capture_diag_pop(capture_diag_t *value)
{
  if (head == tail)
    return false;

  *value = capture_buf[tail];
  tail = (tail + 1) & (CAPTURE_BUF_SIZE - 1);
  return true;
}

uint32_t capture_diag_get_dropped(void)
{
	return dropped;
}

void debug_print_str(const char *msg)
{
	while (*msg)
		ITM_SendChar(*msg++);
}

void debug_print(char *buf, size_t len)
{
	for (size_t i = 0; i < len; i++)
		ITM_SendChar(buf[i]);
}
