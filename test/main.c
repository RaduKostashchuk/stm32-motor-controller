#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

extern void test_parser(void);
extern void test_format(void);

int main(void) {
	UNITY_BEGIN();

	test_parser();
	test_format();

	return UNITY_END();
}
