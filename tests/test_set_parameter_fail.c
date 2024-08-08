#include "config.h"

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>

#include <stdlib.h>

static void mock_function(int *result)
{
    *result = (int) mock_parameter(result);
}

static void mock_function_uint(uintmax_t *result)
{
    *result = mock_uint();
}

static void mock_function_ptr(void **result)
{
    *result = mock_parameter_ptr_type(result, void *);
}


static void mock_function_call_times(size_t times, int expectedValue)
{
    size_t i;
    for (i = 0u; i < times; ++i)
    {
        int result_param = INT_MAX;
        mock_function(&result_param);
        assert_int_equal(expectedValue, result_param);
    }
}

static void test_will_return_fails_for_no_calls(void **state)
{
    (void) state;

    will_set_parameter(mock_function, result, 32);
}

static void test_will_return_count_fails_for_unreturned_items(void **state)
{
    int value;
    size_t numberOfCalls;

    (void) state;

    value = rand();
    numberOfCalls = (size_t) ((rand()) % 20 + 2);

    will_set_parameter_count(mock_function, result, value, numberOfCalls);
    mock_function_call_times(numberOfCalls - 1u, value);
}

static void test_will_return_always_fails_for_no_calls(void **state)
{
    int value;

    (void) state;

    value = rand();

    will_set_parameter_always(mock_function, result, value);
}

static int teardown(void **state) {
    free(*state);

    return 0;
}

static void test_will_return_int_type_mismatch(void **state)
{
    intmax_t value = rand();

    (void) state;

    will_set_parameter_int(mock_function_uint, result, value);
    uintmax_t result_param = UINT_MAX;
    mock_function_uint(&result_param);
}

static void test_will_return_ptr_type_mismatch(void **state)
{
    const char *value = "What a Wurst!";

    (void) state;

    will_set_parameter_ptr_type(mock_function_uint, result, value, const char *);
    const char *result_param = NULL;
    mock_function_ptr((void*)&result_param);
}

int main(void)
{
    const struct CMUnitTest will_set_parameter_mock_tests[] = {
        cmocka_unit_test_teardown(test_will_return_fails_for_no_calls, teardown),
        cmocka_unit_test_teardown(test_will_return_count_fails_for_unreturned_items, teardown),
        cmocka_unit_test_teardown(test_will_return_always_fails_for_no_calls, teardown),
        cmocka_unit_test_teardown(test_will_return_fails_for_no_calls, teardown),
        cmocka_unit_test(test_will_return_int_type_mismatch),
        cmocka_unit_test(test_will_return_ptr_type_mismatch),
    };

    return cmocka_run_group_tests(will_set_parameter_mock_tests, NULL, NULL);
}
