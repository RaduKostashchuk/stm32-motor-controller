#include "unity.h"
#include "unity_fixture.h"
#include "parser.h"
#include <stdint.h>

TEST_GROUP(Parser);

TEST_SETUP(Parser) {}

TEST_TEAR_DOWN(Parser) {}

// tokenize

TEST(Parser, test_empty_string)
{
	char *input = "";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

TEST(Parser, test_only_spaces)
{
	char *input =  "     ";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

TEST(Parser, test_leading_spaces)
{
	char *input = "  STOP";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

TEST(Parser, test_trailing_spaces)
{
	char *input = "STOP    ";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

TEST(Parser, test_double_spaces)
{
	char *input = "MODE   AUTO";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == AppMode);
}

TEST(Parser, test_single_token)
{
	char *input = "STATUS";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Status);
}

// try_get_int32

TEST(Parser, test_motor_positive)
{
	char *input = "MOTOR 1 50";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.target == 50);
}

TEST(Parser, test_motor_negative)
{
	char *input = "MOTOR 1 -50";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.target == -50);
}

TEST(Parser, test_motor_zero)
{
	char *input = "MOTOR 1 0";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.target == 0);
}

TEST(Parser, test_motor_trailing_garbage)
{
	char *input = "MOTOR 1 1a";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

TEST(Parser, test_motor_leading_garbage)
{
	char *input = "MOTOR 1 a1";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

TEST(Parser, test_motor_num_garbage)
{
	char *input = "MOTOR x 50";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

// parseMotorCmd

TEST(Parser, test_motor_valid)
{
	char *input = "MOTOR 1 50";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Motor && parsed.motorNumber == 1 && parsed.target == 50);
}

TEST(Parser, test_motor_mo_args)
{
	char *input = "MOTOR";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

TEST(Parser, test_motor_too_many_args)
{
	char *input = "MOTOR 1 1 1";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

TEST(Parser, test_motor_any_number)
{
	char *input = "MOTOR 99 99";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Motor && parsed.motorNumber == 99 && parsed.target == 99);
}

// parseStopCmd

TEST(Parser, test_stop_no_args)
{
	char *input = "STOP";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

TEST(Parser, test_stop_with_number)
{
	char *input = "STOP 111";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Stop && parsed.motorNumber == 111);
}

TEST(Parser, test_stop_invalid_number)
{
	char *input = "STOP xxx";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStop);
}

TEST(Parser, test_stop_too_many_args)
{
	char *input = "STOP 1 1";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStop);
}

// parseModeCmd

TEST(Parser, test_mode_auto)
{
	char *input = "MODE AUTO";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == AppMode && parsed.modeType == Auto);
}

TEST(Parser, test_mode_manual)
{
	char *input = "MODE MANUAL";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == AppMode && parsed.modeType == Manual);
}

TEST(Parser, test_mode_no_args)
{
	char *input = "MODE";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

TEST(Parser, test_mode_invalid_arg)
{
	char *input = "MODE QWE";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

TEST(Parser, test_mode_extra_arg)
{
	char *input = "MODE QW ER";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

// General
TEST(Parser, test_unknown_command)
{
	char *input = "TEST";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

TEST(Parser, test_status_valid)
{
	char *input = "STATUS";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Status);
}

TEST(Parser, test_status_extra_arg)
{
	char *input = "STATUS 1";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStatus);
}

TEST(Parser, test_low_case)
{
	char *input = "stop 1";
	Command parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

TEST(Parser, test_motor_max_target) {
	char *input = "MOTOR 1 250";
    Command parsed = parse(input);
    TEST_ASSERT_EQUAL(Motor, parsed.type);
    TEST_ASSERT_EQUAL(1, parsed.motorNumber);
    TEST_ASSERT_EQUAL(250, parsed.target);
}

TEST(Parser, test_motor_max_negative) {
	char *input = "MOTOR 1 -250";
    Command parsed = parse(input);
    TEST_ASSERT_EQUAL(Motor, parsed.type);
    TEST_ASSERT_EQUAL(-250, parsed.target);
}

TEST_GROUP_RUNNER(Parser)
{
	// tokenize
	RUN_TEST_CASE(Parser, test_empty_string);
	RUN_TEST_CASE(Parser, test_only_spaces);
	RUN_TEST_CASE(Parser, test_leading_spaces);
	RUN_TEST_CASE(Parser, test_trailing_spaces);
	RUN_TEST_CASE(Parser, test_double_spaces);
	RUN_TEST_CASE(Parser, test_single_token);

	// tryGetLong
	RUN_TEST_CASE(Parser, test_motor_positive);
	RUN_TEST_CASE(Parser, test_motor_negative);
	RUN_TEST_CASE(Parser, test_motor_zero);
	RUN_TEST_CASE(Parser, test_motor_trailing_garbage);
	RUN_TEST_CASE(Parser, test_motor_leading_garbage);
	RUN_TEST_CASE(Parser, test_motor_num_garbage);

	// parseMotorCmd
	RUN_TEST_CASE(Parser, test_motor_valid);
	RUN_TEST_CASE(Parser, test_motor_mo_args);
	RUN_TEST_CASE(Parser, test_motor_too_many_args);
	RUN_TEST_CASE(Parser, test_motor_any_number);

	// parseStopCmd
	RUN_TEST_CASE(Parser, test_stop_no_args);
	RUN_TEST_CASE(Parser, test_stop_with_number);
	RUN_TEST_CASE(Parser, test_stop_invalid_number);
	RUN_TEST_CASE(Parser, test_stop_too_many_args);

	// parseModeCmd
	RUN_TEST_CASE(Parser, test_mode_auto);
	RUN_TEST_CASE(Parser, test_mode_manual);
	RUN_TEST_CASE(Parser, test_mode_no_args);
	RUN_TEST_CASE(Parser, test_mode_invalid_arg);
	RUN_TEST_CASE(Parser, test_mode_extra_arg);

	// General
	RUN_TEST_CASE(Parser, test_unknown_command);
	RUN_TEST_CASE(Parser, test_status_valid);
	RUN_TEST_CASE(Parser, test_status_extra_arg);
	RUN_TEST_CASE(Parser, test_low_case);
	RUN_TEST_CASE(Parser, test_motor_max_target);
	RUN_TEST_CASE(Parser, test_motor_max_negative);
}

void test_parser(void)
{
    //test parser.c
	// tokenize
	RUN_TEST_CASE(Parser, test_empty_string);
	RUN_TEST_CASE(Parser, test_only_spaces);
	RUN_TEST_CASE(Parser, test_leading_spaces);
	RUN_TEST_CASE(Parser, test_trailing_spaces);
	RUN_TEST_CASE(Parser, test_double_spaces);
	RUN_TEST_CASE(Parser, test_single_token);

	// tryGetLong
	RUN_TEST_CASE(Parser, test_motor_positive);
	RUN_TEST_CASE(Parser, test_motor_negative);
	RUN_TEST_CASE(Parser, test_motor_zero);
	RUN_TEST_CASE(Parser, test_motor_trailing_garbage);
	RUN_TEST_CASE(Parser, test_motor_leading_garbage);
	RUN_TEST_CASE(Parser, test_motor_num_garbage);

	// parseMotorCmd
	RUN_TEST_CASE(Parser, test_motor_valid);
	RUN_TEST_CASE(Parser, test_motor_mo_args);
	RUN_TEST_CASE(Parser, test_motor_too_many_args);
	RUN_TEST_CASE(Parser, test_motor_any_number);

	// parseStopCmd
	RUN_TEST_CASE(Parser, test_stop_no_args);
	RUN_TEST_CASE(Parser, test_stop_with_number);
	RUN_TEST_CASE(Parser, test_stop_invalid_number);
	RUN_TEST_CASE(Parser, test_stop_too_many_args);

	// parseModeCmd
	RUN_TEST_CASE(Parser, test_mode_auto);
	RUN_TEST_CASE(Parser, test_mode_manual);
	RUN_TEST_CASE(Parser, test_mode_no_args);
	RUN_TEST_CASE(Parser, test_mode_invalid_arg);
	RUN_TEST_CASE(Parser, test_mode_extra_arg);

	// General
	RUN_TEST_CASE(Parser, test_unknown_command);
	RUN_TEST_CASE(Parser, test_status_valid);
	RUN_TEST_CASE(Parser, test_status_extra_arg);
	RUN_TEST_CASE(Parser, test_low_case);
	RUN_TEST_CASE(Parser, test_motor_max_target);
	RUN_TEST_CASE(Parser, test_motor_max_negative);
}
