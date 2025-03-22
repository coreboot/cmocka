#include "config.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>

static void test_assert_double_equal(void **state)
{
    assert_double_equal(0.5f, 1.f / 2.f, 0.000001f);
    assert_double_equal(-0.5f, -1.f / 2.f, 0.000001f);
}

static void test_assert_float_equal(void **state)
{
    assert_float_equal(0.5f, 1.f / 2.f, 0.000001f);
    assert_float_equal(-0.5f, -1.f / 2.f, 0.000001f);
}

static void test_assert_double_not_equal(void **state)
{
    assert_double_not_equal(0.5f, 1.f, 0.000001f);
}
static void test_assert_float_not_equal(void **state)
{
    assert_float_not_equal(0.5f, 1.f, 0.000001f);
}


int main(void) {
    const struct CMUnitTest float_tests[] = {
        cmocka_unit_test(test_assert_double_equal),
        cmocka_unit_test(test_assert_double_not_equal),
        cmocka_unit_test(test_assert_float_equal),
        cmocka_unit_test(test_assert_float_not_equal),
    };

    return cmocka_run_group_tests(float_tests, NULL, NULL);
}
