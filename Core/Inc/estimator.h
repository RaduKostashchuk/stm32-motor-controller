#ifndef INC_ESTIMATOR_H
#define INC_ESTIMATOR_H

#include <stdint.h>

#define TICKS_PER_REV 990
#define CPU_FREQ 170000000
#define PSC 99 // 16-bit counter doesnt overflow in rpm range 5 - 95
#define RPMT_T_COEF (float)CPU_FREQ / (float)((PSC + 1) * TICKS_PER_REV) // 1717
#define UPPER_CYCLES_BOUND 60000 // ~2 Rpm
#define AVG_WINDOW 22

void estimator_init(int32_t initialPosition);
void estimator_run_m(int32_t currentPosition);
float estimator_get_rpm_m(void);
float estimator_get_rpm_t(void);
void estimator_update_t(uint32_t cpu_cycles, int32_t cur_pos, uint32_t now);
void estimator_try_reset_t_method(uint32_t now, int32_t cur_pos);

#endif /* INC_ESTIMATOR_H */
