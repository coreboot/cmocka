#include "config.h"

#include <cmocka.h>
#include <cmocka_private.h>
#include <stdio.h>
#include <string.h>

static void mock_test_a(int value)
{
    check_expected(value);
}

int custom_checker(CMockaValueData param, CMockaValueData check);
int custom_checker(CMockaValueData param, CMockaValueData check)
{
    assert_int_equal(param.uint_val, check.uint_val);
    return true;
}

static void test_expect_check(void **state)
{
    (void)state; /* unused */
    expect_check(mock_test_a,
                 value,
                 custom_checker,
                 cast_int_to_cmocka_value(0));
    mock_test_a(0);
}

static void test_expect_check_count(void **state)
{
    (void)state; /* unused */
    expect_check_count(
        mock_test_a, value, custom_checker, cast_int_to_cmocka_value(0), 2);
    mock_test_a(0);
    mock_test_a(0);
}

static void test_expect_check_count_always(void **state)
{
    (void)state; /* unused */
    expect_check_count(mock_test_a,
                       value,
                       custom_checker,
                       cast_int_to_cmocka_value(0),
                       EXPECT_ALWAYS);
    mock_test_a(0);
    mock_test_a(0);
    mock_test_a(0);
    mock_test_a(0);
}

static void test_expect_check_count_maybe_1(void **state)
{
    (void)state; /* unused */
    expect_check_count(mock_test_a,
                       value,
                       custom_checker,
                       cast_int_to_cmocka_value(0),
                       EXPECT_MAYBE);
    mock_test_a(0);
    mock_test_a(0);
    mock_test_a(0);
    mock_test_a(0);
}

static void test_expect_check_count_maybe_2(void **state)
{
    (void)state; /* unused */
    expect_check_count(mock_test_a,
                       value,
                       custom_checker,
                       cast_int_to_cmocka_value(0),
                       EXPECT_MAYBE);
}

static void mock_test_ptr(const void *value)
{
    check_expected_ptr(value);
}

static void test_expect_string(void **state)
{
    (void)state; /* unused */
    char string[64];
    snprintf(string, sizeof(string), "hello world");
    assert_ptr_not_equal(string,
                         "hello world"); // should be different memory addresses

    expect_string(mock_test_ptr, value, string);
    mock_test_ptr("hello world");

    expect_not_string(mock_test_ptr, value, string);
    mock_test_ptr("hello world with extra bytes");

    expect_memory(mock_test_ptr, value, string, strlen(string));
    mock_test_ptr("hello world");

    expect_not_memory(mock_test_ptr, value, string, strlen(string));
    mock_test_ptr("different data");
}

static void test_expect_string_count_always(void **state)
{
    (void)state; /* unused */
    char string[64];
    snprintf(string, sizeof(string), "hello world");
    assert_ptr_not_equal(string,
                         "hello world"); // should be different memory addresses

    expect_string_count(mock_test_ptr, value, string, EXPECT_ALWAYS);
    mock_test_ptr("hello world");
    mock_test_ptr("hello world");
}

static void test_expect_string_count_maybe_1(void **state)
{
    (void)state; /* unused */
    char string[64];
    snprintf(string, sizeof(string), "hello world");
    assert_ptr_not_equal(string,
                         "hello world"); // should be different memory addresses

    expect_string_count(mock_test_ptr, value, string, EXPECT_MAYBE);
    mock_test_ptr("hello world");
    mock_test_ptr("hello world");
}

static void test_expect_string_count_maybe_2(void **state)
{
    (void)state; /* unused */
    const char string[] = "hello world";
    expect_string_count(mock_test_ptr, value, string, EXPECT_MAYBE);
}

static void mock_test_b(double value)
{
    check_expected_float(value);
}

static void test_expect_float(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_float(mock_test_b, value, d, precision);
    mock_test_b(1.6180339);
}

static void test_expect_float_count(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_float_count(mock_test_b, value, d, precision, 3);
    mock_test_b(1.6180339);
    mock_test_b(1.6180339);
    mock_test_b(1.6180339);
}

static void test_expect_float_count_always(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_float_count(mock_test_b, value, d, precision, EXPECT_ALWAYS);
    mock_test_b(1.6180339);
    mock_test_b(1.6180339);
}

static void test_expect_float_count_maybe_1(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_float_count(mock_test_b, value, d, precision, EXPECT_MAYBE);
    mock_test_b(1.6180339);
    mock_test_b(1.6180339);
}

static void test_expect_float_count_maybe_2(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_float_count(mock_test_b, value, d, precision, EXPECT_MAYBE);
}

static void test_expect_not_float(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_not_float(mock_test_b, value, d, precision);
    mock_test_b(2.71828); /* pi != e, so this passes */
}

static void test_expect_not_float_count(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_not_float_count(mock_test_b, value, d, precision, 3);
    mock_test_b(2.71828);
    mock_test_b(3.14159);
    mock_test_b(1.41421);
}

static void test_expect_not_float_count_always(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_not_float_count(mock_test_b, value, d, precision, EXPECT_ALWAYS);
    mock_test_b(2.71828);
    mock_test_b(3.14159);
}

static void test_expect_not_float_count_maybe_1(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_not_float_count(mock_test_b, value, d, precision, EXPECT_MAYBE);
    mock_test_b(2.71828);
    mock_test_b(3.14159);
}

static void test_expect_not_float_count_maybe_2(void **state)
{
    (void)state; /* unused */
    double d = 1.61803398875;
    double precision = 0.0000001;
    expect_not_float_count(mock_test_b, value, d, precision, EXPECT_MAYBE);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_check),
        cmocka_unit_test(test_expect_check_count),
        cmocka_unit_test(test_expect_check_count_always),
        cmocka_unit_test(test_expect_check_count_maybe_1),
        cmocka_unit_test(test_expect_check_count_maybe_2),
        cmocka_unit_test(test_expect_string),
        cmocka_unit_test(test_expect_string_count_always),
        cmocka_unit_test(test_expect_string_count_maybe_1),
        cmocka_unit_test(test_expect_string_count_maybe_2),
        cmocka_unit_test(test_expect_float),
        cmocka_unit_test(test_expect_float_count),
        cmocka_unit_test(test_expect_float_count_always),
        cmocka_unit_test(test_expect_float_count_maybe_1),
        cmocka_unit_test(test_expect_float_count_maybe_2),
        cmocka_unit_test(test_expect_not_float),
        cmocka_unit_test(test_expect_not_float_count),
        cmocka_unit_test(test_expect_not_float_count_always),
        cmocka_unit_test(test_expect_not_float_count_maybe_1),
        cmocka_unit_test(test_expect_not_float_count_maybe_2)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
