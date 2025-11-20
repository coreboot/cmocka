#include "config.h"

#include <limits.h>

#include <cmocka.h>
#include <cmocka_private.h>

static void test_assert_int_in_range(void **state)
{
    (void)state; /* unused */
    assert_int_in_range(0, 0, 1);
    assert_int_in_range(1, 0, 1);
    assert_int_in_range(0, -1, 1);
    assert_int_in_range(0, INTMAX_MIN, INTMAX_MAX);
}

static void test_assert_int_not_in_range(void **state)
{
    (void)state; /* unused */
    assert_int_not_in_range(-1, 0, 1);
    assert_int_not_in_range(2, 0, 1);
    assert_int_not_in_range(2, -1, 1);
    assert_int_not_in_range(INTMAX_MIN, INTMAX_MIN + 1, INTMAX_MAX);
}

static void test_assert_uint_in_range(void **state)
{
    (void)state; /* unused */
    assert_uint_in_range(0, 0, 1);
    assert_uint_in_range(1, 0, 1);
    assert_uint_in_range(1, 0, 1);
    assert_uint_in_range(100, 0, UINTMAX_MAX);
}

static void test_assert_uint_not_in_range(void **state)
{
    (void)state; /* unused */
    assert_int_not_in_range(2, 0, 1);
    assert_int_not_in_range(0, 1, UINTMAX_MAX);
}

static void test_assert_uint_in_range_limits(void **state)
{
    (void)state; /* unused */
    /* Test with values greater than UINT_MAX */
    uintmax_t large_value = (uintmax_t)UINT_MAX + 1000;
    uintmax_t large_min = (uintmax_t)UINT_MAX + 1;
    uintmax_t large_max = (uintmax_t)UINT_MAX + 2000;

    assert_uint_in_range(large_value, large_min, large_max);
    assert_uint_in_range(large_min, large_min, large_max);
    assert_uint_in_range(large_max, large_min, large_max);
    assert_uint_in_range(UINTMAX_MAX, 0, UINTMAX_MAX);
}

static void test_assert_float_in_range(void **state)
{
    (void)state; /* unused */
    assert_float_in_range(0.001, 0.001, 0.01, 0.00001);
    assert_float_in_range(0.0001, 0.001, 0.1, 0.1);
    assert_float_in_range(0.001, 0.001, 0.01, 0.001);
    assert_float_in_range(0.3210, 0.20132013, 0.4013, 0.1);
    assert_float_in_range(-4.32103, -5.5, 0.0, 1.0);
}

static void test_assert_float_not_in_range(void **state)
{
    (void)state; /* unused */
    assert_float_not_in_range(0.0001, 0.01, 0.0001, 0.001);
    assert_float_not_in_range(0.0001, 0.001, 0.1, 0.0001);
    assert_float_not_in_range(0.1, 0.01, 0.001, 0.001);
    assert_float_not_in_range(0.6210, 0.20132013, 0.4013, 0.1);
    assert_float_not_in_range(-4.8, 0.2, -5.0, 0.1);
}

int main(void) {
    const struct CMUnitTest range_tests[] = {
        cmocka_unit_test(test_assert_int_in_range),
        cmocka_unit_test(test_assert_int_not_in_range),
        cmocka_unit_test(test_assert_uint_in_range),
        cmocka_unit_test(test_assert_uint_not_in_range),
        cmocka_unit_test(test_assert_uint_in_range_limits),
        cmocka_unit_test(test_assert_float_in_range),
        cmocka_unit_test(test_assert_float_not_in_range),
    };

    return cmocka_run_group_tests(range_tests, NULL, NULL);
}
