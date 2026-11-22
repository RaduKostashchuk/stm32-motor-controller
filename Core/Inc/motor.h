#ifndef INC_MOTOR_H
#define INC_MOTOR_H

#include <stdint.h>

void motor_init(void);
void motor_set_pwm(int16_t pwm);

#endif /* INC_MOTOR_H */
