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
 * Test that the NEW API properly detects failures.
 * These tests are expected to FAIL (marked with WILL_FAIL in CMakeLists.txt).
 */

static void mock_test_a(int value)
{
    check_expected_int(value);
}

static void mock_test_b(void *ptr)
{
    check_expected_ptr(ptr);
}

static void mock_test_c(float fval)
{
    check_expected_float(fval);
}

static void mock_test_d(double dval)
{
    CMockaValueData val = assign_double_to_cmocka_value(dval);
    _check_expected(__func__, "dval", __FILE__, __LINE__, val);
}

/* Custom checker for NEW API */
static int failing_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_uint_equal(param.uint_val, check.uint_val);
    return 1;
}

static void test_expect_check_data_fail_mismatch(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      failing_checker_data,
                      assign_uint_to_cmocka_value(0));
    /* This should fail - passing 1 when expecting 0 */
    mock_test_a(1);
}

static void test_expect_check_data_fail_missing_call(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      failing_checker_data,
                      assign_uint_to_cmocka_value(0));
    /* This should fail - expected call never happens */
}

static void test_expect_check_data_fail_extra_call(void **state)
{
    (void)state;

    expect_check_data_count(mock_test_a,
                            value,
                            failing_checker_data,
                            assign_uint_to_cmocka_value(0),
                            1);
    mock_test_a(0);
    /* This should fail - extra unexpected call */
    mock_test_a(0);
}

static void test_expect_check_data_fail_wrong_count(void **state)
{
    (void)state;

    expect_check_data_count(mock_test_a,
                            value,
                            failing_checker_data,
                            assign_uint_to_cmocka_value(42),
                            3);
    mock_test_a(42);
    mock_test_a(42);
    /* This should fail - only 2 calls made, expected 3 */
}

/* Float mismatch test */
static int float_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_float_equal(param.float_val, check.float_val, 0.001f);
    return 1;
}

static void test_expect_check_data_fail_float_mismatch(void **state)
{
    (void)state;

    expect_check_data(mock_test_c,
                      fval,
                      float_checker_data,
                      assign_float_to_cmocka_value(3.14f));
    /* This should fail - different float value */
    mock_test_c(2.71f);
}

/* Double mismatch test */
static int double_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_double_equal(param.real_val, check.real_val, 0.001);
    return 1;
}

static void test_expect_check_data_fail_double_mismatch(void **state)
{
    (void)state;

    expect_check_data(mock_test_d,
                      dval,
                      double_checker_data,
                      assign_double_to_cmocka_value(2.71828));
    /* This should fail - different double value */
    mock_test_d(1.41421);
}

/* Pointer mismatch test */
static int ptr_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_ptr_equal(param.ptr, check.ptr);
    return 1;
}

static void test_expect_check_data_fail_ptr_mismatch(void **state)
{
    char buffer1[10];
    char buffer2[10];
    (void)state;

    expect_check_data(mock_test_b,
                      ptr,
                      ptr_checker_data,
                      cast_ptr_to_cmocka_value(buffer1));
    /* This should fail - different pointer */
    mock_test_b(buffer2);
}

/* Signed integer mismatch test */
static int int_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_int_equal(param.int_val, check.int_val);
    return 1;
}

static void test_expect_check_data_fail_signed_int_mismatch(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      int_checker_data,
                      assign_int_to_cmocka_value(-42));
    /* This should fail - different signed value */
    mock_test_a(42);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_check_data_fail_mismatch),
        cmocka_unit_test(test_expect_check_data_fail_missing_call),
        cmocka_unit_test(test_expect_check_data_fail_extra_call),
        cmocka_unit_test(test_expect_check_data_fail_wrong_count),
        cmocka_unit_test(test_expect_check_data_fail_float_mismatch),
        cmocka_unit_test(test_expect_check_data_fail_double_mismatch),
        cmocka_unit_test(test_expect_check_data_fail_ptr_mismatch),
        cmocka_unit_test(test_expect_check_data_fail_signed_int_mismatch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
