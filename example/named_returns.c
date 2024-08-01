
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <stddef.h>
#include <cmocka.h>

void named_return_parameter(int *number, double *real, const char **text);
int internal_named_mocks(void);

void test_named_return_parameter(void **state);
void test_internal_named_mocks(void **state);

void named_return_parameter(int *number, double *real, const char **text)
{
    *real = mock_named_float(real);
    *text = mock_named_ptr_type(text, const char *);
    *number = mock_named_int(number);
}

int internal_named_mocks(void)
{
    return mock_named_int(first_number)
         + mock_named_int(second_number)
         + mock_named_int(third_number);
}

void test_named_return_parameter(void **state)
{
    (void) state; /* unused */

    int expected_number = 42;
    double expected_real = 17.46;
    const char *expected_text = "10 zahme Ziegen ziehen 10 Zentner Zement zum Zoo.";

    will_return_named_ptr_type(named_return_parameter, text, expected_text, const char *);
    will_return_named_int(named_return_parameter, number, expected_number);
    will_return_named_float(named_return_parameter, real, expected_real);

    int result_number = 0;
    double result_real = 0.0;
    const char *result_text = NULL;

    named_return_parameter(&result_number, &result_real, &result_text);

    assert_int_equal(result_number, expected_number);
    assert_double_equal(result_real, expected_real, 0.0);
    assert_ptr_equal(result_text, expected_text);
}

void test_internal_named_mocks(void **state)
{
    (void) state; /* unused */

    will_return_named_int(internal_named_mocks, second_number, 5);
    will_return_named_int(internal_named_mocks, third_number, 3);
    will_return_named_int(internal_named_mocks, first_number, 7);

    assert_int_equal(internal_named_mocks(), 5 + 3 + 7);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_named_return_parameter),
        cmocka_unit_test(test_internal_named_mocks),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
