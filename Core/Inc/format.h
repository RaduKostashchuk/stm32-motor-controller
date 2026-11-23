#ifndef INC_FORMAT_H
#define INC_FORMAT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool try_uint32_to_str(char *buf, size_t buf_size, uint32_t value, size_t *out_len);
bool try_int32_to_str(char *buf, size_t buf_size, int32_t value, size_t *out_len);
bool try_float_to_str(char *buf, size_t buf_size, float value, size_t *out_len);
bool try_copy_str(char *dst, size_t dst_size, const char *src, size_t *out_len);

#endif /* INC_FORMAT_H */
