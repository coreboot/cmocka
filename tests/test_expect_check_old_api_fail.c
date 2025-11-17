/*
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

#include "config.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

/*
 * Test that the OLD API properly detects failures.
 * These tests are expected to FAIL (marked with WILL_FAIL in CMakeLists.txt).
 */

static void mock_test_a(int value)
{
    check_expected(value);
}

static void mock_test_b(void *ptr)
{
    check_expected_ptr(ptr);
}

/* Custom checker for OLD API */
static int failing_checker_old(uintmax_t param, uintmax_t check)
{
    assert_int_equal(param, check);
    return 1;
}

static void test_expect_check_old_api_fail_mismatch(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, failing_checker_old, 0);
    /* This should fail - passing 1 when expecting 0 */
    mock_test_a(1);
}

static void test_expect_check_old_api_fail_missing_call(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, failing_checker_old, 0);
    /* This should fail - expected call never happens */
}

static void test_expect_check_old_api_fail_extra_call(void **state)
{
    (void)state;

    expect_check_count(mock_test_a, value, failing_checker_old, 0, 1);
    mock_test_a(0);
    /* This should fail - extra unexpected call */
    mock_test_a(0);
}

static void test_expect_check_old_api_fail_wrong_count(void **state)
{
    (void)state;

    expect_check_count(mock_test_a, value, failing_checker_old, 42, 3);
    mock_test_a(42);
    mock_test_a(42);
    /* This should fail - only 2 calls made, expected 3 */
}

/* Pointer mismatch test */
static int ptr_checker_old(uintmax_t param, uintmax_t check)
{
    void *ptr_param = (void *)(uintptr_t)param;
    void *ptr_check = (void *)(uintptr_t)check;
    assert_ptr_equal(ptr_param, ptr_check);
    return 1;
}

static void test_expect_check_old_api_fail_ptr_mismatch(void **state)
{
    char buffer1[10];
    char buffer2[10];
    (void)state;

    expect_check(mock_test_b,
                 ptr,
                 ptr_checker_old,
                 cast_ptr_to_uintmax_type(buffer1));
    /* This should fail - different pointer */
    mock_test_b(buffer2);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_check_old_api_fail_mismatch),
        cmocka_unit_test(test_expect_check_old_api_fail_missing_call),
        cmocka_unit_test(test_expect_check_old_api_fail_extra_call),
        cmocka_unit_test(test_expect_check_old_api_fail_wrong_count),
        cmocka_unit_test(test_expect_check_old_api_fail_ptr_mismatch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
