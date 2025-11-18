#include "config.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>

#include <stdlib.h>

/* Mock function that uses has_mock() to return mock data or default value */
int get_value(void);
int get_value(void)
{
    if (has_mock()) {
        return (int)mock();
    }
    return 100; /* default value when no mock data */
}

/* Mock function for testing will_return_always */
int get_repeating_value(void);
int get_repeating_value(void)
{
    if (has_mock()) {
        return (int)mock();
    }
    return -1; /* default value */
}

static void test_has_mock_no_data(void **state)
{
    (void)state;

    /* When no mock data is set up, should return default value */
    assert_int_equal(100, get_value());
}

static void test_has_mock_with_data(void **state)
{
    (void)state;

    /* Set up mock data for get_value */
    will_return(get_value, 42);

    /* has_mock() inside get_value should return true, so mock value is returned
     */
    assert_int_equal(42, get_value());

    /* After consuming, should return default value */
    assert_int_equal(100, get_value());
}

static void test_has_mock_doesnt_consume(void **state)
{
    int value = 123;

    (void)state;

    /* Set up mock data for get_value */
    will_return(get_value, value);

    /* First call: has_mock() is true, returns mock value */
    assert_int_equal(value, get_value());

    /* Second call: no more mock data, returns default */
    assert_int_equal(100, get_value());
}

static void test_has_mock_with_will_return_always(void **state)
{
    int value = 999;

    (void)state;

    /* Set up mock data with will_return_always */
    will_return_always(get_repeating_value, value);

    /* Call multiple times - should always return mock value */
    assert_int_equal(value, get_repeating_value());
    assert_int_equal(value, get_repeating_value());
    assert_int_equal(value, get_repeating_value());
}

static void test_has_mock_with_will_return_count(void **state)
{
    int value = 555;

    (void)state;

    /* Set up mock data with will_return_count */
    will_return_count(get_value, value, 3);

    /* Call 3 times - should return mock value */
    assert_int_equal(value, get_value());
    assert_int_equal(value, get_value());
    assert_int_equal(value, get_value());

    /* 4th call - no more mock data, should return default */
    assert_int_equal(100, get_value());
}

static void test_has_mock_conditional_use(void **state)
{
    (void)state;

    /* First call without mock data - should return default value */
    assert_int_equal(100, get_value());

    /* Second call with mock data - should return mock value */
    will_return(get_value, 200);
    assert_int_equal(200, get_value());

    /* Third call without mock data again - should return default value */
    assert_int_equal(100, get_value());
}

int main(int argc, char **argv)
{
    const struct CMUnitTest has_mock_tests[] = {
        cmocka_unit_test(test_has_mock_no_data),
        cmocka_unit_test(test_has_mock_with_data),
        cmocka_unit_test(test_has_mock_doesnt_consume),
        cmocka_unit_test(test_has_mock_with_will_return_always),
        cmocka_unit_test(test_has_mock_with_will_return_count),
        cmocka_unit_test(test_has_mock_conditional_use),
    };

    (void)argc;
    (void)argv;

    return cmocka_run_group_tests(has_mock_tests, NULL, NULL);
}
