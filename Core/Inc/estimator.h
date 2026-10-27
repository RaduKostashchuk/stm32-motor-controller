#ifndef INC_ESTIMATOR_H_
#define INC_ESTIMATOR_H_

#include <stdint.h>

void estimator_init(int32_t initialPosition);
void estimator_run(int32_t currentPosition);
float estimator_get_rpm(void);

#endif /* INC_ESTIMATOR_H_ */
