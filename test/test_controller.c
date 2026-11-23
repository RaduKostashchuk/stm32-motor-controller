#include "unity.h"
#include "unity_fixture.h"
#include "vel_controller.h"
#include <stdint.h>

TEST_GROUP(Controller);

TEST_SETUP(Controller)
{
	vel_controller_init();
}

TEST_TEAR_DOWN(Controller) {}

TEST(Controller, test_target_zero)
{
	vel_controller_set_target_rpm(0.0f);
	TEST_ASSERT_EQUAL_INT16(0, vel_controller_get_control_value());
}

TEST(Controller, test_target_zero_reset_values)
{
	vel_controller_set_target_rpm(20.0f);
	vel_controller_set_current_rpm(5.0f);
	vel_controller_get_control_value();

	drive_state state;
	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_NOT_EQUAL_FLOAT(state.cont_vel_i, 0.0f);

	vel_controller_set_target_rpm(0.0f);
	vel_controller_get_control_value();

	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_FLOAT(state.cont_vel_i, 0.0f);
}

TEST(Controller, test_proportional_positive)
{
	vel_controller_set_target_rpm(10.0f);
	vel_controller_set_current_rpm(0.0f);
	float error = 10.0f;
	float expected = VEL_KP * error;
	vel_controller_get_control_value();
	drive_state state;
	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_FLOAT(expected, state.cont_vel_p);
}

TEST(Controller, test_proportional_negative)
{
	vel_controller_set_target_rpm(-10.0f);
	vel_controller_set_current_rpm(0.0f);
	float error = -10.0f;
	float expected = VEL_KP * error;
	vel_controller_get_control_value();
	drive_state state;
	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_FLOAT(expected, state.cont_vel_p);
}

TEST(Controller, test_saturation_positive)
{
	vel_controller_set_target_rpm(95.0f);
	TEST_ASSERT_EQUAL_INT16(255, vel_controller_get_control_value());
}

TEST(Controller, test_saturation_negative)
{
	vel_controller_set_target_rpm(-95.0f);
	TEST_ASSERT_EQUAL_INT16(-255, vel_controller_get_control_value());
}

TEST(Controller, test_i_anti_windup_after_accumulation)
{
	vel_controller_set_target_rpm(20.0f);
	vel_controller_set_current_rpm(5.0f);
	vel_controller_get_control_value();

	for (int i = 0; i < 10; i++)
	{
		vel_controller_set_current_rpm(5.0f + i);
		vel_controller_get_control_value();
	}

	drive_state state;
	vel_controller_fill_drive_state(&state);
	float i_accumulated = state.cont_vel_i;
	TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, i_accumulated);

	vel_controller_set_target_rpm(95.0f); //saturation
	vel_controller_set_current_rpm(0);
	for (int i = 0; i < 10; i++)
	{
		vel_controller_get_control_value();
		vel_controller_fill_drive_state(&state);
		TEST_ASSERT_EQUAL_FLOAT(i_accumulated, state.cont_vel_i);
	}
}

TEST(Controller, test_i_deadband)
{
	vel_controller_set_target_rpm(80.0f);
	vel_controller_set_current_rpm(79.0f);
	vel_controller_get_control_value();

	drive_state state;
	vel_controller_fill_drive_state(&state);
	float i_prev = state.cont_vel_i;

	for (int i = 0; i < 10; i++)
	{
		vel_controller_set_current_rpm(80.0f + 0.01f * i);
		vel_controller_get_control_value();
		vel_controller_fill_drive_state(&state);
		float i_curr = state.cont_vel_i;
		TEST_ASSERT_EQUAL_FLOAT(i_prev, i_curr);
		i_prev = i_curr;
	}
}

TEST(Controller, test_steady_state_with_accumulated_i)
{
	vel_controller_set_target_rpm(80.0f);
	vel_controller_set_current_rpm(50.0f);

	for (int i = 0; i < 10; i++)
		vel_controller_get_control_value();

	drive_state state;
	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, state.cont_vel_i);

	vel_controller_set_current_rpm(80.0f);
	int16_t control_value = vel_controller_get_control_value();
	vel_controller_fill_drive_state(&state);
	TEST_ASSERT_FLOAT_WITHIN(0.5f, (int)(state.cont_vel_uff + state.cont_vel_i), control_value);
}

TEST(Controller, test_i_normal_accumulation)
{
	vel_controller_set_target_rpm(80.0f);
	vel_controller_set_current_rpm(70.0f);

	float i_expected = 0.0f;
	for (int8_t i = 0; i < 10; i++)
		i_expected += VEL_KI * 10.0f * DELTA_TIME_SEC;

	for (int8_t i = 0; i < 10; i++)
		vel_controller_get_control_value();

	drive_state state;
	vel_controller_fill_drive_state(&state);

	TEST_ASSERT_EQUAL_FLOAT(i_expected, state.cont_vel_i);
}

TEST(Controller, test_i_normal_negative_accumulation)
{
	vel_controller_set_target_rpm(-80.0f);
	vel_controller_set_current_rpm(-70.0f);

	float i_expected = 0.0f;
	for (int8_t i = 0; i < 10; i++)
		i_expected += VEL_KI * -10.0f * DELTA_TIME_SEC;

	for (int8_t i = 0; i < 10; i++)
		vel_controller_get_control_value();

	drive_state state;
	vel_controller_fill_drive_state(&state);

	TEST_ASSERT_EQUAL_FLOAT(i_expected, state.cont_vel_i);
}

TEST_GROUP_RUNNER(Controller)
{
	RUN_TEST_CASE(Controller, test_target_zero);
	RUN_TEST_CASE(Controller, test_target_zero_reset_values);
	RUN_TEST_CASE(Controller, test_proportional_positive);
	RUN_TEST_CASE(Controller, test_proportional_negative);
	RUN_TEST_CASE(Controller, test_saturation_positive);
	RUN_TEST_CASE(Controller, test_saturation_negative);
	RUN_TEST_CASE(Controller, test_i_anti_windup_after_accumulation);
	RUN_TEST_CASE(Controller, test_i_deadband);
	RUN_TEST_CASE(Controller, test_steady_state_with_accumulated_i);
	RUN_TEST_CASE(Controller, test_i_normal_accumulation);
	RUN_TEST_CASE(Controller, test_i_normal_negative_accumulation);
}
