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

// M-method

TEST(Estimator, test_zero_delta)
{
	estimator_run_m(0);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_glitch_positive)
{
	estimator_run_m(0);
	TEST_ASSERT_EQUAL(0.0f, estimator_get_rpm_m());
	estimator_run_m(1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_glitch_negative)
{
	estimator_run_m(0);
	TEST_ASSERT_EQUAL(0.0f, estimator_get_rpm_m());
	estimator_run_m(-1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_motor_stopped)
{
	estimator_run_m(10);
	estimator_run_m(10);
	estimator_run_m(10);
	estimator_run_m(10);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_recovery_after_glitch)
{
	estimator_run_m(1000);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
	estimator_run_m(1010);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, 60.6f, estimator_get_rpm_m());
}

TEST(Estimator, test_60_rpm)
{
	estimator_run_m(10);
	TEST_ASSERT_FLOAT_WITHIN(0.7f, 60.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_minus_60_rpm)
{
	estimator_run_m(-10);
	TEST_ASSERT_FLOAT_WITHIN(0.7f, -60.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_init)
{
	estimator_run_m(99);
	estimator_init(0);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

TEST(Estimator, test_boundary_ok)
{
	estimator_run_m(46);
	TEST_ASSERT_FLOAT_WITHIN(0.1f, 278.78f, estimator_get_rpm_m());
}

TEST(Estimator, test_boundary_exceeded)
{
	estimator_run_m(201);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_m());
}

// T-method
TEST(Estimator, test_less_then_window_size_updates)
{
	estimator_update_t(10000, 10, 10);
	estimator_update_t(10000, 20, 20);
	estimator_update_t(10000, 30, 30);
	float cycles_per_tick = 10000.0f;
	float expected_speed = RPMT_T_COEF * 60 / cycles_per_tick;
	TEST_ASSERT_FLOAT_WITHIN(0.1f, expected_speed, estimator_get_rpm_t());
}

TEST(Estimator, test_exact_window_size_updates)
{
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	float cycles_per_tick = 10000.0f;
	float expected_speed = RPMT_T_COEF * 60 / cycles_per_tick;
	TEST_ASSERT_FLOAT_WITHIN(0.1f, expected_speed, estimator_get_rpm_t());
}

TEST(Estimator, test_greater_then_window_size_updates)
{
	for (uint8_t i = 0; i < AVG_WINDOW + 20; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	float cycles_per_tick = 10000.0f;
	float expected_speed = RPMT_T_COEF * 60 / cycles_per_tick;
	TEST_ASSERT_FLOAT_WITHIN(0.1f, expected_speed, estimator_get_rpm_t());
}

TEST(Estimator, test_ignore_too_high_values)
{
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	estimator_update_t(UPPER_CYCLES_BOUND + 1, 1000, 10);
	float cycles_per_tick = 10000.0f;
	float expected_speed = RPMT_T_COEF * 60 / cycles_per_tick;
	TEST_ASSERT_FLOAT_WITHIN(0.1f, expected_speed, estimator_get_rpm_t());
}

TEST_GROUP_RUNNER(Estimator)
{
	// M-method
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
	// T-method
	RUN_TEST_CASE(Estimator, test_less_then_window_size_updates);
	RUN_TEST_CASE(Estimator, test_exact_window_size_updates);
	RUN_TEST_CASE(Estimator, test_greater_then_window_size_updates);
	RUN_TEST_CASE(Estimator, test_ignore_too_high_values);
}
