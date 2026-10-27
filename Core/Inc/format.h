#ifndef INC_FORMAT_H_
#define INC_FORMAT_H_

#include <stdint.h>
#include <stdbool.h>

bool try_uint32_to_dec(char *buf, size_t buf_size, uint32_t value, size_t *out_len);
size_t int32_to_char_dec(char *buf, size_t buf_size, int32_t value);
size_t float_to_char_dec(char *buf, size_t buf_size, float value);
bool try_copy_str(const char *src, char *dst, size_t dst_size, size_t *out_len);

#endif /* INC_FORMAT_H_ */
