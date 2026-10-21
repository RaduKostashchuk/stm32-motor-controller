#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include <stdint.h>

#define TICKS_PER_REV 1980

int32_t encoder_get_position(void);
void encoder_init(void);

#endif /* INC_ENCODER_H_ */
