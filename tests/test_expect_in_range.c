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


static void mock_test_float(double value)
{
    check_expected_float(value);
}

static void mock_test_int(int value)
{
    check_expected(value);
}

static void test_expect_float_in_range(void **state)
{
    (void)state; /* unused */

    expect_float_in_range(mock_test_float, value, -0.61234, 0.65432, 0.00001);

    mock_test_float(0.01);
}

static void test_expect_float_in_range_count(void **state)
{
    (void)state; /* unused */

    expect_float_in_range_count(mock_test_float, value, -0.61234, 0.65432, 0.00001, 4);

    mock_test_float(0.01);
    mock_test_float(-0.6123);
    mock_test_float(0.6543);
    mock_test_float(0.65432901);
}

static void test_expect_float_not_in_range(void **state)
{
    (void)state; /* unused */

    expect_float_not_in_range(mock_test_float, value, -0.61234, 0.65432, 0.00001);

    mock_test_float(0.655);
}

static void test_expect_float_not_in_range_count(void **state)
{
    (void)state; /* unused */

    expect_float_not_in_range_count(mock_test_float, value, -0.61234, 0.65432, 0.00001, 4);

    mock_test_float(10.0);
    mock_test_float(-0.62);
    mock_test_float(0.6545);
    mock_test_float(0.659);
}

static void test_expect_int_in_range(void **state)
{
    (void)state; /* unused */

    expect_in_range(mock_test_int, value, 0, 10);
    mock_test_int(0);
}

static void test_expect_int_in_range_count(void **state)
{
    (void)state; /* unused */

    expect_in_range_count(mock_test_int, value, 0, 10, 4);
    mock_test_int(0);
    mock_test_int(5);
    mock_test_int(1);
    mock_test_int(10);
}

static void test_expect_int_not_in_range(void **state)
{
    (void)state; /* unused */

    expect_not_in_range(mock_test_int, value, 0, 10);
    mock_test_int(11);
}

static void test_expect_int_not_in_range_count(void **state)
{
    (void)state; /* unused */

    expect_not_in_range_count(mock_test_int, value, 5, 10, 4);
    mock_test_int(11);
    mock_test_int(4);
    mock_test_int(12);
    mock_test_int(3);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_float_in_range),
        cmocka_unit_test(test_expect_float_in_range_count),
        cmocka_unit_test(test_expect_float_not_in_range),
        cmocka_unit_test(test_expect_float_not_in_range_count),
        cmocka_unit_test(test_expect_int_in_range),
        cmocka_unit_test(test_expect_int_in_range_count),
        cmocka_unit_test(test_expect_int_not_in_range),
        cmocka_unit_test(test_expect_int_not_in_range_count),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
