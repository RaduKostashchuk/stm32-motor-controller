#include "unity.h"
#include "unity_fixture.h"
#include "reader.h"
#include <stdint.h>

TEST_GROUP(Reader);

TEST_SETUP(Reader) {}

TEST_TEAR_DOWN(Reader)
{
	reader_reset();
}

TEST(Reader, test_normal_read)
{
	const char *input = "MOTOR 1 50\r\n";
	reader_status status = reader_read(input, 12);
	reader_raw_cmd read;
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL(READER_OK, status);
	TEST_ASSERT_EQUAL_STRING("MOTOR 1 50", read.cmd);
}

TEST(Reader, test_ring_overflow)
{
	for (int i = 0; i < 15; i++) // fill ring completely
		reader_read("STOP 1\n", 7);

	reader_status status;
	for (int i = 0; i < 5; i++)
	{
		status = reader_read("STOP 1\n", 7); // try add command to ring
		TEST_ASSERT_EQUAL(READER_ERR_RING_OVERFLOW, status);
	}


	TEST_ASSERT_EQUAL(5, reader_dropped_count()); // 5 reads dropped due to ring capacity is 15

	reader_raw_cmd cmd;
	for (int i = 0; i < 15; i++)
		TEST_ASSERT_TRUE(reader_try_get_command(&cmd)); // 15 success reads

	TEST_ASSERT_FALSE(reader_try_get_command(&cmd)); // 16th read fails due to ring is empty
}

TEST(Reader, test_long_command_discarded)
{
	reader_status status = reader_read("STOP 1 STOP 1 STOP 1\n", 21);
	TEST_ASSERT_EQUAL(READER_ERR_CMD_TOO_LONG, status);
	reader_raw_cmd cmd;
	TEST_ASSERT_FALSE(reader_try_get_command(&cmd));
}

TEST(Reader, test_multi_command)
{
	reader_status status = reader_read("STOP 1\nMOTOR 1 50\nSTATUS\n", 25);
	TEST_ASSERT_EQUAL(READER_OK, status);
	reader_raw_cmd read;
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STOP 1", read.cmd);
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("MOTOR 1 50", read.cmd);
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STATUS", read.cmd);
	TEST_ASSERT_FALSE(reader_try_get_command(&read));
}

TEST(Reader, test_char_by_char)
{
	reader_read("S", 1);
	reader_read("T", 1);
	reader_read("O", 1);
	reader_read("P", 1);
	reader_read("\n", 1);
	reader_raw_cmd read;
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STOP", read.cmd);
}

TEST(Reader, test_cr_lf)
{
	reader_read("STOP 1\r\n", 8);
	reader_raw_cmd read;
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STOP 1", read.cmd);
}

TEST(Reader, test_cr_only)
{
	reader_read("STOP 1\r", 7);
	reader_raw_cmd read;
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STOP 1", read.cmd);
}

TEST(Reader, test_empty_buffer)
{
	reader_read(NULL, 0);
	reader_raw_cmd read;
	TEST_ASSERT_FALSE(reader_try_get_command(&read));
}

TEST(Reader, test_no_terminator)
{
	reader_read("STOP 1", 6);
	reader_raw_cmd read;
	TEST_ASSERT_FALSE(reader_try_get_command(&read));
}

TEST(Reader, test_discard)
{
	reader_read("MOTOR 1 50                    ", 30);
	reader_raw_cmd read;
	TEST_ASSERT_FALSE(reader_try_get_command(&read));

	reader_read("  \rSTATUS\r", 10);
	TEST_ASSERT_TRUE(reader_try_get_command(&read));
	TEST_ASSERT_EQUAL_STRING("STATUS", read.cmd);
}

TEST(Reader, test_only_terminators)
{
	reader_read("\r\r\r\r\r\r\n\n\n\n", 10);
	reader_raw_cmd read;
	TEST_ASSERT_FALSE(reader_try_get_command(&read));
}

TEST_GROUP_RUNNER(Reader)
{
	RUN_TEST_CASE(Reader, test_normal_read);
	RUN_TEST_CASE(Reader, test_ring_overflow);
	RUN_TEST_CASE(Reader, test_long_command_discarded);
	RUN_TEST_CASE(Reader, test_multi_command);
	RUN_TEST_CASE(Reader, test_char_by_char);
	RUN_TEST_CASE(Reader, test_cr_lf);
	RUN_TEST_CASE(Reader, test_cr_only);
	RUN_TEST_CASE(Reader, test_empty_buffer);
	RUN_TEST_CASE(Reader, test_no_terminator);
	RUN_TEST_CASE(Reader, test_discard);
	RUN_TEST_CASE(Reader, test_only_terminators);
}
