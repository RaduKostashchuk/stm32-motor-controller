#include "unity.h"
#include "format.h"
#include <stdint.h>
#define DST_SIZE 16

// try_copy_str
void test_normal(void) {
	size_t src_len = 4;
	size_t out_len = 0;
	char dst_buf[DST_SIZE];
	const char *src_buf = "1234";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
    TEST_ASSERT_EQUAL_UINT32((uint32_t)src_len, (uint32_t)out_len);
}

void test_dst_null(void) {
	size_t out_len = 33;
	char src_buf[] = "1234";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, NULL, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
}

void test_src_null(void) {
	size_t out_len = 33;
	char dst_buf[DST_SIZE] = "prev";
	TEST_ASSERT_FALSE(try_copy_str(NULL, dst_buf, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
    TEST_ASSERT_EQUAL_STRING("prev", dst_buf);
}

void test_short_dst(void) {
	size_t out_len = 33;
	char dst_buf[DST_SIZE];
	const char *src_buf = "12345678";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 4, &out_len));
    TEST_ASSERT_EQUAL_STRING("123", dst_buf);
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

void test_dst_len_0(void) {
	size_t out_len = 33;
	char dst_buf[DST_SIZE] = "W";
	const char *src_buf = "12345678";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 0, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, out_len);
    TEST_ASSERT_EQUAL_STRING("W", dst_buf);
}

void test_out_len_null(void)
{
	char dst_buf[DST_SIZE];
	const char *src_buf = "1234";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, DST_SIZE, NULL));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
}

void test_dst_len_1_src_empty(void) {
	size_t out_len = 42;
	char dst_buf[DST_SIZE];
	const char *src_buf = "";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)out_len);
}

void test_dst_len_1_src_non_empty(void) {
	size_t out_len;
	char dst_buf[DST_SIZE];
    const char *src_buf = "123";
    out_len = 33;
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

void test_copy_str_exact_fit(void) {
    char dst[DST_SIZE]; // "1234\0"
    size_t out_len = 0;
    TEST_ASSERT_TRUE(try_copy_str("1234", dst, 5, &out_len));
    TEST_ASSERT_EQUAL_STRING("1234", dst);
    TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_empty_src(void) {
    size_t out_len = 42;
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_copy_str("", dst, DST_SIZE, &out_len));
    TEST_ASSERT_EQUAL_STRING("", dst);
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)out_len);
}

// try_uint32_to_str

void test_u32_zero(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 0, &out_len));
	TEST_ASSERT_EQUAL_STRING("0", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

void test_u32_one_digit(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 5, &out_len));
	TEST_ASSERT_EQUAL_STRING("5", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

void test_u32_multi_digit(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 12345, &out_len));
	TEST_ASSERT_EQUAL_STRING("12345", dst);
	TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

void test_u32_uint_max(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, UINT32_MAX, &out_len));
	TEST_ASSERT_EQUAL_STRING("4294967295", dst);
	TEST_ASSERT_EQUAL_UINT32(10, (uint32_t)out_len);
}

void test_u32_out_len_null(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, DST_SIZE, 123, NULL));
    TEST_ASSERT_EQUAL_STRING("123", dst);
}

void test_u32_dst_size_1(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_uint32_to_str(dst, 1, 123, NULL));
}

void test_u32_dst_small(void)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_uint32_to_str(dst, 5, 123456, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

void test_u32_dst_exact_fit(void) {
    char dst[DST_SIZE];                    // "12345\0" — впритык
    size_t out_len;
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, 6, 12345, &out_len));
    TEST_ASSERT_EQUAL_STRING("12345", dst);
    TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

void test_u32_max_dst_exact_fit(void) {
    char dst[DST_SIZE];                   // "4294967295\0"
    TEST_ASSERT_TRUE(try_uint32_to_str(dst, 11, UINT32_MAX, NULL));
    TEST_ASSERT_EQUAL_STRING("4294967295", dst);
}

// try_int32_to_str
void test_i32_positive(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, 12, &out_len));
	TEST_ASSERT_EQUAL_STRING("12", dst);
	TEST_ASSERT_EQUAL_UINT32(2, (uint32_t)out_len);
}

void test_i32_negative(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -12, &out_len));
	TEST_ASSERT_EQUAL_STRING("-12", dst);
	TEST_ASSERT_EQUAL_UINT32(3, (uint32_t)out_len);
}

void test_i32_zero(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, 0, &out_len));
	TEST_ASSERT_EQUAL_STRING("0", dst);
	TEST_ASSERT_EQUAL_UINT32(1, (uint32_t)out_len);
}

void test_i32_max(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, INT32_MAX, &out_len));
	TEST_ASSERT_EQUAL_STRING("2147483647", dst);
	TEST_ASSERT_EQUAL_UINT32(10, (uint32_t)out_len);
}

void test_i32_min(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, INT32_MIN, &out_len));
	TEST_ASSERT_EQUAL_STRING("-2147483648", dst);
	TEST_ASSERT_EQUAL_UINT32(11, (uint32_t)out_len);
}

void test_i32_minus_one(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -1, &out_len));
	TEST_ASSERT_EQUAL_STRING("-1", dst);
	TEST_ASSERT_EQUAL_UINT32(2, (uint32_t)out_len);
}

void test_i32_out_len_null(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_int32_to_str(dst, DST_SIZE, -123, NULL));
    TEST_ASSERT_EQUAL_STRING("-123", dst);
}

void test_i32_dst_size_1(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 1, 123, NULL));
}

void test_i32_dst_small(void)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 5, -123456, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

void test_i32_min_dst_exact_fit(void) {
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_int32_to_str(dst, 12, INT32_MIN, NULL));
    TEST_ASSERT_EQUAL_STRING("-2147483648", dst);
}

void test_i32_min_dst_too_small(void) {
    char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_int32_to_str(dst, 11, INT32_MIN, NULL));
}

// try_float_to_str
void test_f32_zero(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 0.0f, &out_len));
	TEST_ASSERT_EQUAL_STRING("0.00", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_positive(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.78f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.78", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_negative(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -102.78f, &out_len));
	TEST_ASSERT_EQUAL_STRING("-102.78", dst);
	TEST_ASSERT_EQUAL_UINT32(7, (uint32_t)out_len);
}

void test_f32_round_up(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.136f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.14", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_round_down(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 2.134f, &out_len));
	TEST_ASSERT_EQUAL_STRING("2.13", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_leading_zeroes(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, 1.05f, &out_len));
	TEST_ASSERT_EQUAL_STRING("1.05", dst);
	TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_negative_small(void)
{
	char dst[DST_SIZE];
	size_t out_len;
	TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -0.01f, &out_len));
	TEST_ASSERT_EQUAL_STRING("-0.01", dst);
	TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

void test_f32_out_len_null(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -123.123f, NULL));
    TEST_ASSERT_EQUAL_STRING("-123.12", dst);
}

void test_f32_dst_size_1(void)
{
	char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_float_to_str(dst, 1, 0.0f, NULL));
}

void test_f32_dst_small(void)
{
	char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, 5, 123.45f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

void test_f32_negative_zero(void) {
    char dst[DST_SIZE];
    TEST_ASSERT_TRUE(try_float_to_str(dst, DST_SIZE, -0.0f, NULL));
    TEST_ASSERT_EQUAL_STRING("0.00", dst);
}

void test_f32_large_value_rejected(void) {
    char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, DST_SIZE, 4294967296.0f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

void test_f32_dst_exact_fit(void) {
    char dst[DST_SIZE];
    size_t out_len;
    TEST_ASSERT_TRUE(try_float_to_str(dst, 6, 2.78f, &out_len));
    TEST_ASSERT_EQUAL_STRING("2.78", dst);
    TEST_ASSERT_EQUAL_UINT32(4, (uint32_t)out_len);
}

void test_f32_dst_one_short(void) {
    char dst[DST_SIZE];
    TEST_ASSERT_FALSE(try_float_to_str(dst, 4, 2.78f, NULL));
}

void test_f32_negative_dst_exact_fit(void) {
    char dst[6];
    size_t out_len;
    TEST_ASSERT_TRUE(try_float_to_str(dst, 6, -2.78f, &out_len));
    TEST_ASSERT_EQUAL_STRING("-2.78", dst);
    TEST_ASSERT_EQUAL_UINT32(5, (uint32_t)out_len);
}

void test_f32_negative_dst_one_short(void) {
    char dst[DST_SIZE] = "prev";
    TEST_ASSERT_FALSE(try_float_to_str(dst, 5, -2.78f, NULL));
    TEST_ASSERT_EQUAL_STRING("prev", dst);
}

void test_format(void)
{
    // try_copy_str
    RUN_TEST(test_normal);
    RUN_TEST(test_dst_null);
    RUN_TEST(test_src_null);
    RUN_TEST(test_short_dst);
    RUN_TEST(test_dst_len_0);
    RUN_TEST(test_out_len_null);
    RUN_TEST(test_dst_len_1_src_empty);
    RUN_TEST(test_dst_len_1_src_non_empty);
    RUN_TEST(test_copy_str_exact_fit);
    RUN_TEST(test_empty_src);

    // try_uint32_to_str
    RUN_TEST(test_u32_zero);
    RUN_TEST(test_u32_one_digit);
    RUN_TEST(test_u32_multi_digit);
    RUN_TEST(test_u32_uint_max);
    RUN_TEST(test_u32_out_len_null);
    RUN_TEST(test_u32_dst_size_1);
    RUN_TEST(test_u32_dst_small);
    RUN_TEST(test_u32_dst_exact_fit);
    RUN_TEST(test_u32_max_dst_exact_fit);

    // try_int32_to_str
    RUN_TEST(test_i32_positive);
    RUN_TEST(test_i32_negative);
    RUN_TEST(test_i32_zero);
    RUN_TEST(test_i32_max);
    RUN_TEST(test_i32_min);
    RUN_TEST(test_i32_minus_one);
    RUN_TEST(test_i32_out_len_null);
    RUN_TEST(test_i32_dst_size_1);
    RUN_TEST(test_i32_dst_small);
    RUN_TEST(test_i32_min_dst_exact_fit);
    RUN_TEST(test_i32_min_dst_too_small);

    // try_float_to_str
    RUN_TEST(test_f32_zero);
    RUN_TEST(test_f32_positive);
    RUN_TEST(test_f32_negative);
    RUN_TEST(test_f32_round_up);
    RUN_TEST(test_f32_round_down);
    RUN_TEST(test_f32_leading_zeroes);
    RUN_TEST(test_f32_negative_small);
    RUN_TEST(test_f32_out_len_null);
    RUN_TEST(test_f32_dst_size_1);
    RUN_TEST(test_f32_dst_small);
    RUN_TEST(test_f32_negative_zero);
    RUN_TEST(test_f32_large_value_rejected);
    RUN_TEST(test_f32_dst_exact_fit);
    RUN_TEST(test_f32_dst_one_short);
    RUN_TEST(test_f32_negative_dst_exact_fit);
    RUN_TEST(test_f32_negative_dst_one_short);
}
