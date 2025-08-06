#include "config.h"

#include <cmocka.h>

static void test_ptr_equal_msg(void **state)
{
    (void)state; /* unused */
    const char *my_pointer = "wurst";
    assert_ptr_equal_msg(my_pointer, my_pointer, "my_pointer should be equal to itself");
}

static void test_ptr_not_equal_msg(void **state)
{
    (void)state; /* unused */
    assert_ptr_not_equal_msg("wurst", "brot", "\"wurst\" should be a different pointer then \"brot\"");
}

static void test_null_msg(void **state)
{
    (void)state; /* unused */
    void *my_pointer = NULL;
    assert_null_msg(my_pointer, "my_pointer should be a NULL pointer");
}

static void test_non_null_msg(void **state)
{
    (void)state; /* unused */
    const char *my_pointer = "hello world";
    assert_non_null_msg(my_pointer, "my_pointer should not be a NULL pointer");
}

int main(void)
{
    const struct CMUnitTest ptr_msg_tests[] = {
        cmocka_unit_test(test_ptr_equal_msg),
        cmocka_unit_test(test_ptr_not_equal_msg),
        cmocka_unit_test(test_null_msg),
        cmocka_unit_test(test_non_null_msg),
    };

    return cmocka_run_group_tests(ptr_msg_tests, NULL, NULL);
}
