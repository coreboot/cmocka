#include "config.h"

#include <limits.h>
#include <cmocka.h>
#include <cmocka_private.h>

#include <stdlib.h>


static void mock_function(int *result)
{
    *result = (int)mock_parameter(result);
}

static void mock_function_int(intmax_t *result)
{
    *result = mock_parameter_int(result);
}

static void mock_function_uint(uintmax_t *result)
{
    *result = mock_parameter_uint(result);
}

static void mock_function_uint64(uint64_t *result)
{
    *result = (uint64_t)mock_parameter_uint(result);
}

static void mock_function_float(float *result)
{
    *result = mock_parameter_float(result);
}

static void mock_function_double(double *result)
{
    *result = mock_parameter_double(result);
}

static void mock_function_ptr(const char **result)
{
    *result = mock_parameter_ptr_type(result, const char *);
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

static void test_will_return_maybe_for_no_calls(void **state)
{
    (void) state;

    will_set_parameter_maybe(mock_function, result, 32);
}

static void test_will_return_maybe_for_one_mock_call(void **state)
{
    int value;

    (void) state;

    value = rand();
    will_set_parameter_maybe(mock_function, result, value);
    mock_function_call_times(1u, value);
}

static void test_will_return_maybe_for_more_than_one_call(void **state)
{
    int value;
    size_t numberOfCalls;
    (void)state;

    value = rand();
    numberOfCalls = (size_t) ((rand()) % 20 + 2);
    will_set_parameter_maybe(mock_function, result, value);
    mock_function_call_times(numberOfCalls, value);
}

static void test_will_return_int(void **state)
{
    intmax_t value;

    (void)state; /* unused */

    value = rand();
    will_set_parameter_int(mock_function_int, result, value);
    intmax_t result_param = INTMAX_MAX;
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
}

static void test_will_return_uint(void **state)
{
    uintmax_t value;

    (void)state; /* unused */

    value = rand();
    will_set_parameter_uint(mock_function_uint, result, value);
    uintmax_t result_param = UINTMAX_MAX;
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
}

static void test_will_return_uint64(void **state)
{
     uint64_t value = 86405000000UL;

    (void)state; /* unused */

    will_set_parameter_uint(mock_function_uint64, result, value);
    uint64_t result_param = UINT64_MAX;
    mock_function_uint64(&result_param);
    assert_uint_equal(value, result_param);
}

static void test_will_return_float(void **state)
{
    float value = 1.0f;

    (void)state; /* unused */

    will_set_parameter_float(mock_function_float, result, value);
    float result_param = 0.0f;
    mock_function_float(&result_param);
    assert_float_equal(value, result_param, 0.0f);
}

static void test_will_set_parameter_double(void **state)
{
    double value = 2.5;

    (void)state; /* unused */

    will_set_parameter_double(mock_function_double, result, value);
    double result_param = 0.0;
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.0);
}

static void test_mock_parameter_double(void **state)
{
    double value = 3.14159;

    (void)state; /* unused */

    will_set_parameter_double(mock_function_double, result, value);
    double result_param = 0.0;
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.001);
}

static void test_will_return_ptr(void **state)
{
    const char *value = "What a Wurst!";

    (void)state; /* unused */

    will_set_parameter_ptr_type(mock_function_ptr, result, value, const char *);
    const char *result_param = NULL;
    mock_function_ptr(&result_param);
    assert_string_equal(value, result_param);
}

static void test_will_set_parameter_int_count(void **state)
{
    intmax_t value = -100;

    (void)state; /* unused */

    will_set_parameter_int_count(mock_function_int, result, value, 3);
    intmax_t result_param;
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
}

static void test_will_set_parameter_uint_count(void **state)
{
    uintmax_t value = 999;

    (void)state; /* unused */

    will_set_parameter_uint_count(mock_function_uint, result, value, 3);
    uintmax_t result_param;
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
}

static void test_will_set_parameter_float_count(void **state)
{
    float value = 3.14f;

    (void)state; /* unused */

    will_set_parameter_float_count(mock_function_float, result, value, 2);
    float result_param;
    mock_function_float(&result_param);
    assert_float_equal(value, result_param, 0.01f);
    mock_function_float(&result_param);
    assert_float_equal(value, result_param, 0.01f);
}

static void test_will_set_parameter_double_count(void **state)
{
    double value = 2.71828;

    (void)state; /* unused */

    will_set_parameter_double_count(mock_function_double, result, value, 2);
    double result_param;
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.0001);
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.0001);
}

static void test_will_set_parameter_int_always(void **state)
{
    intmax_t value = -50;

    (void)state; /* unused */

    will_set_parameter_int_always(mock_function_int, result, value);
    intmax_t result_param;
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
    mock_function_int(&result_param);
    assert_int_equal(value, result_param);
}

static void test_will_set_parameter_uint_always(void **state)
{
    uintmax_t value = 777;

    (void)state; /* unused */

    will_set_parameter_uint_always(mock_function_uint, result, value);
    uintmax_t result_param;
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
    mock_function_uint(&result_param);
    assert_uint_equal(value, result_param);
}

static void test_will_set_parameter_float_always(void **state)
{
    float value = 1.414f;

    (void)state; /* unused */

    will_set_parameter_float_always(mock_function_float, result, value);
    float result_param;
    mock_function_float(&result_param);
    assert_float_equal(value, result_param, 0.001f);
    mock_function_float(&result_param);
    assert_float_equal(value, result_param, 0.001f);
}

static void test_will_set_parameter_double_always(void **state)
{
    double value = 1.732050808;

    (void)state; /* unused */

    will_set_parameter_double_always(mock_function_double, result, value);
    double result_param;
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.0001);
    mock_function_double(&result_param);
    assert_double_equal(value, result_param, 0.0001);
}

static void test_will_set_parameter_int_maybe(void **state)
{
    intmax_t value = -123;

    (void)state; /* unused */

    will_set_parameter_int_maybe(mock_function_int, result, value);
    /* Maybe variants don't require the value to be consumed */
}

static void test_will_set_parameter_uint_maybe(void **state)
{
    uintmax_t value = 456;

    (void)state; /* unused */

    will_set_parameter_uint_maybe(mock_function_uint, result, value);
    /* Maybe variants don't require the value to be consumed */
}

static void test_will_set_parameter_float_maybe(void **state)
{
    float value = 2.236f;

    (void)state; /* unused */

    will_set_parameter_float_maybe(mock_function_float, result, value);
    /* Maybe variants don't require the value to be consumed */
}

static void test_will_set_parameter_double_maybe(void **state)
{
    double value = 2.23606798;

    (void)state; /* unused */

    will_set_parameter_double_maybe(mock_function_double, result, value);
    /* Maybe variants don't require the value to be consumed */
}

int main(int argc, char **argv) {
    const struct CMUnitTest alloc_tests[] = {
        cmocka_unit_test(test_will_return_maybe_for_no_calls),
        cmocka_unit_test(test_will_return_maybe_for_one_mock_call),
        cmocka_unit_test(test_will_return_maybe_for_more_than_one_call),
        cmocka_unit_test(test_will_return_int),
        cmocka_unit_test(test_will_return_uint),
        cmocka_unit_test(test_will_return_uint64),
        cmocka_unit_test(test_will_return_float),
        cmocka_unit_test(test_will_set_parameter_double),
        cmocka_unit_test(test_mock_parameter_double),
        cmocka_unit_test(test_will_return_ptr),
        cmocka_unit_test(test_will_set_parameter_int_count),
        cmocka_unit_test(test_will_set_parameter_uint_count),
        cmocka_unit_test(test_will_set_parameter_float_count),
        cmocka_unit_test(test_will_set_parameter_double_count),
        cmocka_unit_test(test_will_set_parameter_int_always),
        cmocka_unit_test(test_will_set_parameter_uint_always),
        cmocka_unit_test(test_will_set_parameter_float_always),
        cmocka_unit_test(test_will_set_parameter_double_always),
        cmocka_unit_test(test_will_set_parameter_int_maybe),
        cmocka_unit_test(test_will_set_parameter_uint_maybe),
        cmocka_unit_test(test_will_set_parameter_float_maybe),
        cmocka_unit_test(test_will_set_parameter_double_maybe),
    };

    (void)argc;
    (void)argv;

    return cmocka_run_group_tests(alloc_tests, NULL, NULL);
}
