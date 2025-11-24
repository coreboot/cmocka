/*
 * Copyright 2008 Google Inc.
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
 * @file assert_module.c
 * @brief Example demonstrating how to mock assert() with cmocka
 *
 * This file shows a critical testing technique: overriding standard assert()
 * with cmocka's mock_assert() to enable testing of assertion failures without
 * crashing the test program.
 *
 * Key concepts:
 * 1. When UNIT_TESTING is defined, assert() is replaced with mock_assert()
 * 2. mock_assert() is provided by cmocka and can be controlled in tests
 * 3. This allows testing that functions correctly assert on invalid input
 */

#include <assert.h>

#include "assert_module.h"

/*
 * If unit testing is enabled, override assert() with mock_assert().
 *
 * This is a powerful pattern for testing code that uses assertions:
 * - In production (UNIT_TESTING not defined): normal assert() behavior
 * - In tests (UNIT_TESTING defined): assertions become testable events
 *
 * The mock_assert() function is provided by cmocka and works with
 * expect_assert_failure() in test code.
 */
#ifdef UNIT_TESTING
extern void mock_assert(const int result,
                        const char *const expression,
                        const char *const file,
                        const int line);
#undef assert
#define assert(expression) \
    mock_assert(((expression) ? 1 : 0), #expression, __FILE__, __LINE__);
#endif /* UNIT_TESTING */

/**
 * @brief Increment an integer value with assertion
 *
 * This function demonstrates defensive programming with assert().
 * The assert() ensures the pointer is valid before dereferencing.
 *
 * When compiled with UNIT_TESTING, the assert becomes testable:
 * - Tests can verify the assert fires on NULL input
 * - The test doesn't crash; cmocka catches the assertion
 */
void increment_value(int *const value)
{
    assert(value);
    (*value)++;
}

/**
 * @brief Decrement an integer value with runtime check
 *
 * This function demonstrates a different validation approach: runtime
 * checking with if statements instead of assertions.
 *
 * Comparison with increment_value():
 * - increment_value() uses assert() - fails fast in debug, removed in release
 * - decrement_value() uses if check - always present, silently handles NULL
 *
 * The test suite shows how these different approaches behave differently
 * when tested with expect_assert_failure().
 */
void decrement_value(int *const value)
{
    if (value) {
        (*value)--;
    }
}
