#include <cmocka.h>

/*
 * This test is based on test_skip_filter.c but using environment variables
 * instead of programmatically setting the filters.
 */

static void test_skip1(void **state)
{
    (void)state;

    assert_true(1);
}

static void test_skip2(void **state)
{
    (void)state;

    assert_false(1);
}

static void test_fail(void **state)
{
    (void)state;

    assert_false(1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_skip1),
        cmocka_unit_test(test_skip2),
        cmocka_unit_test(test_fail),
    };

    /*
     * Set environment variables before running this:
     * CMOCKA_TEST_FILTER=test_skip*
     * CMOCKA_SKIP_FILTER=test_skip2
     */

    return cmocka_run_group_tests(tests, NULL, NULL);
}
