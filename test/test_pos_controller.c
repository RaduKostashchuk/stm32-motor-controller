#include "unity.h"
#include "unity_fixture.h"
#include "pos_controller.h"
#include <stdint.h>

TEST_GROUP(PosController);

TEST_SETUP(PosController)
{
	pos_controller_reset(0);
}

TEST_TEAR_DOWN(PosController) {}

TEST(PosController, test_reset_clears_target)
{
	pos_controller_set_target_pos(1000);
	pos_controller_calc_control_value(100);
	pos_controller_reset(0);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(0, state.target_rpm);
	TEST_ASSERT_EQUAL_INT32(0, state.cont_pos_error);
}

TEST(PosController, test_set_target_pos)
{
	pos_controller_set_target_pos(1000);
	pos_controller_calc_control_value(0);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(1000, state.cont_pos_error);
}

TEST(PosController, test_deadband_no_output)
{
	pos_controller_set_target_pos(1000);
	pos_controller_calc_control_value(1000 - POS_DEADBAND + 1);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(0, state.target_rpm);
}

TEST(PosController, test_proportional_positive)
{
	pos_controller_set_target_pos(1000);
	pos_controller_calc_control_value(900);
	float expected = POS_KP * 100;
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(expected, state.target_rpm);
}

TEST(PosController, test_proportional_negative)
{
	pos_controller_set_target_pos(1000);
	pos_controller_calc_control_value(1100);
	float expected = -POS_KP * 100;
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(expected, state.target_rpm);
}

TEST(PosController, test_clamp_positive)
{
	pos_controller_set_target_pos(100000);
	pos_controller_calc_control_value(0);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(POS_CONTROL_MAX_RPM, state.target_rpm);
}

TEST(PosController, test_clamp_negative)
{
	pos_controller_set_target_pos(0);
	pos_controller_calc_control_value(100000);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_EQUAL_INT32(POS_CONTROL_MIN_RPM, state.target_rpm);
}

TEST(PosController, test_direction_reverse)
{
	pos_controller_set_target_pos(100);
	pos_controller_calc_control_value(0);
	drive_state_t state;
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_TRUE(state.target_rpm > 0);

	pos_controller_set_target_pos(100);
	pos_controller_calc_control_value(110);
	pos_controller_fill_drive_state(&state);
	TEST_ASSERT_TRUE(state.target_rpm < 0);
}

TEST_GROUP_RUNNER(PosController)
{
	RUN_TEST_CASE(PosController, test_reset_clears_target);
	RUN_TEST_CASE(PosController, test_set_target_pos);
	RUN_TEST_CASE(PosController, test_deadband_no_output);
	RUN_TEST_CASE(PosController, test_proportional_positive);
	RUN_TEST_CASE(PosController, test_proportional_negative);
	RUN_TEST_CASE(PosController, test_clamp_positive);
	RUN_TEST_CASE(PosController, test_clamp_negative);
	RUN_TEST_CASE(PosController, test_direction_reverse);
}
