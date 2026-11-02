#include "unity.h"
#include "unity_fixture.h"
#include "controller.h"
#include <stdint.h>

TEST_GROUP(Controller);

TEST_SETUP(Controller)
{
	controller_init();
}

TEST_TEAR_DOWN(Controller) {}

TEST(Controller, test_target_zero)
{
	controller_set_target_rpm(0.0f);
	TEST_ASSERT_EQUAL_INT16(0, controller_get_control_value());
}

TEST(Controller, test_target_zero_reset_values)
{
	controller_set_target_rpm(20.0f);
	controller_set_current_rpm(5.0f);
	controller_get_control_value();

	DriveState state;
	controller_fill_drive_state(&state);
	TEST_ASSERT_NOT_EQUAL_FLOAT(state.cont_i, 0.0f);

	controller_set_target_rpm(0.0f);
	controller_get_control_value();

	controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_FLOAT(state.cont_i, 0.0f);
}

TEST(Controller, test_proportional_positive)
{
	controller_set_target_rpm(10.0f);
	controller_set_current_rpm(0.0f);
	controller_get_control_value();
	DriveState state;
	controller_fill_drive_state(&state);
	TEST_ASSERT_TRUE(state.cont_p > 0);
}

TEST(Controller, test_proportional_negative)
{
	controller_set_target_rpm(-10.0f);
	controller_set_current_rpm(0.0f);
	controller_get_control_value();
	DriveState state;
	controller_fill_drive_state(&state);
	TEST_ASSERT_TRUE(state.cont_p < 0);
}

TEST(Controller, test_saturation_positive)
{
	controller_set_target_rpm(95.0f);
	TEST_ASSERT_EQUAL_INT16(255, controller_get_control_value());
}

TEST(Controller, test_saturation_negative)
{
	controller_set_target_rpm(-95.0f);
	TEST_ASSERT_EQUAL_INT16(-255, controller_get_control_value());
}

TEST(Controller, test_i_anti_windup_after_accumulation)
{
	controller_set_target_rpm(20.0f);
	controller_set_current_rpm(5.0f);
	controller_get_control_value();

	for (int i = 0; i < 10; i++)
	{
		controller_set_current_rpm(5.0f + i);
		controller_get_control_value();
	}

	DriveState state;
	controller_fill_drive_state(&state);
	float i_accumulated = state.cont_i;
	TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, i_accumulated);

	controller_set_target_rpm(95.0f); //saturation
	controller_set_current_rpm(0);
	for (int i = 0; i < 10; i++)
	{
		controller_get_control_value();
		controller_fill_drive_state(&state);
		TEST_ASSERT_EQUAL_FLOAT(i_accumulated, state.cont_i);
	}
}

TEST(Controller, test_i_deadband)
{
	controller_set_target_rpm(80.0f);
	controller_set_current_rpm(79.0f);
	controller_get_control_value();

	DriveState state;
	controller_fill_drive_state(&state);
	float i_prev = state.cont_i;

	for (int i = 0; i < 10; i++)
	{
		controller_set_current_rpm(79.0f + 0.1f * i);
		controller_get_control_value();
		controller_fill_drive_state(&state);
		float i_curr = state.cont_i;
		TEST_ASSERT_EQUAL_FLOAT(i_prev, i_curr);
		i_prev = i_curr;
	}
}

TEST(Controller, test_steady_state_with_accumulated_i)
{
	controller_set_target_rpm(80.0f);
	controller_set_current_rpm(50.0f);

	for (int i = 0; i < 10; i++)
		controller_get_control_value();

	DriveState state;
	controller_fill_drive_state(&state);
	TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, state.cont_i);

	controller_set_current_rpm(80.0f);
	int16_t control_value = controller_get_control_value();
	controller_fill_drive_state(&state);
	TEST_ASSERT_FLOAT_WITHIN(0.5f, (int)(state.cont_uff + state.cont_i), control_value);
}

TEST(Controller, test_i_normal_accumulation)
{
	controller_set_target_rpm(80.0f);
	controller_set_current_rpm(50.0f);
	controller_get_control_value();
	DriveState state;
	controller_fill_drive_state(&state);
	float i_prev = state.cont_i;

	for (int i = 1; i < 11; i++)
	{
		controller_set_current_rpm(50.0f + i);
		controller_get_control_value();
		controller_fill_drive_state(&state);
		float i_curr = state.cont_i;
		TEST_ASSERT_GREATER_THAN_FLOAT(i_prev, i_curr);
		i_prev = i_curr;
	}
}

TEST(Controller, test_i_normal_negative_accumulation)
{
	controller_set_target_rpm(-80.0f);
	controller_set_current_rpm(-50.0f);
	controller_get_control_value();
	DriveState state;
	controller_fill_drive_state(&state);
	float i_prev = state.cont_i;
	TEST_ASSERT_LESS_THAN_FLOAT(0.0f, i_prev);

	for (int i = 1; i < 11; i++)
	{
		controller_set_current_rpm(-50.0f - i);
		controller_get_control_value();
		controller_fill_drive_state(&state);
		float i_curr = state.cont_i;
		TEST_ASSERT_LESS_THAN_FLOAT(i_prev, i_curr);
		i_prev = i_curr;
	}
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
