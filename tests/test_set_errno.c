
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdlib.h>

void mock_errno_function(void);
void mock_function_call_times(size_t times, int expectedValue);

void mock_errno_function(void)
{
    mock_errno();
}

void mock_function_call_times(size_t times, int expectedValue)
{
    size_t i;
    for (i = 0u; i < times; ++i)
    {
        errno = 0;
        mock_errno_function();
        assert_int_equal(errno, expectedValue);
    }
}

static void test_will_set_errno_maybe_for_no_calls(void **state)
{
    (void) state;

    will_set_errno_maybe(mock_errno_function, -3)
}

static void test_will_set_errno_maybe_for_one_mock_call(void **state)
{
    int value;

    (void) state;

    value = rand();
    will_set_errno_maybe(mock_errno_function, value);
    mock_function_call_times(1u, value);
}

static void test_will_set_errno_maybe_for_more_than_one_call(void **state)
{
    int value;
    size_t numberOfCalls;
    (void)state;

    value = rand();
    numberOfCalls = (size_t) ((rand()) % 20 + 2);
    will_set_errno_maybe(mock_errno_function, value);
    mock_function_call_times(numberOfCalls, value);
}

static void test_will_set_errno_always_for_one_mock_call(void **state)
{
    int value;
    (void) state;

    value = rand();
    will_set_errno_always(mock_errno_function, value);
    mock_function_call_times(1u, value);
}

static void test_will_set_errno_always_for_more_than_one_call(void **state)
{
    int value;
    size_t numberOfCalls;
    (void)state;

    value = rand();
    numberOfCalls = (size_t) ((rand()) % 20 + 2);
    will_set_errno_always(mock_errno_function, value);
    mock_function_call_times(numberOfCalls, value);
}


static void test_set_errno(void **state)
{
    intmax_t value;

    (void)state; /* unused */

    value = rand();
    will_set_errno(mock_errno_function, value);
    mock_errno_function();
    assert_int_equal(errno, value);
}

int main(int argc, char **argv) {
    const struct CMUnitTest alloc_tests[] = {
        cmocka_unit_test(test_will_set_errno_maybe_for_no_calls),
        cmocka_unit_test(test_will_set_errno_maybe_for_one_mock_call),
        cmocka_unit_test(test_will_set_errno_maybe_for_more_than_one_call),
        cmocka_unit_test(test_will_set_errno_always_for_one_mock_call),
        cmocka_unit_test(test_will_set_errno_always_for_more_than_one_call),
        cmocka_unit_test(test_set_errno),
    };

    (void)argc;
    (void)argv;

    return cmocka_run_group_tests(alloc_tests, NULL, NULL);
}
