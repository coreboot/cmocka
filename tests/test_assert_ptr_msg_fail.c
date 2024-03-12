#include "config.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

static void test_ptr_equal_msg_fail(void **state)
{
    (void)state; /* unused */
    const char *my_pointer = "wurst";
    assert_ptr_equal_msg(my_pointer, my_pointer + 1, "my_pointer should be equal to itself plus one");
}

static void test_ptr_not_equal_msg_fail(void **state)
{
    (void)state; /* unused */
    const char *my_pointer = "wurst";
    assert_ptr_not_equal_msg(my_pointer, my_pointer, "my_pointer should not be equal to itself");
}

static void test_null_msg_fail(void **state)
{
    (void)state; /* unused */
    assert_null_msg("wurst", "\"wurst\" should be a NULL pointer");
}

static void test_non_null_msg_fail(void **state)
{
    (void)state; /* unused */
    const char *my_pointer = NULL;
    assert_non_null_msg(my_pointer, "my_pointer should not be a NULL pointer");
}

int main(void)
{
    const struct CMUnitTest ptr_msg_tests[] = {
        cmocka_unit_test(test_ptr_equal_msg_fail),
        cmocka_unit_test(test_ptr_not_equal_msg_fail),
        cmocka_unit_test(test_null_msg_fail),
        cmocka_unit_test(test_non_null_msg_fail),
    };

    return cmocka_run_group_tests(ptr_msg_tests, NULL, NULL);
}
