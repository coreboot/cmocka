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
 * @file assert_module_test.c
 * @brief Example demonstrating expect_assert_failure() with cmocka
 *
 * This test suite demonstrates a powerful cmocka feature: testing that
 * functions correctly assert on invalid input using expect_assert_failure().
 *
 * Key concepts demonstrated:
 * 1. Testing code that uses assert() without crashing the test program
 * 2. Using expect_assert_failure() to verify assertions fire correctly
 * 3. Distinguishing between assert() and runtime if-checks in tests
 *
 * This example intentionally includes both passing and failing tests to
 * show the different behaviors.
 *
 * NOTE: This test suite is designed to have FAILING tests to demonstrate
 * what happens when assertions don't fire as expected.
 */

#include <cmocka.h>

#include "assert_module.h"

extern void increment_value(int *const value);

/**
 * @brief Test increment_value() without expect_assert_failure
 *
 * This test FAILS because:
 * - increment_value(NULL) triggers assert(value)
 * - Without expect_assert_failure(), the assertion is treated as a failure
 * - cmocka catches the assertion and reports it as a test failure
 *
 * This demonstrates what happens when you call a function that asserts
 * without wrapping it in expect_assert_failure().
 */
static void increment_value_fail(void **state)
{
    (void)state; /* unused */

    /* BUG: Calling function that asserts without expect_assert_failure()
     * This will FAIL because the assert() fires but wasn't expected.
     */
    increment_value(NULL);
}

/**
 * @brief Test that increment_value() correctly asserts on NULL
 *
 * This test PASSES because:
 * - expect_assert_failure() tells cmocka to expect an assertion
 * - increment_value(NULL) triggers assert(value) as expected
 * - The assertion is caught and treated as success
 *
 * This is the correct way to test that a function validates input
 * using assertions.
 */
static void increment_value_assert(void **state)
{
    (void)state; /* unused */

    /* expect_assert_failure() wraps the call and expects assert() to fire.
     * This test PASSES because increment_value() does assert on NULL.
     */
    expect_assert_failure(increment_value(NULL));
}

/**
 * @brief Test decrement_value() with expect_assert_failure
 *
 * This test FAILS because:
 * - expect_assert_failure() expects an assertion to fire
 * - decrement_value() uses an if-check, not assert()
 * - No assertion fires, so the expectation is not met
 *
 * This demonstrates the difference between assert() and runtime checks:
 * - increment_value() uses assert() - testable with expect_assert_failure()
 * - decrement_value() uses if-check - not detectable by expect_assert_failure()
 *
 * Key lesson: expect_assert_failure() only works with actual assert() calls,
 * not with runtime if-checks or other validation methods.
 */
static void decrement_value_fail(void **state)
{
    (void)state; /* unused */

    /* BUG: This test will FAIL!
     * expect_assert_failure() expects an assert() to fire, but
     * decrement_value() uses an if-check instead of assert().
     * No assertion fires, so the test fails.
     */
    expect_assert_failure(decrement_value(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(increment_value_fail),
        cmocka_unit_test(increment_value_assert),
        cmocka_unit_test(decrement_value_fail),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
