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
TEST(Estimator, test_normal_rpm_calculation)
{
	for (uint8_t i = 0; i < 10; i++)
		estimator_update_t(10000, 10, 10);

	float cycles_per_tick = 10000.0f;
    float expected = RPMT_T_COEF * 60.0f / cycles_per_tick;
	TEST_ASSERT_EQUAL_FLOAT(expected, estimator_get_rpm_t());
}

TEST(Estimator, test_when_upper_bound_no_rpm_update)
{
	estimator_update_t(10000, 10, 10);
	float initial_speed = estimator_get_rpm_t();
	estimator_update_t(UPPER_CYCLES_BOUND, 10, 10);
	TEST_ASSERT_EQUAL_FLOAT(initial_speed, estimator_get_rpm_t());
}

TEST(Estimator, test_avg_moves_correctly)
{
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	float cycles_per_tick = 10000.0f;
    float expected = RPMT_T_COEF * 60.0f / cycles_per_tick;
    TEST_ASSERT_EQUAL_FLOAT(expected, estimator_get_rpm_t());

	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(20000, 10 * i, 10 * i);

	cycles_per_tick = 20000.0f;
    expected = RPMT_T_COEF * 60.0f / cycles_per_tick;
    TEST_ASSERT_EQUAL_FLOAT(expected, estimator_get_rpm_t());
}

TEST(Estimator, test_zero_cpu_cycles)
{
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	float initial_speed = estimator_get_rpm_t();
	estimator_update_t(0, 1000, 10);
	TEST_ASSERT_EQUAL_FLOAT(initial_speed, estimator_get_rpm_t());
}

TEST(Estimator, test_init_clears_rpm)
{
	for (uint8_t i = 0; i < AVG_WINDOW; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	estimator_init(123);
	TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_t());
}

TEST(Estimator, test_direction_changes_sign)
{
	for (uint8_t i = 0; i < 10; i++)
		estimator_update_t(10000, 10 * i, 10 * i);

	float speed = estimator_get_rpm_t();
	TEST_ASSERT_TRUE(speed > 0.0f);

	for (uint8_t i = 0; i < 10; i++)
		estimator_update_t(10000, 10 - i, 10 * i);

	speed = estimator_get_rpm_t();
	TEST_ASSERT_TRUE(speed < 0.0f);
}

TEST(Estimator, test_avg_window_border_causes_no_spikes)
{
	float cycles_per_tick = 10000.0f;
    float expected = RPMT_T_COEF * 60.0f / cycles_per_tick;

	for (uint8_t i = 0; i < AVG_WINDOW + 3; i++)
	{
		estimator_update_t(10000, 10 * i, 10 * i);
		TEST_ASSERT_EQUAL_FLOAT(expected, estimator_get_rpm_t());
	}
}

TEST(Estimator, test_sign_debounce)
{
	const int32_t init_pos = 100;
	estimator_update_t(10000, init_pos, 10);

	float speed = estimator_get_rpm_t();
	TEST_ASSERT_TRUE(speed > 0.0f);

	uint8_t counter = 1;
	while(counter < SIGN_STABLE_THRESHOLD)
	{
		estimator_update_t(10000, init_pos - counter, 10);
		TEST_ASSERT_TRUE(estimator_get_rpm_t() > 0.0f);
		counter++;
	}

	estimator_update_t(10000, init_pos - counter, 10);
	TEST_ASSERT_TRUE(estimator_get_rpm_t() < 0.0f);
}

TEST(Estimator, test_timeout_resets_state)
{
	uint32_t now = 1000;
    estimator_update_t(10000, 100, now);
    estimator_try_reset_t_method(now + T_METHOD_TIMEOUT_MS, 110);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, estimator_get_rpm_t());
}

TEST(Estimator, test_less_then_timeout_desnt_reset_state)
{
	uint32_t now = 1000;
    estimator_update_t(10000, 100, now);
	float cycles_per_tick = 10000.0f;
    float expected = RPMT_T_COEF * 60.0f / cycles_per_tick;
    estimator_try_reset_t_method(now + T_METHOD_TIMEOUT_MS - 10, 110);
    TEST_ASSERT_EQUAL_FLOAT(expected, estimator_get_rpm_t());
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
	RUN_TEST_CASE(Estimator, test_normal_rpm_calculation);
	RUN_TEST_CASE(Estimator, test_when_upper_bound_no_rpm_update);
	RUN_TEST_CASE(Estimator, test_avg_moves_correctly);
	RUN_TEST_CASE(Estimator, test_zero_cpu_cycles);
	RUN_TEST_CASE(Estimator, test_init_clears_rpm);
	RUN_TEST_CASE(Estimator, test_direction_changes_sign);
	RUN_TEST_CASE(Estimator, test_avg_window_border_causes_no_spikes);
	RUN_TEST_CASE(Estimator, test_sign_debounce);
	RUN_TEST_CASE(Estimator, test_timeout_resets_state);
	RUN_TEST_CASE(Estimator, test_less_then_timeout_desnt_reset_state);
}
