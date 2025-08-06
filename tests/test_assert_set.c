#include "config.h"

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

static void test_assert_int_not_in_set_32(void **state)
{
    int32_t set[] = {INT32_MIN + 1, -2, -1, 0, 1, 2, 3, INT32_MAX - 1};

    (void)state; /* unused */

    assert_int_not_in_set(INT32_MIN, set, ARRAY_SIZE(set));
    assert_int_not_in_set(-3, set, ARRAY_SIZE(set));
    assert_int_not_in_set(4, set, ARRAY_SIZE(set));
    assert_int_not_in_set(INT32_MAX, set, ARRAY_SIZE(set));
}


static void test_assert_int_not_in_set_64(void **state)
{
    int64_t set[] = {INT64_MIN + 1, -2, -1, 0, 1, 2, 3, INT64_MAX - 1};

    (void)state; /* unused */

    assert_int_not_in_set(INT64_MIN, set, ARRAY_SIZE(set));
    assert_int_not_in_set(-3, set, ARRAY_SIZE(set));
    assert_int_not_in_set(4, set, ARRAY_SIZE(set));
    assert_int_not_in_set(INT64_MAX, set, ARRAY_SIZE(set));
}

static void test_assert_uint_in_set_32(void **state)
{
    uint32_t set[] = {0, 1, 2, 3, UINT32_MAX};

    (void)state; /* unused */

    assert_int_in_set(0, set, ARRAY_SIZE(set));
    assert_int_in_set(1, set, ARRAY_SIZE(set));
    assert_int_in_set(UINT32_MAX, set, ARRAY_SIZE(set));
}

static void test_assert_uint_in_set_64(void **state)
{
    uint64_t set[] = {1, 2, 3, UINT64_MAX};

    (void)state; /* unused */

    assert_int_in_set(UINT64_MAX, set, ARRAY_SIZE(set));
}

static void test_assert_uint_not_in_set_64(void **state)
{
    uint64_t set[] = {0, 1, 2, 3, UINT64_MAX - 1};

    (void)state; /* unused */

    assert_uint_not_in_set(UINT64_MAX, set, ARRAY_SIZE(set));
    assert_uint_not_in_set(4, set, ARRAY_SIZE(set));
}

static void test_assert_float_in_set(void **state)
{
    double set[] = {3.14, 2.718, 42.0, 1.618};
    (void)state; /* unused */

    assert_float_in_set(3.14, set, ARRAY_SIZE(set), 0.01);
    assert_float_in_set(2.7, set, ARRAY_SIZE(set), 0.1);
    assert_float_in_set(2.718, set, ARRAY_SIZE(set), 0.001);
    assert_float_in_set(42.0, set, ARRAY_SIZE(set), 0.01);
}

static void test_assert_float_not_in_set(void **state)
{
    double set[] = {3.14, 2.718, 42.0, 1.618};
    (void)state; /* unused */

    assert_float_not_in_set(3.145, set, ARRAY_SIZE(set), 0.001);
    assert_float_not_in_set(0.83462, set, ARRAY_SIZE(set), 0.001);
}

int main(void) {
    const struct CMUnitTest set_tests[] = {
        cmocka_unit_test(test_assert_int_in_set_8),
        cmocka_unit_test(test_assert_int_in_set_16),
        cmocka_unit_test(test_assert_int_in_set_32),
        cmocka_unit_test(test_assert_int_in_set_64),
        cmocka_unit_test(test_assert_int_not_in_set_32),
        cmocka_unit_test(test_assert_int_not_in_set_64),
        cmocka_unit_test(test_assert_uint_in_set_32),
        cmocka_unit_test(test_assert_uint_in_set_64),
        cmocka_unit_test(test_assert_uint_not_in_set_64),
        cmocka_unit_test(test_assert_float_in_set),
        cmocka_unit_test(test_assert_float_not_in_set),
    };

    return cmocka_run_group_tests(set_tests, NULL, NULL);
}
