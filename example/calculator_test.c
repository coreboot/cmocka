/*
 * Copyright 2008 Google Inc.
 * Copyright 2025 Andreas Schneider <asn@cryptomilk.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file calculator_test.c
 * @brief Unit tests for the calculator example demonstrating cmocka features.
 *
 * This test suite demonstrates various cmocka testing capabilities including:
 * - Basic assertion testing (assert_int_equal, assert_null, etc.)
 * - Testing assertion failures (expect_assert_failure)
 * - Mocking functions and checking expected values
 * - Redirecting I/O to verify output
 * - Testing error conditions and edge cases
 *
 * The tests show how to test both individual functions and the complete
 * application behavior through example_main().
 */

#include <stdio.h>
#include <stdlib.h>

#include <cmocka.h>

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

#include <stdint.h>

#define array_length(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @brief Cast away const qualifier for passing to functions with non-const API.
 *
 * Used for example_main() which has the standard main() signature.
 */
#define discard_const_p(type, ptr) ((type)((uintptr_t)(ptr)))

/**
 * Type definitions duplicated from calculator.c to avoid needing a shared
 * header. This demonstrates how to test existing code with minimal
 * modification.
 */
typedef int (*BinaryOperator)(int a, int b);

struct OperatorFunction {
    const char *operator;
    BinaryOperator function;
};

/**
 * External function declarations for calculator.c functions.
 * These are made available through the UNIT_TESTING conditional compilation.
 */
extern int add(int a, int b);
extern int subtract(int a, int b);
extern int multiply(int a, int b);
extern int divide(int a, int b);
extern BinaryOperator find_operator_function_by_string(
    const size_t number_of_operator_functions,
    const struct OperatorFunction *const operator_functions,
    const char *const operator_string);
extern int perform_operation(
    int number_of_arguments,
    const char *const *arguments,
    const size_t number_of_operator_functions,
    const struct OperatorFunction *const operator_functions,
    size_t *const number_of_intermediate_values,
    int **const intermediate_values,
    int *const error_occurred);
extern int example_main(int argc, char *argv[]);

/* Forward declarations for mock functions */
int example_test_fprintf(FILE *const file, const char *format, ...)
    CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int example_test_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Temporary buffer for capturing printf/fprintf output in tests.
 */
static char temporary_buffer[256];

/**
 * @brief Mock fprintf function that captures and validates stderr output.
 *
 * This function replaces fprintf when UNIT_TESTING is defined in calculator.c,
 * allowing tests to verify error messages written to stderr.
 *
 * @param file File stream (must be stderr)
 * @param format Printf-style format string
 * @return Number of characters written
 */
int example_test_fprintf(FILE *const file, const char *format, ...)
{
    int return_value;
    va_list args;

    assert_true(file == stderr);
    va_start(args, format);
    return_value = vsnprintf(temporary_buffer,
                             sizeof(temporary_buffer),
                             format,
                             args);
    check_expected_ptr(temporary_buffer);
    va_end(args);

    return return_value;
}

/**
 * @brief Mock printf function that captures and validates stdout output.
 *
 * This function replaces printf when UNIT_TESTING is defined in calculator.c,
 * allowing tests to verify messages written to stdout.
 *
 * @param format Printf-style format string
 * @return Number of characters written
 */
int example_test_printf(const char *format, ...)
{
    int return_value;
    va_list args;

    va_start(args, format);
    return_value = vsnprintf(temporary_buffer,
                             sizeof(temporary_buffer),
                             format,
                             args);
    check_expected_ptr(temporary_buffer);
    va_end(args);

    return return_value;
}

/**
 * @brief Mock binary operator function for testing perform_operation().
 *
 * This function checks that it receives expected operand values and returns
 * a predetermined result, allowing tests to verify operation sequencing.
 *
 * @param a First operand
 * @param b Second operand
 * @return Mocked result value
 */
static int binary_operator(int a, int b)
{
    check_expected_int(a);
    check_expected_int(b);
    return (int)mock_int();
}

/* ========================================================================
 * Arithmetic Operation Tests
 * ======================================================================== */

/**
 * @brief Test that add() correctly adds two integers.
 */
static void test_add(void **state)
{
    (void)state;

    assert_int_equal(add(3, 3), 6);
    assert_int_equal(add(3, -3), 0);
    assert_int_equal(add(-5, -2), -7);
    assert_int_equal(add(100, 200), 300);
}

/**
 * @brief Test that subtract() correctly subtracts two integers.
 */
static void test_subtract(void **state)
{
    (void)state;

    assert_int_equal(subtract(3, 3), 0);
    assert_int_equal(subtract(3, -3), 6);
    assert_int_equal(subtract(10, 5), 5);
    assert_int_equal(subtract(-5, -2), -3);
}

/**
 * @brief Test that multiply() correctly multiplies two integers.
 */
static void test_multiply(void **state)
{
    (void)state;

    assert_int_equal(multiply(3, 3), 9);
    assert_int_equal(multiply(3, 0), 0);
    assert_int_equal(multiply(-2, 5), -10);
    assert_int_equal(multiply(-3, -4), 12);
}

/**
 * @brief Test that divide() correctly divides one integer by another.
 */
static void test_divide(void **state)
{
    (void)state;

    assert_int_equal(divide(10, 2), 5);
    assert_int_equal(divide(2, 10), 0);
    assert_int_equal(divide(100, 10), 10);
    assert_int_equal(divide(-10, 2), -5);
}

/**
 * @brief Test that divide() triggers an assertion on division by zero.
 *
 * This demonstrates cmocka's expect_assert_failure() macro for testing
 * assertion handling.
 */
static void test_divide_by_zero(void **state)
{
    (void)state;

    expect_assert_failure(divide(100, 0));
}

/* ========================================================================
 * Operator Lookup Function Tests
 * ======================================================================== */

/**
 * @brief Test that find_operator_function_by_string() asserts when passed
 *        a NULL operator_functions array with non-zero count.
 */
static void test_find_operator_function_by_string_null_functions(void **state)
{
    (void)state;

    expect_assert_failure(find_operator_function_by_string(1, NULL, "test"));
}

/**
 * @brief Test that find_operator_function_by_string() asserts when passed
 *        a NULL operator_string.
 */
static void test_find_operator_function_by_string_null_string(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };

    (void)state;

    expect_assert_failure(find_operator_function_by_string(
        array_length(operator_functions), operator_functions, NULL));
}

/**
 * @brief Test that find_operator_function_by_string() correctly handles
 *        a NULL functions array when count is 0.
 */
static void test_find_operator_function_by_string_valid_null_functions(
    void **state)
{
    (void)state;

    assert_null(find_operator_function_by_string(0, NULL, "test"));
}

/**
 * @brief Test that find_operator_function_by_string() returns NULL when
 *        the operator is not found in the table.
 */
static void test_find_operator_function_by_string_not_found(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
        {"-", binary_operator},
        {"/", binary_operator},
    };

    (void)state;

    assert_null(find_operator_function_by_string(
        array_length(operator_functions), operator_functions, "test"));
    assert_null(find_operator_function_by_string(
        array_length(operator_functions), operator_functions, "%"));
}

/**
 * @brief Test that find_operator_function_by_string() returns the correct
 *        function when the operator is found.
 */
static void test_find_operator_function_by_string_found(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", (BinaryOperator)0x12345678},
        {"-", (BinaryOperator)0xDEADBEEF},
        {"/", (BinaryOperator)0xABADCAFE},
    };

    (void)state;

    assert_int_equal(
        cast_ptr_to_uintmax_type(find_operator_function_by_string(
            array_length(operator_functions), operator_functions, "-")),
        0xDEADBEEF);
    assert_int_equal(
        cast_ptr_to_uintmax_type(find_operator_function_by_string(
            array_length(operator_functions), operator_functions, "+")),
        0x12345678);
}

/* ========================================================================
 * perform_operation() Parameter Validation Tests
 * ======================================================================== */

/**
 * @brief Test that perform_operation() asserts when arguments is NULL
 *        with non-zero count.
 */
static void test_perform_operation_null_args(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_assert_failure(perform_operation(1,
                                            NULL,
                                            array_length(operator_functions),
                                            operator_functions,
                                            &number_of_intermediate_values,
                                            &intermediate_values,
                                            &error_occurred));
}

/**
 * @brief Test that perform_operation() asserts when operator_functions is NULL
 *        with non-zero count.
 */
static void test_perform_operation_null_operator_functions(void **state)
{
    const char *args[] = {"1", "+", "2", "*", "4"};
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_assert_failure(perform_operation(array_length(args),
                                            args,
                                            1,
                                            NULL,
                                            &number_of_intermediate_values,
                                            &intermediate_values,
                                            &error_occurred));
}

/**
 * @brief Test that perform_operation() asserts when
 * number_of_intermediate_values is NULL.
 */
static void test_perform_operation_null_number_of_intermediate_values(
    void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {"1", "+", "2", "*", "4"};
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_assert_failure(perform_operation(array_length(args),
                                            args,
                                            1,
                                            operator_functions,
                                            NULL,
                                            &intermediate_values,
                                            &error_occurred));
}

/**
 * @brief Test that perform_operation() asserts when intermediate_values is
 * NULL.
 */
static void test_perform_operation_null_intermediate_values(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {"1", "+", "2", "*", "4"};
    size_t number_of_intermediate_values;
    int error_occurred;

    (void)state;

    expect_assert_failure(perform_operation(array_length(args),
                                            args,
                                            array_length(operator_functions),
                                            operator_functions,
                                            &number_of_intermediate_values,
                                            NULL,
                                            &error_occurred));
}

/* ========================================================================
 * perform_operation() Functional Tests
 * ======================================================================== */

/**
 * @brief Test that perform_operation() returns 0 when no arguments provided.
 */
static void test_perform_operation_no_arguments(void **state)
{
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    assert_int_equal(perform_operation(0,
                                       NULL,
                                       0,
                                       NULL,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     0);
    assert_int_equal(error_occurred, 0);
}

/**
 * @brief Test that perform_operation() returns an error when the first
 *        argument is not a valid integer.
 */
static void test_perform_operation_first_arg_not_integer(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {"test", "+", "2", "*", "4"};
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_string(example_test_fprintf,
                  temporary_buffer,
                  "Unable to parse integer from argument test\n");

    assert_int_equal(perform_operation(array_length(args),
                                       args,
                                       array_length(operator_functions),
                                       operator_functions,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     0);
    assert_int_equal(error_occurred, 1);
}

/**
 * @brief Test that perform_operation() returns an error when an unknown
 *        operator is encountered.
 */
static void test_perform_operation_unknown_operator(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {"1", "*", "2", "*", "4"};
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_string(example_test_fprintf,
                  temporary_buffer,
                  "Unknown operator *, argument 1\n");

    assert_int_equal(perform_operation(array_length(args),
                                       args,
                                       array_length(operator_functions),
                                       operator_functions,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     0);
    assert_int_equal(error_occurred, 1);
}

/**
 * @brief Test that perform_operation() returns an error when an operator
 *        is missing its second operand.
 */
static void test_perform_operation_missing_argument(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {
        "1",
        "+",
    };
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_string(example_test_fprintf,
                  temporary_buffer,
                  "Binary operator + missing argument\n");

    assert_int_equal(perform_operation(array_length(args),
                                       args,
                                       array_length(operator_functions),
                                       operator_functions,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     0);
    assert_int_equal(error_occurred, 1);
}

/**
 * @brief Test that perform_operation() returns an error when a non-integer
 *        follows an operator.
 */
static void test_perform_operation_no_integer_after_operator(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
    };
    const char *args[] = {
        "1",
        "+",
        "test",
    };
    size_t number_of_intermediate_values;
    int *intermediate_values;
    int error_occurred;

    (void)state;

    expect_string(example_test_fprintf,
                  temporary_buffer,
                  "Unable to parse integer test of argument 2\n");

    assert_int_equal(perform_operation(array_length(args),
                                       args,
                                       array_length(operator_functions),
                                       operator_functions,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     0);
    assert_int_equal(error_occurred, 1);
}

/**
 * @brief Test that perform_operation() correctly processes valid operations.
 *
 * This test demonstrates using mock functions to verify that operations are
 * performed in the correct order with correct operands, and that intermediate
 * values are properly stored.
 */
static void test_perform_operation(void **state)
{
    const struct OperatorFunction operator_functions[] = {
        {"+", binary_operator},
        {"*", binary_operator},
    };
    const char *args[] = {
        "1",
        "+",
        "3",
        "*",
        "10",
    };
    size_t number_of_intermediate_values;
    int *intermediate_values = NULL;
    int error_occurred;

    (void)state;

    /* Setup expected calls and return values for mock operator functions */
    /* First operation: 1 + 3 = 4 */
    expect_int_value(binary_operator, a, 1);
    expect_int_value(binary_operator, b, 3);
    will_return_int(binary_operator, 4);

    /* Second operation: 4 * 10 = 40 */
    expect_int_value(binary_operator, a, 4);
    expect_int_value(binary_operator, b, 10);
    will_return_int(binary_operator, 40);

    assert_int_equal(perform_operation(array_length(args),
                                       args,
                                       array_length(operator_functions),
                                       operator_functions,
                                       &number_of_intermediate_values,
                                       &intermediate_values,
                                       &error_occurred),
                     40);
    assert_int_equal(error_occurred, 0);

    /* Verify intermediate results */
    assert_non_null(intermediate_values);
    assert_int_equal(intermediate_values[0], 4);
    assert_int_equal(intermediate_values[1], 40);

    free(intermediate_values);
}

/* ========================================================================
 * Integration Tests for example_main()
 * ======================================================================== */

/**
 * @brief Test that example_main() handles the case of no arguments gracefully.
 */
static void test_example_main_no_args(void **state)
{
    const char *args[] = {
        "example",
    };

    (void)state;

    assert_int_equal(example_main(array_length(args),
                                  discard_const_p(char **, args)),
                     0);
}

/**
 * @brief Test that example_main() correctly processes a valid expression
 *        and produces expected output.
 *
 * This integration test verifies the complete application behavior including
 * output formatting.
 */
static void test_example_main(void **state)
{
    const char *args[] = {
        "example",
        "1",
        "+",
        "3",
        "*",
        "10",
    };

    (void)state;

    /* Verify expected output lines */
    expect_string(example_test_printf, temporary_buffer, "1\n");
    expect_string(example_test_printf, temporary_buffer, "  + 3 = 4\n");
    expect_string(example_test_printf, temporary_buffer, "  * 10 = 40\n");
    expect_string(example_test_printf, temporary_buffer, "= 40\n");

    assert_int_equal(example_main(array_length(args),
                                  discard_const_p(char **, args)),
                     0);
}

/* ========================================================================
 * Test Suite Definition and Entry Point
 * ======================================================================== */

/**
 * @brief Main test entry point that runs all calculator tests.
 *
 * @return 0 if all tests pass, non-zero otherwise
 */
int main(void)
{
    const struct CMUnitTest tests[] = {
        /* Arithmetic operation tests */
        cmocka_unit_test(test_add),
        cmocka_unit_test(test_subtract),
        cmocka_unit_test(test_multiply),
        cmocka_unit_test(test_divide),
        cmocka_unit_test(test_divide_by_zero),

        /* Operator lookup tests */
        cmocka_unit_test(test_find_operator_function_by_string_null_functions),
        cmocka_unit_test(test_find_operator_function_by_string_null_string),
        cmocka_unit_test(
            test_find_operator_function_by_string_valid_null_functions),
        cmocka_unit_test(test_find_operator_function_by_string_not_found),
        cmocka_unit_test(test_find_operator_function_by_string_found),

        /* perform_operation() parameter validation tests */
        cmocka_unit_test(test_perform_operation_null_args),
        cmocka_unit_test(test_perform_operation_null_operator_functions),
        cmocka_unit_test(
            test_perform_operation_null_number_of_intermediate_values),
        cmocka_unit_test(test_perform_operation_null_intermediate_values),

        /* perform_operation() functional tests */
        cmocka_unit_test(test_perform_operation_no_arguments),
        cmocka_unit_test(test_perform_operation_first_arg_not_integer),
        cmocka_unit_test(test_perform_operation_unknown_operator),
        cmocka_unit_test(test_perform_operation_missing_argument),
        cmocka_unit_test(test_perform_operation_no_integer_after_operator),
        cmocka_unit_test(test_perform_operation),

        /* Integration tests */
        cmocka_unit_test(test_example_main_no_args),
        cmocka_unit_test(test_example_main),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
