#include "config.h"

#include <cmocka.h>
#include <cmocka_private.h>
#include <stdio.h>
#include <string.h>


static void mock_test_int(intmax_t value)
{
    check_expected_int(value);
}

static void mock_test_uint(uintmax_t value)
{
    check_expected_uint(value);
}

static void mock_test_float(float value)
{
    check_expected_float(value);
}

static void test_expect_int_in_set_count(void **state)
{
    intmax_t set[] = { -1, 0, 1 };
    (void)state; /* unused */

    expect_int_in_set_count(mock_test_int, value, set, 1);

    mock_test_int(-1);
}

static void test_expect_uint_in_set_count(void **state)
{
    uintmax_t set[] = { 0, 1, 42, UINTMAX_MAX };
    (void)state; /* unused */

    expect_uint_in_set_count(mock_test_uint, value, set, 1);

    mock_test_uint(42);
}

static void test_expect_float_in_set_count(void **state)
{
    double set[] = {3.14, 2.718, 42.0, 1.618};
    (void)state; /* unused */

    expect_float_in_set_count(mock_test_float, value, set, 0.01, 1);

    mock_test_float(2.71f);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_int_in_set_count),
        cmocka_unit_test(test_expect_uint_in_set_count),
        cmocka_unit_test(test_expect_float_in_set_count)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
