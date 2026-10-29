#ifndef INC_FORMAT_H_
#define INC_FORMAT_H_

#include <stdint.h>
#include <stdbool.h>

bool try_uint32_to_str(char *buf, size_t buf_size, uint32_t value, size_t *out_len);
bool try_int32_to_str(char *buf, size_t buf_size, int32_t value, size_t *out_len);
bool try_float_to_str(char *buf, size_t buf_size, float value, size_t *out_len);
bool try_copy_str(const char *src, char *dst, size_t dst_size, size_t *out_len);

#endif /* INC_FORMAT_H_ */
