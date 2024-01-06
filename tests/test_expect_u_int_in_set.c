#include "config.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>
#include <stdio.h>
#include <string.h>


static void mock_test_int(intmax_t value)
{
    check_expected(value);
}

static void test_expect_int_in_set_count(void **state)
{
    intmax_t set[] = { -1, 0, 1 };
    (void)state; /* unused */

    expect_int_in_set_count(mock_test_int, value, set, 1);

    mock_test_int(-1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_int_in_set_count)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
