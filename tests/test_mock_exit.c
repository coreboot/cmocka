#include "config.h"

#include <cmocka.h>
#include <cmocka_private.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * This test verifies that when mock() is called outside of a test case,
 * the error message is properly printed before the program exits.
 * This is important for debugging cases where users accidentally mock
 * low-level functions that cmocka itself uses (like calloc).
 */

static int mock_function(void)
{
    /* Calling mock() outside of a test should print an error message */
    return mock_int();
}

static void test_mock_outside_test(void **state)
{
    (void)state;

    /* Call mock function without setting up will_return */
    /* This triggers the same code path as calling mock() outside a test */
    mock_function();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_mock_outside_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
