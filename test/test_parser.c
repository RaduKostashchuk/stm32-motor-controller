#include "unity.h"
#include "parser.h"
#include <stdint.h>

// tokenize

void test_empty_string(void)
{
	RawCmd input = {.cmd = ""};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

void test_only_spaces(void)
{
	RawCmd input = {.cmd = "     "};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

void test_leading_spaces(void)
{
	RawCmd input = {.cmd = "  STOP"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

void test_trailing_spaces(void)
{
	RawCmd input = {.cmd = "STOP    "};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

void test_double_spaces(void)
{
	RawCmd input = {.cmd = "MODE   AUTO"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == AppMode);
}

void test_single_token(void)
{
	RawCmd input = {.cmd = "STATUS"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Status);
}

// try_get_int32

void test_motor_positive(void)
{
	RawCmd input = {.cmd = "MOTOR 1 50"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.target == 50);
}

void test_motor_negative(void)
{
	RawCmd input = {.cmd = "MOTOR 1 -50"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.target == -50);
}

void test_motor_zero(void)
{
	RawCmd input = {.cmd = "MOTOR 1 0"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.target == 0);
}

void test_motor_trailing_garbage(void)
{
	RawCmd input = {.cmd = "MOTOR 1 1a"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

void test_motor_leading_garbage(void)
{
	RawCmd input = {.cmd = "MOTOR 1 a1"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

void test_motor_num_garbage(void)
{
	RawCmd input = {.cmd = "MOTOR x 50"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

// parseMotorCmd

void test_motor_valid(void)
{
	RawCmd input = {.cmd = "MOTOR 1 50"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Motor && parsed.motorNumber == 1 && parsed.target == 50);
}

void test_motor_mo_args(void)
{
	RawCmd input = {.cmd = "MOTOR"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

void test_motor_too_many_args(void)
{
	RawCmd input = {.cmd = "MOTOR 1 1 1"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMotor);
}

void test_motor_any_number(void)
{
	RawCmd input = {.cmd = "MOTOR 99 99"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Motor && parsed.motorNumber == 99 && parsed.target == 99);
}

// parseStopCmd

void test_stop_no_args(void)
{
	RawCmd input = {.cmd = "STOP"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Stop);
}

void test_stop_with_number(void)
{
	RawCmd input = {.cmd = "STOP 111"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Stop && parsed.motorNumber == 111);
}

void test_stop_invalid_number(void)
{
	RawCmd input = {.cmd = "STOP xxx"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStop);
}

void test_stop_too_many_args(void)
{
	RawCmd input = {.cmd = "STOP 1 1"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStop);
}

// parseModeCmd

void test_mode_auto(void)
{
	RawCmd input = {.cmd = "MODE AUTO"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == AppMode && parsed.modeType == Auto);
}

void test_mode_manual(void)
{
	RawCmd input = {.cmd = "MODE MANUAL"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == AppMode && parsed.modeType == Manual);
}

void test_mode_no_args(void)
{
	RawCmd input = {.cmd = "MODE"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

void test_mode_invalid_arg(void)
{
	RawCmd input = {.cmd = "MODE QWE"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

void test_mode_extra_arg(void)
{
	RawCmd input = {.cmd = "MODE QW ER"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidMode);
}

// General
void test_unknown_command(void)
{
	RawCmd input = {.cmd = "TEST"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

void test_status_valid(void)
{
	RawCmd input = {.cmd = "STATUS"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Status);
}

void test_status_extra_arg(void)
{
	RawCmd input = {.cmd = "STATUS 1"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == InvalidStatus);
}

void test_low_case(void)
{
	RawCmd input = {.cmd = "stop 1"};
	Command parsed = parse(&input);
	TEST_ASSERT_TRUE(parsed.type == Unknown);
}

void test_motor_max_target(void) {
    RawCmd input = { .cmd = "MOTOR 1 250" };
    Command parsed = parse(&input);
    TEST_ASSERT_EQUAL(Motor, parsed.type);
    TEST_ASSERT_EQUAL(1, parsed.motorNumber);
    TEST_ASSERT_EQUAL(250, parsed.target);
}

void test_motor_max_negative(void) {
    RawCmd input = { .cmd = "MOTOR 1 -250" };
    Command parsed = parse(&input);
    TEST_ASSERT_EQUAL(Motor, parsed.type);
    TEST_ASSERT_EQUAL(-250, parsed.target);
}

void test_parser(void)
{
    //test parser.c
	// tokenize
	RUN_TEST(test_empty_string);
	RUN_TEST(test_only_spaces);
	RUN_TEST(test_leading_spaces);
	RUN_TEST(test_trailing_spaces);
	RUN_TEST(test_double_spaces);
	RUN_TEST(test_single_token);

	// tryGetLong
	RUN_TEST(test_motor_positive);
	RUN_TEST(test_motor_negative);
	RUN_TEST(test_motor_zero);
	RUN_TEST(test_motor_trailing_garbage);
	RUN_TEST(test_motor_leading_garbage);
	RUN_TEST(test_motor_num_garbage);

	// parseMotorCmd
	RUN_TEST(test_motor_valid);
	RUN_TEST(test_motor_mo_args);
	RUN_TEST(test_motor_too_many_args);
	RUN_TEST(test_motor_any_number);

	// parseStopCmd
	RUN_TEST(test_stop_no_args);
	RUN_TEST(test_stop_with_number);
	RUN_TEST(test_stop_invalid_number);
	RUN_TEST(test_stop_too_many_args);

	// parseModeCmd
//	RUN_TEST(test_mode_auto);
	RUN_TEST(test_mode_manual);
	RUN_TEST(test_mode_no_args);
	RUN_TEST(test_mode_invalid_arg);
	RUN_TEST(test_mode_extra_arg);

	// General
	RUN_TEST(test_unknown_command);
	RUN_TEST(test_status_valid);
	RUN_TEST(test_status_extra_arg);
	RUN_TEST(test_low_case);
	RUN_TEST(test_motor_max_target);
	RUN_TEST(test_motor_max_negative);
}
