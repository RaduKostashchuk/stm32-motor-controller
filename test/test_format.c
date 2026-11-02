#include "unity.h"
#include "unity_fixture.h"
#include "format.h"
#include <stdint.h>
#define DST_SIZE 16

TEST_GROUP(Formatter);

TEST_SETUP(Formatter) {}

TEST_TEAR_DOWN(Formatter) {}

// try_copy_str
TEST(Formatter, test_normal)
{
	size_t src_len = 4;
	size_t out_len = 0;
	char dst_buf[DST_SIZE];
	const char *src_buf = "1234";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
    TEST_ASSERT_EQUAL_UINT32((uint32_t)src_len, (uint32_t)out_len);
}

TEST(Formatter, test_dst_null)
{
	size_t out_len = 33;
	char src_buf[] = "1234";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, NULL, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
}

TEST(Formatter, test_src_null)
{
	size_t out_len = 33;
	char dst_buf[DST_SIZE] = "prev";
	TEST_ASSERT_FALSE(try_copy_str(NULL, dst_buf, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
    TEST_ASSERT_EQUAL_STRING("prev", dst_buf);
}

TEST(Formatter, test_short_dst)
{
	size_t out_len = 33;
	char dst_buf[DST_SIZE];
	const char *src_buf = "12345678";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 4, &out_len));
    TEST_ASSERT_EQUAL_STRING("123", dst_buf);
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

TEST(Formatter, test_dst_len_0)
{
	size_t out_len = 33;
	char dst_buf[DST_SIZE] = "W";
	const char *src_buf = "12345678";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 0, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
    TEST_ASSERT_EQUAL_STRING("W", dst_buf);
}

TEST(Formatter, test_out_len_null)
{
	char dst_buf[DST_SIZE];
	const char *src_buf = "1234";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, DST_SIZE, NULL));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
}

TEST(Formatter, test_dst_len_1_src_empty)
		{
	size_t out_len = 42;
	char dst_buf[DST_SIZE];
	const char *src_buf = "";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)out_len);
}

TEST(Formatter, test_dst_len_1_src_non_empty)
{
	size_t out_len;
	char dst_buf[DST_SIZE];
    const char *src_buf = "123";
    out_len = 33;
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

TEST(Formatter, test_copy_str_exact_fit)
{
    char dst[DST_SIZE]; // "1234\0"
    size_t out_len = 0;
    TEST_ASSERT_TRUE(try_copy_str("1234", dst, 5, &out_len));
    TEST_ASSERT_EQUAL_STRING("1234", dst);
    TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_empty_src)
{
    size_t out_len = 42;
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_copy_str("", dst, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_STRING("", dst);
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)out_len);
}

// try_uint32_to_str

TEST(Formatter, test_u32_zero)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 0, &out_len));
	TEST_ASSERT_EQUAL_STRING("0", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

TEST(Formatter, test_u32_one_digit)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 5, &out_len));
	TEST_ASSERT_EQUAL_STRING("5", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

TEST(Formatter, test_u32_multi_digit)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 12345, &out_len));
	TEST_ASSERT_EQUAL_STRING("12345", dst);
	TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

TEST(Formatter, test_u32_uint_max)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, UINT32_MAX, &out_len));
	TEST_ASSERT_EQUAL_STRING("4294967295", dst);
	TEST_ASSERT_EQUAL_UINT32(10, (uint32_t)out_len);
}

TEST(Formatter, test_u32_out_len_null)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 123, NULL));
    TEST_ASSERT_EQUAL_STRING("123", dst);
}

TEST(Formatter, test_u32_dst_size_1)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_uint32_to_str(dst, 1, 123, NULL));
}

TEST(Formatter, test_u32_dst_small)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_uint32_to_str(dst, 5, 123456, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

TEST(Formatter, test_u32_dst_exact_fit)
{
    char dst[DST_SIZE];                    // "12345\0" — впритык
    size_t out_len;
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, 6, 12345, &out_len));
    TEST_ASSERT_EQUAL_STRING("12345", dst);
    TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

TEST(Formatter, test_u32_max_dst_exact_fit)
{
    char dst[DST_SIZE];                   // "4294967295\0"
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, 11, UINT32_MAX, NULL));
    TEST_ASSERT_EQUAL_STRING("4294967295", dst);
}

// try_int32_to_str
TEST(Formatter, test_i32_positive)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, 12, &out_len));
	TEST_ASSERT_EQUAL_STRING("12", dst);
	TEST_ASSERT_EQUAL_UINT32(2, (uint32_t)out_len);
}

TEST(Formatter, test_i32_negative)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -12, &out_len));
	TEST_ASSERT_EQUAL_STRING("-12", dst);
	TEST_ASSERT_EQUAL_UINT32(3, (uint32_t)out_len);
}

TEST(Formatter, test_i32_zero)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, 0, &out_len));
	TEST_ASSERT_EQUAL_STRING("0", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

TEST(Formatter, test_i32_max)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, INT32_MAX, &out_len));
	TEST_ASSERT_EQUAL_STRING("2147483647", dst);
	TEST_ASSERT_EQUAL_UINT32(10, (uint32_t)out_len);
}

TEST(Formatter, test_i32_min)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, INT32_MIN, &out_len));
	TEST_ASSERT_EQUAL_STRING("-2147483648", dst);
	TEST_ASSERT_EQUAL_UINT32(11, (uint32_t)out_len);
}

TEST(Formatter, test_i32_minus_one)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -1, &out_len));
	TEST_ASSERT_EQUAL_STRING("-1", dst);
	TEST_ASSERT_EQUAL_UINT32(2, (uint32_t)out_len);
}

TEST(Formatter, test_i32_out_len_null)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -123, NULL));
    TEST_ASSERT_EQUAL_STRING("-123", dst);
}

TEST(Formatter, test_i32_dst_size_1)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 1, 123, NULL));
}

TEST(Formatter, test_i32_dst_small)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 5, -123456, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

TEST(Formatter, test_i32_min_dst_exact_fit)
{
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_int32_to_str(dst, 12, INT32_MIN, NULL));
    TEST_ASSERT_EQUAL_STRING("-2147483648", dst);
}

TEST(Formatter, test_i32_min_dst_too_small)
{
    char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 11, INT32_MIN, NULL));
}

// try_float_to_str
TEST(Formatter, test_f32_zero)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 0.0f, &out_len));
	TEST_ASSERT_EQUAL_STRING("0.00", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_positive)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.78f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.78", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_negative)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -102.78f, &out_len));
	TEST_ASSERT_EQUAL_STRING("-102.78", dst);
	TEST_ASSERT_EQUAL_UINT32(7, (uint32_t)out_len);
}

TEST(Formatter, test_f32_round_up)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.136f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.14", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_round_down)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.134f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.13", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_leading_zeroes)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 1.05f, &out_len));
	TEST_ASSERT_EQUAL_STRING("1.05", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_negative_small)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -0.01f, &out_len));
	TEST_ASSERT_EQUAL_STRING("-0.01", dst);
	TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

TEST(Formatter, test_f32_out_len_null)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -123.123f, NULL));
    TEST_ASSERT_EQUAL_STRING("-123.12", dst);
}

TEST(Formatter, test_f32_dst_size_1)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_float_to_str(dst, 1, 0.0f, NULL));
}

TEST(Formatter, test_f32_dst_small)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, 5, 123.45f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

TEST(Formatter, test_f32_negative_zero)
{
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -0.0f, NULL));
    TEST_ASSERT_EQUAL_STRING("0.00", dst);
}

TEST(Formatter, test_f32_large_value_rejected)
{
    char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, DST_SIZE, 4294967296.0f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

TEST(Formatter, test_f32_dst_exact_fit)
{
    char dst[DST_SIZE];
    size_t out_len;
    TEST_ASSERT_TRUE(try_float_to_str(dst, 6, 2.78f, &out_len));
    TEST_ASSERT_EQUAL_STRING("2.78", dst);
    TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

TEST(Formatter, test_f32_dst_one_short)
{
    char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_float_to_str(dst, 4, 2.78f, NULL));
}

TEST(Formatter, test_f32_negative_dst_exact_fit)
{
    char dst[6];
    size_t out_len;
    TEST_ASSERT_TRUE(try_float_to_str(dst, 6, -2.78f, &out_len));
    TEST_ASSERT_EQUAL_STRING("-2.78", dst);
    TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

TEST(Formatter, test_f32_negative_dst_one_short)
{
    char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, 5, -2.78f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

TEST_GROUP_RUNNER(Formatter)
{
    // try_copy_str
    RUN_TEST_CASE(Formatter, test_normal);
    RUN_TEST_CASE(Formatter, test_dst_null);
    RUN_TEST_CASE(Formatter, test_src_null);
    RUN_TEST_CASE(Formatter, test_short_dst);
    RUN_TEST_CASE(Formatter, test_dst_len_0);
    RUN_TEST_CASE(Formatter, test_out_len_null);
    RUN_TEST_CASE(Formatter, test_dst_len_1_src_empty);
    RUN_TEST_CASE(Formatter, test_dst_len_1_src_non_empty);
    RUN_TEST_CASE(Formatter, test_copy_str_exact_fit);
    RUN_TEST_CASE(Formatter, test_empty_src);

    // try_uint32_to_str
    RUN_TEST_CASE(Formatter, test_u32_zero);
    RUN_TEST_CASE(Formatter, test_u32_one_digit);
    RUN_TEST_CASE(Formatter, test_u32_multi_digit);
    RUN_TEST_CASE(Formatter, test_u32_uint_max);
    RUN_TEST_CASE(Formatter, test_u32_out_len_null);
    RUN_TEST_CASE(Formatter, test_u32_dst_size_1);
    RUN_TEST_CASE(Formatter, test_u32_dst_small);
    RUN_TEST_CASE(Formatter, test_u32_dst_exact_fit);
    RUN_TEST_CASE(Formatter, test_u32_max_dst_exact_fit);

    // try_int32_to_str
    RUN_TEST_CASE(Formatter, test_i32_positive);
    RUN_TEST_CASE(Formatter, test_i32_negative);
    RUN_TEST_CASE(Formatter, test_i32_zero);
    RUN_TEST_CASE(Formatter, test_i32_max);
    RUN_TEST_CASE(Formatter, test_i32_min);
    RUN_TEST_CASE(Formatter, test_i32_minus_one);
    RUN_TEST_CASE(Formatter, test_i32_out_len_null);
    RUN_TEST_CASE(Formatter, test_i32_dst_size_1);
    RUN_TEST_CASE(Formatter, test_i32_dst_small);
    RUN_TEST_CASE(Formatter, test_i32_min_dst_exact_fit);
    RUN_TEST_CASE(Formatter, test_i32_min_dst_too_small);

    // try_float_to_str
    RUN_TEST_CASE(Formatter, test_f32_zero);
    RUN_TEST_CASE(Formatter, test_f32_positive);
    RUN_TEST_CASE(Formatter, test_f32_negative);
    RUN_TEST_CASE(Formatter, test_f32_round_up);
    RUN_TEST_CASE(Formatter, test_f32_round_down);
    RUN_TEST_CASE(Formatter, test_f32_leading_zeroes);
    RUN_TEST_CASE(Formatter, test_f32_negative_small);
    RUN_TEST_CASE(Formatter, test_f32_out_len_null);
    RUN_TEST_CASE(Formatter, test_f32_dst_size_1);
    RUN_TEST_CASE(Formatter, test_f32_dst_small);
    RUN_TEST_CASE(Formatter, test_f32_negative_zero);
    RUN_TEST_CASE(Formatter, test_f32_large_value_rejected);
    RUN_TEST_CASE(Formatter, test_f32_dst_exact_fit);
    RUN_TEST_CASE(Formatter, test_f32_dst_one_short);
    RUN_TEST_CASE(Formatter, test_f32_negative_dst_exact_fit);
    RUN_TEST_CASE(Formatter, test_f32_negative_dst_one_short);
}
