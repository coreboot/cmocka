#include "config.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>
#include <stdio.h>
#include <string.h>

static void mock_test_int(int value)
{
    check_expected(value);
}

static void mock_test_intmax(intmax_t value)
{
    check_expected(value);
}

static void mock_test_uint(uintmax_t value)
{
    check_expected(value);
}

static void test_expect_int_value_with_int(void **state)
{
    (void)state; /* unused */

    expect_int_value(mock_test_int, value, 42);
    mock_test_int(42);
}

static void test_expect_int_value_count_with_intmax(void **state)
{
    (void)state; /* unused */

    expect_int_value_count(mock_test_intmax, value, -100, 4);
    mock_test_intmax(-100);
    mock_test_intmax(-100);
    mock_test_intmax(-100);
    mock_test_intmax(-100);
}

static void test_expect_int_value_negative(void **state)
{
    (void)state; /* unused */

    expect_int_value(mock_test_int, value, -42);
    mock_test_int(-42);
}

static void test_expect_uint_value_with_uint(void **state)
{
    (void)state; /* unused */

    expect_uint_value(mock_test_uint, value, 123);
    mock_test_uint(123);
}

static void test_expect_uint_value_count_with_uintmax(void **state)
{
    (void)state; /* unused */

    expect_uint_value_count(mock_test_uint, value, 999, 4);
    mock_test_uint(999);
    mock_test_uint(999);
    mock_test_uint(999);
    mock_test_uint(999);
}

static void test_expect_uint_value_zero(void **state)
{
    (void)state; /* unused */

    expect_uint_value(mock_test_uint, value, 0);
    mock_test_uint(0);
}

static void test_expect_uint_value_max(void **state)
{
    (void)state; /* unused */

    expect_uint_value(mock_test_uint, value, UINTMAX_MAX);
    mock_test_uint(UINTMAX_MAX);
}

static void test_expect_int_not_value_with_int(void **state)
{
    (void)state; /* unused */

    expect_int_not_value(mock_test_int, value, 42);
    mock_test_int(99); /* 99 != 42, so this passes */
}

static void test_expect_int_not_value_count_with_intmax(void **state)
{
    (void)state; /* unused */

    expect_int_not_value_count(mock_test_intmax, value, -100, 4);
    mock_test_intmax(-99);  /* -99 != -100 */
    mock_test_intmax(-101); /* -101 != -100 */
    mock_test_intmax(0);    /* 0 != -100 */
    mock_test_intmax(100);  /* 100 != -100 */
}

static void test_expect_int_not_value_negative(void **state)
{
    (void)state; /* unused */

    expect_int_not_value(mock_test_int, value, -42);
    mock_test_int(42); /* 42 != -42, so this passes */
}

static void test_expect_uint_not_value_with_uint(void **state)
{
    (void)state; /* unused */

    expect_uint_not_value(mock_test_uint, value, 123);
    mock_test_uint(456); /* 456 != 123, so this passes */
}

static void test_expect_uint_not_value_count_with_uintmax(void **state)
{
    (void)state; /* unused */

    expect_uint_not_value_count(mock_test_uint, value, 999, 4);
    mock_test_uint(998);  /* 998 != 999 */
    mock_test_uint(1000); /* 1000 != 999 */
    mock_test_uint(0);    /* 0 != 999 */
    mock_test_uint(1);    /* 1 != 999 */
}

static void test_expect_uint_not_value_zero(void **state)
{
    (void)state; /* unused */

    expect_uint_not_value(mock_test_uint, value, 0);
    mock_test_uint(1); /* 1 != 0, so this passes */
}

static void test_expect_uint_not_value_max(void **state)
{
    (void)state; /* unused */

    expect_uint_not_value(mock_test_uint, value, UINTMAX_MAX);
    mock_test_uint(0); /* 0 != UINTMAX_MAX, so this passes */
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_int_value_with_int),
        cmocka_unit_test(test_expect_int_value_count_with_intmax),
        cmocka_unit_test(test_expect_int_value_negative),
        cmocka_unit_test(test_expect_uint_value_with_uint),
        cmocka_unit_test(test_expect_uint_value_count_with_uintmax),
        cmocka_unit_test(test_expect_uint_value_zero),
        cmocka_unit_test(test_expect_uint_value_max),
        cmocka_unit_test(test_expect_int_not_value_with_int),
        cmocka_unit_test(test_expect_int_not_value_count_with_intmax),
        cmocka_unit_test(test_expect_int_not_value_negative),
        cmocka_unit_test(test_expect_uint_not_value_with_uint),
        cmocka_unit_test(test_expect_uint_not_value_count_with_uintmax),
        cmocka_unit_test(test_expect_uint_not_value_zero),
        cmocka_unit_test(test_expect_uint_not_value_max),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
