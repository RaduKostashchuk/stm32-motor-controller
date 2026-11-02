#include "unity.h"
#include "unity_fixture.h"
#include "estimator.h"
#include <stdint.h>

TEST_GROUP(Estimator);

TEST_SETUP(Estimator)
{
	estimator_init(0);
}

TEST_TEAR_DOWN(Estimator) {}

TEST(Estimator, test_zero_delta)
{
	estimator_run(0);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST(Estimator, test_glitch_positive)
{
	estimator_run(0);
	TEST_ASSERT_EQUAL(0.0f, estimator_get_rpm());
	estimator_run(1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST(Estimator, test_glitch_negative)
{
	estimator_run(0);
	TEST_ASSERT_EQUAL(0.0f, estimator_get_rpm());
	estimator_run(-1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST(Estimator, test_motor_stopped)
{
	estimator_run(10);
	estimator_run(10);
	estimator_run(10);
	estimator_run(10);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST(Estimator, test_recovery_after_glitch)
{
	estimator_run(1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
	estimator_run(1010);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, 6.06f, estimator_get_rpm());
}

TEST(Estimator, test_60_rpm)
{
	estimator_run(99);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, 60.0f, estimator_get_rpm());
}

TEST(Estimator, test_minus_60_rpm)
{
	estimator_run(-99);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, -60.0f, estimator_get_rpm());
}

TEST(Estimator, test_init)
{
	estimator_run(99);
	estimator_init(0);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST(Estimator, test_boundary_ok)
{
	estimator_run(199);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, 120.6f, estimator_get_rpm());
}

TEST(Estimator, test_boundary_exceeded)
{
	estimator_run(201);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm());
}

TEST_GROUP_RUNNER(Estimator)
{
	RUN_TEST_CASE(Estimator, test_zero_delta);
	RUN_TEST_CASE(Estimator, test_glitch_positive);
	RUN_TEST_CASE(Estimator, test_glitch_negative);
	RUN_TEST_CASE(Estimator, test_motor_stopped);
	RUN_TEST_CASE(Estimator, test_recovery_after_glitch);
	RUN_TEST_CASE(Estimator, test_60_rpm);
	RUN_TEST_CASE(Estimator, test_minus_60_rpm);
	RUN_TEST_CASE(Estimator, test_init);
	RUN_TEST_CASE(Estimator, test_boundary_ok);
	RUN_TEST_CASE(Estimator, test_boundary_exceeded);
}
