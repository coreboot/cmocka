#include "config.h"

#include <errno.h>
#include <limits.h>
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

static void test_will_set_errno_fails_for_no_calls(void **state)
{
    (void) state;

    will_set_errno(mock_errno_function, -32);
}

static void test_will_set_errno_count_fails_for_unreturned_items(void **state)
{
    int value;
    size_t numberOfCalls;

    (void) state;

    value = rand();
    numberOfCalls = (size_t) ((rand()) % 20 + 2);

    will_set_errno_count(mock_errno_function, value, numberOfCalls);
    mock_function_call_times(numberOfCalls - 1u, value);
}

static void test_will_set_errno_always_fails_for_no_calls(void **state)
{
    int value;
    (void) state;

    value = rand();

    will_set_errno_always(mock_function, value);
}

int main(void)
{
    const struct CMUnitTest will_set_errno_tests[] = {
        cmocka_unit_test(test_will_set_errno_fails_for_no_calls),
        cmocka_unit_test(test_will_set_errno_count_fails_for_unreturned_items),
        cmocka_unit_test(test_will_set_errno_always_fails_for_no_calls),
    };

    return cmocka_run_group_tests(will_set_errno_tests, NULL, NULL);
}
