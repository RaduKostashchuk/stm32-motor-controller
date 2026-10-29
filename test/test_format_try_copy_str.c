#include "unity.h"
#include "format.h"
#include <stdint.h>
#define DST_SIZE 16

void setUp(void) {}
void tearDown(void) {}

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
	char dst_buf[4];
	const char *src_buf = "12345678";
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 4, &out_len));
    TEST_ASSERT_EQUAL_STRING("123", dst_buf);
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

void test_dst_len_0(void) {
	size_t out_len = 33;
	char dst_buf[2] = "W";
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
	char dst_buf[1];
	const char *src_buf = "";
    TEST_ASSERT_TRUE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_STRING(src_buf, dst_buf);
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)out_len);
}

void test_dst_len_1_src_non_empty(void) {
	size_t out_len;
	char dst_buf[1];
    const char *src_buf = "123";
    out_len = 33;
    TEST_ASSERT_FALSE(try_copy_str(src_buf, dst_buf, 1, &out_len));
    TEST_ASSERT_EQUAL_UINT32(33, (uint32_t)out_len);
}

void test_copy_str_exact_fit(void) {
    char dst[5]; // "1234\0"
    size_t out_len = 0;
    TEST_ASSERT_TRUE(try_copy_str("1234", dst, sizeof(dst), &out_len));
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

int main(void) {
    UNITY_BEGIN();
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
    return UNITY_END();
}
