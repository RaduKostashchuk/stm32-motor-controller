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
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_UNKNOWN);
}

TEST(Parser, test_only_spaces)
{
	char *input =  "     ";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_UNKNOWN);
}

TEST(Parser, test_leading_spaces)
{
	char *input = "  STOP";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STOP);
}

TEST(Parser, test_trailing_spaces)
{
	char *input = "STOP    ";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STOP);
}

TEST(Parser, test_double_spaces)
{
	char *input = "PWM  1 10";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_PWM);
}

TEST(Parser, test_single_token)
{
	char *input = "STATUS";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STATUS);
}

// try_get_int32

TEST(Parser, test_pwm_positive)
{
	char *input = "PWM 1 50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.pwm == 50);
}

TEST(Parser, test_pwm_negative)
{
	char *input = "PWM 1 -50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.pwm == -50);
}

TEST(Parser, test_pwm_zero)
{
	char *input = "PWM 1 0";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.pwm == 0);
}

TEST(Parser, test_pwm_trailing_garbage)
{
	char *input = "PWM 1 1a";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_PWM);
}

TEST(Parser, test_pwm_leading_garbage)
{
	char *input = "PWM 1 a1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_PWM);
}

TEST(Parser, test_pwm_num_garbage)
{
	char *input = "PWM x 50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_PWM);
}

// parse_pwm_cmd

TEST(Parser, test_pwm_valid)
{
	char *input = "PWM 1 50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_PWM && parsed.motor_number == 1 && parsed.pwm == 50);
}

TEST(Parser, test_pwm_no_args)
{
	char *input = "PWM";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_PWM);
}

TEST(Parser, test_pwm_too_many_args)
{
	char *input = "PWM 1 1 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_PWM);
}

TEST(Parser, test_pwm_any_number)
{
	char *input = "PWM 99 99";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_PWM && parsed.motor_number == 99 && parsed.pwm == 99);
}

// parse_vel_cmd

TEST(Parser, test_vel_valid)
{
	char *input = "VEL 1 50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_VEL && parsed.motor_number == 1 && parsed.velocity == 50);
}

TEST(Parser, test_vel_no_args)
{
	char *input = "VEL";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_VEL);
}

TEST(Parser, test_vel_too_many_args)
{
	char *input = "VEL 1 1 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_VEL);
}

TEST(Parser, test_vel_any_number)
{
	char *input = "VEL 99 99";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_VEL && parsed.motor_number == 99 && parsed.velocity == 99);
}

// parse_pos_cmd

TEST(Parser, test_pos_valid)
{
	char *input = "POS 1 50";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_POS && parsed.motor_number == 1 && parsed.position == 50);
}

TEST(Parser, test_pos_no_args)
{
	char *input = "POS";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_POS);
}

TEST(Parser, test_pos_too_many_args)
{
	char *input = "POS 1 1 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_POS);
}

TEST(Parser, test_pos_any_number)
{
	char *input = "POS 99 99";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_POS && parsed.motor_number == 99 && parsed.position == 99);
}

// parse_stop_cmd

TEST(Parser, test_stop_no_args)
{
	char *input = "STOP";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STOP);
}

TEST(Parser, test_stop_with_number)
{
	char *input = "STOP 111";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STOP && parsed.motor_number == 111);
}

TEST(Parser, test_stop_invalid_number)
{
	char *input = "STOP xxx";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_STOP);
}

TEST(Parser, test_stop_too_many_args)
{
	char *input = "STOP 1 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_STOP);
}

// General
TEST(Parser, test_unknown_command)
{
	char *input = "TEST";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_UNKNOWN);
}

TEST(Parser, test_status_valid)
{
	char *input = "STATUS";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_STATUS);
}

TEST(Parser, test_status_extra_arg)
{
	char *input = "STATUS 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_INVALID_STATUS);
}

TEST(Parser, test_low_case)
{
	char *input = "stop 1";
	command_t parsed = parse(input);
	TEST_ASSERT_TRUE(parsed.type == COMMAND_UNKNOWN);
}

TEST(Parser, test_pwm_max_target)
{
	char *input = "PWM 1 255";
	command_t parsed = parse(input);
    TEST_ASSERT_EQUAL(COMMAND_PWM, parsed.type);
    TEST_ASSERT_EQUAL(1, parsed.motor_number);
    TEST_ASSERT_EQUAL(255, parsed.pwm);
}

TEST(Parser, test_pwm_max_negative)
{
	char *input = "PWM 1 -255";
	command_t parsed = parse(input);
    TEST_ASSERT_EQUAL(COMMAND_PWM, parsed.type);
    TEST_ASSERT_EQUAL(-255, parsed.pwm);
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

	// try_get_int32
	RUN_TEST_CASE(Parser, test_pwm_positive);
	RUN_TEST_CASE(Parser, test_pwm_negative);
	RUN_TEST_CASE(Parser, test_pwm_zero);
	RUN_TEST_CASE(Parser, test_pwm_trailing_garbage);
	RUN_TEST_CASE(Parser, test_pwm_leading_garbage);
	RUN_TEST_CASE(Parser, test_pwm_num_garbage);

	// parse_pwm_cmd
	RUN_TEST_CASE(Parser, test_pwm_valid);
	RUN_TEST_CASE(Parser, test_pwm_no_args);
	RUN_TEST_CASE(Parser, test_pwm_too_many_args);
	RUN_TEST_CASE(Parser, test_pwm_any_number);

	// parse_vel_cmd
	RUN_TEST_CASE(Parser, test_vel_valid);
	RUN_TEST_CASE(Parser, test_vel_no_args);
	RUN_TEST_CASE(Parser, test_vel_too_many_args);
	RUN_TEST_CASE(Parser, test_vel_any_number);

	// parse_pos_cmd
	RUN_TEST_CASE(Parser, test_pos_valid);
	RUN_TEST_CASE(Parser, test_pos_no_args);
	RUN_TEST_CASE(Parser, test_pos_too_many_args);
	RUN_TEST_CASE(Parser, test_pos_any_number);

	// parse_stop_cmd
	RUN_TEST_CASE(Parser, test_stop_no_args);
	RUN_TEST_CASE(Parser, test_stop_with_number);
	RUN_TEST_CASE(Parser, test_stop_invalid_number);
	RUN_TEST_CASE(Parser, test_stop_too_many_args);

	// General
	RUN_TEST_CASE(Parser, test_unknown_command);
	RUN_TEST_CASE(Parser, test_status_valid);
	RUN_TEST_CASE(Parser, test_status_extra_arg);
	RUN_TEST_CASE(Parser, test_low_case);
	RUN_TEST_CASE(Parser, test_pwm_max_target);
	RUN_TEST_CASE(Parser, test_pwm_max_negative);
}
