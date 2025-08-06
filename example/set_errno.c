
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>

void sets_errno(void);

void test_singe_run_sets_errno(void **state);
void test_multiple_runs_sets_errno(void **state);

void sets_errno(void)
{
    mock_errno();
}

void test_singe_run_sets_errno(void **state)
{
    (void) state; /* unused */

    will_set_errno(sets_errno, -12);
    sets_errno();
    assert_int_equal(errno, -12);
}

void test_multiple_runs_sets_errno(void **state)
{
    (void) state; /* unused */

    size_t count = 3;
    will_set_errno_count(sets_errno, -12, count);

    for (size_t i = 0; i < count; i++) {
        errno = 0;
        sets_errno();
        assert_int_equal(errno, -12);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_singe_run_sets_errno),
        cmocka_unit_test(test_multiple_runs_sets_errno),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
