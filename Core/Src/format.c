#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool try_uint32_to_str(char *buf, size_t buf_size, uint32_t value, size_t *out_len)
{
    if (buf == NULL || buf_size == 0)
        return false;

    buf[0] = '\0';

    size_t digits = 1;
    for (uint32_t v = value; v >= 10; v /= 10)
        digits++;

    if (digits + 1 > buf_size)
        return false;

    size_t i = digits;
    buf[i] = '\0';
    do {
        buf[--i] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);

    if (out_len != NULL)
        *out_len = digits;

    return true;
}

bool try_int32_to_str(char *buf, size_t buf_size, int32_t value, size_t *out_len)
{
	if (buf == NULL || buf_size == 0)
		return false;

	buf[0] = '\0';

    uint32_t absNum;
    size_t len = 0;

    if (value < 0)
    {
    	if (buf_size < 3) // sign, one digit, '\0'
    		return false;

        absNum = (uint32_t)(-(int64_t)value);  // safe for INT32_MIN
        buf[len++] = (char)'-';
    }
    else
    {
        absNum = (uint32_t)value;
    }

    size_t copy_len = 0;
    if (!try_uint32_to_str(&buf[len], buf_size - len, absNum, &copy_len))
    {
    	buf[0] = '\0';
    	return false;
    }

    if (out_len != NULL)
    	*out_len = len + copy_len;

    return true;
}

bool try_float_to_str(char *buf, size_t buf_size, float value, size_t *out_len)
{
	if (buf == NULL || buf_size == 0)
		return false;

	buf[0] = '\0';

	if (buf_size < 15)  // sign + 10 (int part) + . + 2 (frac part) + '\0'
		return false;

    if (!(value > -4294967296.0f && value < 4294967296.0f))
        return false;

    size_t len = 0;
    if (value < 0.0f)
    {
    	value = -value;
        buf[len++] = '-';
    }

    uint32_t int_part = (uint32_t)value;

    uint32_t frac_part = (uint32_t)((value - (float)int_part) * 100.0f + 0.5f); // round up to two signs
    if (frac_part >= 100)
    {
        if (int_part == UINT32_MAX)
            return false;

        frac_part -= 100;
        int_part++;
    }

    size_t copy_len = 0;
    if (!try_uint32_to_str(&buf[len], buf_size - len, int_part, &copy_len))
    {
    	buf[0] = '\0';
    	return false;
    }

    len += copy_len;
    buf[len++] = '.'; //overwrite '\0'

    buf[len++] = (char)('0' + frac_part / 10);
    buf[len++] = (char)('0' + frac_part % 10);
    buf[len] = '\0';

    if (out_len != NULL)
    	*out_len = len;

    return true;
}

bool try_copy_str(const char *src, char *dst, size_t dst_size, size_t *out_len)
{
    if (dst == NULL || dst_size == 0)
        return false;

    dst[0] = '\0';

    if (src == NULL)
        return false;

    size_t i = 0;
    while (i + 1 < dst_size && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';

    if (src[i] != '\0')
        return false;

    if (out_len != NULL)
        *out_len = i;

    return true;
}
