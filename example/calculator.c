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
 * @file calculator.c
 * @brief A simple calculator demonstrating the cmocka testing library.
 *
 * This file implements a command-line calculator that performs basic arithmetic
 * operations (addition, subtraction, multiplication, division) on integers.
 * It demonstrates how to structure code for testing with cmocka by using
 * function pointers, modular design, and conditional compilation for test
 * hooks.
 *
 * Example usage:
 *   ./calculator 10 + 5 - 3
 *   Output:
 *     10
 *       + 5 = 15
 *       - 3 = 12
 *     = 12
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UNIT_TESTING

/**
 * When built for unit testing, redirect printf to a mock function so tests
 * can verify output.
 */
#ifdef printf
#undef printf
#endif
extern int example_test_printf(const char *format, ...);
#define printf example_test_printf

/**
 * When built for unit testing, redirect fprintf to a mock function so tests
 * can verify error messages.
 */
#ifdef fprintf
#undef fprintf
#endif
#define fprintf example_test_fprintf
extern int example_test_fprintf(FILE *const file, const char *format, ...);

/**
 * Redirect assert to mock_assert() so assertions can be caught by cmocka.
 */
#ifdef assert
#undef assert
#endif
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__)
void mock_assert(const int result,
                 const char *expression,
                 const char *file,
                 const int line);

/**
 * Rename main to example_main so the test suite can define its own main.
 */
int example_main(int argc, char *argv[]);
#define main example_main

/**
 * Expose static functions to tests by removing the static keyword.
 */
#define static

#endif /* UNIT_TESTING */

/**
 * @typedef BinaryOperator
 * @brief Function pointer type for binary arithmetic operations.
 *
 * @param a First operand
 * @param b Second operand
 * @return Result of the operation
 */
typedef int (*BinaryOperator)(int a, int b);

/**
 * @struct OperatorFunction
 * @brief Maps operator symbols (e.g., "+", "-") to their implementation
 * functions.
 */
struct OperatorFunction {
    const char *operator;    /**< Operator symbol (e.g., "+", "-") */
    BinaryOperator function; /**< Function implementing the operation */
};

/* Function declarations */
BinaryOperator find_operator_function_by_string(
    const size_t number_of_operator_functions,
    const struct OperatorFunction *const operator_functions,
    const char *const operator_string);

int perform_operation(int number_of_arguments,
                      const char *const *arguments,
                      const size_t number_of_operator_functions,
                      const struct OperatorFunction *const operator_functions,
                      size_t *const number_of_intermediate_values,
                      int **const intermediate_values,
                      int *const error_occurred);

static int add(int a, int b);
static int subtract(int a, int b);
static int multiply(int a, int b);
static int divide(int a, int b);

/**
 * @brief Lookup table mapping operator symbols to functions.
 */
static struct OperatorFunction operator_function_map[] = {
    {"+", add},
    {"-", subtract},
    {"*", multiply},
    {"/", divide},
};

/**
 * @brief Adds two integers.
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
static int add(int a, int b)
{
    return a + b;
}

/**
 * @brief Subtracts one integer from another.
 *
 * @param a First operand (minuend)
 * @param b Second operand (subtrahend)
 * @return Difference (a - b)
 */
static int subtract(int a, int b)
{
    return a - b;
}

/**
 * @brief Multiplies two integers.
 *
 * @param a First operand
 * @param b Second operand
 * @return Product of a and b
 */
static int multiply(int a, int b)
{
    return a * b;
}

/**
 * @brief Divides one integer by another.
 *
 * @param a Dividend
 * @param b Divisor
 * @return Quotient (a / b)
 *
 * @note Asserts if b is zero to prevent division by zero.
 */
static int divide(int a, int b)
{
    assert(b); /* Check for divide by zero */
    return a / b;
}

/**
 * @brief Finds the function associated with an operator symbol.
 *
 * Searches through the operator_functions array to find a function matching
 * the specified operator string.
 *
 * @param number_of_operator_functions Size of the operator_functions array
 * @param operator_functions Array of OperatorFunction structures to search
 * @param operator_string Operator symbol to find (e.g., "+", "-")
 * @return Function pointer if found, NULL otherwise
 *
 * @pre operator_functions must be non-NULL if number_of_operator_functions > 0
 * @pre operator_string must be non-NULL
 */
BinaryOperator find_operator_function_by_string(
    const size_t number_of_operator_functions,
    const struct OperatorFunction *const operator_functions,
    const char *const operator_string)
{
    size_t i;

    assert(!number_of_operator_functions || operator_functions);
    assert(operator_string != NULL);

    for (i = 0; i < number_of_operator_functions; i++) {
        const struct OperatorFunction *const operator_function =
            &operator_functions[i];
        if (strcmp(operator_function->operator, operator_string) == 0) {
            return operator_function->function;
        }
    }
    return NULL;
}

/**
 * @brief Performs a sequence of binary arithmetic operations.
 *
 * Evaluates an expression given as an array of strings in the format:
 * number operator number operator number ...
 * Operations are performed left-to-right with no operator precedence.
 *
 * Example: ["10", "+", "5", "*", "2"] evaluates as ((10 + 5) * 2) = 30
 *
 * @param number_of_arguments Number of strings in the arguments array
 * @param arguments Array of strings representing the expression
 * @param number_of_operator_functions Size of operator_functions array
 * @param operator_functions Array mapping operator symbols to functions
 * @param[out] number_of_intermediate_values Number of operations performed
 * @param[out] intermediate_values Allocated array of intermediate results
 * @param[out] error_occurred Set to 1 if an error occurs, 0 otherwise
 * @return Final result of the calculation, or 0 if an error occurred
 *
 * @note The caller is responsible for freeing the intermediate_values array.
 * @note intermediate_values is set to NULL if an error occurs.
 *
 * @pre arguments must be non-NULL if number_of_arguments > 0
 * @pre operator_functions must be non-NULL if number_of_operator_functions > 0
 * @pre error_occurred, number_of_intermediate_values, and intermediate_values
 *      must be non-NULL
 */
int perform_operation(int number_of_arguments,
                      const char *const *arguments,
                      const size_t number_of_operator_functions,
                      const struct OperatorFunction *const operator_functions,
                      size_t *const number_of_intermediate_values,
                      int **const intermediate_values,
                      int *const error_occurred)
{
    char *end_of_integer;
    int value;
    int i;

    assert(!number_of_arguments || arguments);
    assert(!number_of_operator_functions || operator_functions);
    assert(error_occurred != NULL);
    assert(number_of_intermediate_values != NULL);
    assert(intermediate_values != NULL);

    *error_occurred = 0;
    *number_of_intermediate_values = 0;
    *intermediate_values = NULL;

    if (!number_of_arguments) {
        return 0;
    }

    /* Parse the first value */
    value = (int)strtol(arguments[0], &end_of_integer, 10);
    if (end_of_integer == arguments[0]) {
        fprintf(stderr,
                "Unable to parse integer from argument %s\n",
                arguments[0]);
        *error_occurred = 1;
        return 0;
    }

    /* Allocate array for intermediate results */
    *intermediate_values = malloc(((number_of_arguments - 1) / 2) *
                                  sizeof(**intermediate_values));
    if (*intermediate_values == NULL) {
        fprintf(stderr, "Failed to allocate memory for intermediate values\n");
        *error_occurred = 1;
        return 0;
    }

    /* Process operator-operand pairs */
    i = 1;
    while (i < number_of_arguments) {
        int other_value;
        const char *const operator_string = arguments[i];
        const BinaryOperator function = find_operator_function_by_string(
            number_of_operator_functions, operator_functions, operator_string);
        int *const intermediate_value = &(
            (*intermediate_values)[*number_of_intermediate_values]);

        (*number_of_intermediate_values)++;

        if (!function) {
            fprintf(stderr,
                    "Unknown operator %s, argument %d\n",
                    operator_string,
                    i);
            *error_occurred = 1;
            break;
        }
        i++;

        if (i == number_of_arguments) {
            fprintf(stderr,
                    "Binary operator %s missing argument\n",
                    operator_string);
            *error_occurred = 1;
            break;
        }

        other_value = (int)strtol(arguments[i], &end_of_integer, 10);
        if (end_of_integer == arguments[i]) {
            fprintf(stderr,
                    "Unable to parse integer %s of argument %d\n",
                    arguments[i],
                    i);
            *error_occurred = 1;
            break;
        }
        i++;

        /* Perform the operation and store the result */
        *intermediate_value = function(value, other_value);
        value = *intermediate_value;
    }

    if (*error_occurred) {
        free(*intermediate_values);
        *intermediate_values = NULL;
        *number_of_intermediate_values = 0;
        return 0;
    }

    return value;
}

/**
 * @brief Main entry point for the calculator program.
 *
 * Processes command-line arguments as an arithmetic expression and displays
 * the intermediate steps and final result.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return 0 on success, non-zero on error
 */
int main(int argc, char *argv[])
{
    int return_value;
    size_t number_of_intermediate_values;
    int *intermediate_values;

    /* Perform the operation */
    const int result = perform_operation(argc - 1,
                                         (const char *const *)&argv[1],
                                         sizeof(operator_function_map) /
                                             sizeof(operator_function_map[0]),
                                         operator_function_map,
                                         &number_of_intermediate_values,
                                         &intermediate_values,
                                         &return_value);

    /* Display the result if no errors occurred */
    if (return_value == 0 && argc > 1) {
        size_t intermediate_value_index = 0;
        size_t i;

        printf("%s\n", argv[1]);
        for (i = 2; i < (size_t)argc; i += 2) {
            assert(intermediate_value_index < number_of_intermediate_values);
            printf("  %s %s = %d\n",
                   argv[i],
                   argv[i + 1],
                   intermediate_values[intermediate_value_index++]);
        }
        printf("= %d\n", result);
    }

    if (intermediate_values) {
        free(intermediate_values);
    }

    return return_value;
}
