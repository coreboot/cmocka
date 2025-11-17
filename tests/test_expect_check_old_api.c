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
 * Test the OLD API for expect_check() using uintmax_t parameters.
 * This tests backward compatibility after restoring the pre-d5ccd88 API.
 */

static void mock_test_a(int value)
{
    check_expected(value);
}

static void mock_test_b(void *ptr)
{
    check_expected_ptr(ptr);
}

/* Custom checker using OLD API signature (uintmax_t) */
static int custom_checker_old(uintmax_t param, uintmax_t check)
{
    assert_int_equal(param, check);
    return 1;
}

static void test_expect_check_old_api_integer(void **state)
{
    (void)state;

    /* OLD API: pass uintmax_t directly, no cast_* wrapper needed */
    expect_check(mock_test_a, value, custom_checker_old, 0);
    mock_test_a(0);
}

static void test_expect_check_old_api_integer_nonzero(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, custom_checker_old, 42);
    mock_test_a(42);
}

static void test_expect_check_old_api_count(void **state)
{
    (void)state;

    expect_check_count(mock_test_a, value, custom_checker_old, 100, 2);
    mock_test_a(100);
    mock_test_a(100);
}

static void test_expect_check_old_api_count_multiple_values(void **state)
{
    (void)state;

    expect_check_count(mock_test_a, value, custom_checker_old, 10, 3);
    mock_test_a(10);
    mock_test_a(10);
    mock_test_a(10);
}

/* Custom range checker using OLD API */
static int custom_range_checker_old(uintmax_t param, uintmax_t max)
{
    assert_in_range(param, 0, max);
    return 1;
}

static void test_expect_check_old_api_range(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, custom_range_checker_old, 100);
    mock_test_a(50);
}

static void test_expect_check_old_api_range_edge_min(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, custom_range_checker_old, 100);
    mock_test_a(0); /* minimum value */
}

static void test_expect_check_old_api_range_edge_max(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, custom_range_checker_old, 100);
    mock_test_a(100); /* maximum value */
}

/* Custom pointer equality checker using OLD API */
static int custom_ptr_checker_old(uintmax_t param, uintmax_t check)
{
    void *ptr_param = (void *)(uintptr_t)param;
    void *ptr_check = (void *)(uintptr_t)check;
    assert_ptr_equal(ptr_param, ptr_check);
    return 1;
}

static void test_expect_check_old_api_pointer(void **state)
{
    char buffer[10];
    (void)state;

    expect_check(mock_test_b,
                 ptr,
                 custom_ptr_checker_old,
                 cast_ptr_to_uintmax_type(buffer));
    mock_test_b(buffer);
}

static void test_expect_check_old_api_pointer_null(void **state)
{
    (void)state;

    expect_check(mock_test_b,
                 ptr,
                 custom_ptr_checker_old,
                 cast_ptr_to_uintmax_type(NULL));
    mock_test_b(NULL);
}

/* Custom not-equal checker */
static int custom_not_equal_checker_old(uintmax_t param, uintmax_t check)
{
    assert_int_not_equal(param, check);
    return 1;
}

static void test_expect_check_old_api_not_equal(void **state)
{
    (void)state;

    expect_check(mock_test_a, value, custom_not_equal_checker_old, 0);
    mock_test_a(42);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_check_old_api_integer),
        cmocka_unit_test(test_expect_check_old_api_integer_nonzero),
        cmocka_unit_test(test_expect_check_old_api_count),
        cmocka_unit_test(test_expect_check_old_api_count_multiple_values),
        cmocka_unit_test(test_expect_check_old_api_range),
        cmocka_unit_test(test_expect_check_old_api_range_edge_min),
        cmocka_unit_test(test_expect_check_old_api_range_edge_max),
        cmocka_unit_test(test_expect_check_old_api_pointer),
        cmocka_unit_test(test_expect_check_old_api_pointer_null),
        cmocka_unit_test(test_expect_check_old_api_not_equal),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
