#include "config.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>

static void test_assert_int_in_set_8(void **state)
{
    int8_t set[] = {1, 2, 3, INT8_MAX};

    (void)state; /* unused */

    assert_int_in_set(1, set, ARRAY_SIZE(set));
}

static void test_assert_int_in_set_16(void **state)
{
    int16_t set[] = {1, 2, 3, INT16_MAX};

    (void)state; /* unused */

    assert_int_in_set(INT16_MAX, set, ARRAY_SIZE(set));
}

static void test_assert_int_in_set_32(void **state)
{
    int32_t set[] = {INT32_MIN, -2, -1, 0, 1, 2, 3, INT32_MAX};

    (void)state; /* unused */

    assert_int_in_set(INT32_MIN, set, ARRAY_SIZE(set));
    assert_int_in_set(-1, set, ARRAY_SIZE(set));
    assert_int_in_set(0, set, ARRAY_SIZE(set));
    assert_int_in_set(1, set, ARRAY_SIZE(set));
    assert_int_in_set(INT32_MAX, set, ARRAY_SIZE(set));
}

static void test_assert_int_in_set_64(void **state)
{
    int64_t set[] = {1, 2, 3, INT64_MAX};

    (void)state; /* unused */

    assert_int_in_set(INT64_MAX, set, ARRAY_SIZE(set));
}

int main(void) {
    const struct CMUnitTest set_tests[] = {
        cmocka_unit_test(test_assert_int_in_set_8),
        cmocka_unit_test(test_assert_int_in_set_16),
        cmocka_unit_test(test_assert_int_in_set_32),
        cmocka_unit_test(test_assert_int_in_set_64),
    };

    return cmocka_run_group_tests(set_tests, NULL, NULL);
}
