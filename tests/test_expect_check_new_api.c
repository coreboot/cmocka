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
 * Test the NEW API for expect_check_data() using CMockaValueData parameters.
 * This tests the new type-safe API that supports int, uint, float, double, and
 * pointers.
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

/* Custom checker using NEW API signature (CMockaValueData) */
static int custom_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_uint_equal(param.uint_val, check.uint_val);
    return 1;
}

static void test_expect_check_data_integer(void **state)
{
    (void)state;

    /* NEW API: use expect_check_data() and assign_*_to_cmocka_value() */
    expect_check_data(mock_test_a,
                      value,
                      custom_checker_data,
                      assign_uint_to_cmocka_value(0));
    mock_test_a(0);
}

static void test_expect_check_data_integer_nonzero(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_checker_data,
                      assign_uint_to_cmocka_value(42));
    mock_test_a(42);
}

static void test_expect_check_data_count(void **state)
{
    (void)state;

    expect_check_data_count(mock_test_a,
                            value,
                            custom_checker_data,
                            assign_uint_to_cmocka_value(100),
                            2);
    mock_test_a(100);
    mock_test_a(100);
}

static void test_expect_check_data_count_multiple_values(void **state)
{
    (void)state;

    expect_check_data_count(mock_test_a,
                            value,
                            custom_checker_data,
                            assign_uint_to_cmocka_value(10),
                            3);
    mock_test_a(10);
    mock_test_a(10);
    mock_test_a(10);
}

/* Custom checker for signed integers */
static int custom_int_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_int_equal(param.int_val, check.int_val);
    return 1;
}

static void test_expect_check_data_signed_int(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_int_checker_data,
                      assign_int_to_cmocka_value(-42));
    mock_test_a(-42);
}

static void test_expect_check_data_signed_int_zero(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_int_checker_data,
                      assign_int_to_cmocka_value(0));
    mock_test_a(0);
}

/* Custom checker for float values */
static int custom_float_checker_data(CMockaValueData param,
                                     CMockaValueData check)
{
    assert_float_equal(param.float_val, check.float_val, 0.001f);
    return 1;
}

static void test_expect_check_data_float(void **state)
{
    (void)state;

    expect_check_data(mock_test_c,
                      fval,
                      custom_float_checker_data,
                      assign_float_to_cmocka_value(3.14f));
    mock_test_c(3.14f);
}

static void test_expect_check_data_float_negative(void **state)
{
    (void)state;

    expect_check_data(mock_test_c,
                      fval,
                      custom_float_checker_data,
                      assign_float_to_cmocka_value(-2.71f));
    mock_test_c(-2.71f);
}

/* Custom checker for double values */
static int custom_double_checker_data(CMockaValueData param,
                                      CMockaValueData check)
{
    assert_double_equal(param.real_val, check.real_val, 0.001);
    return 1;
}

static void test_expect_check_data_double(void **state)
{
    (void)state;

    expect_check_data(mock_test_d,
                      dval,
                      custom_double_checker_data,
                      assign_double_to_cmocka_value(2.71828));
    mock_test_d(2.71828);
}

static void test_expect_check_data_double_negative(void **state)
{
    (void)state;

    expect_check_data(mock_test_d,
                      dval,
                      custom_double_checker_data,
                      assign_double_to_cmocka_value(-1.41421));
    mock_test_d(-1.41421);
}

/* Custom checker for typed pointers */
static int custom_ptr_checker_data(CMockaValueData param, CMockaValueData check)
{
    assert_ptr_equal(param.ptr, check.ptr);
    return 1;
}

static void test_expect_check_data_pointer(void **state)
{
    char buffer[10];
    (void)state;

    expect_check_data(mock_test_b,
                      ptr,
                      custom_ptr_checker_data,
                      cast_ptr_to_cmocka_value(buffer));
    mock_test_b(buffer);
}

static void test_expect_check_data_pointer_null(void **state)
{
    (void)state;

    expect_check_data(mock_test_b,
                      ptr,
                      custom_ptr_checker_data,
                      cast_ptr_to_cmocka_value(NULL));
    mock_test_b(NULL);
}

/* Custom range checker using NEW API */
static int custom_range_checker_data(CMockaValueData param, CMockaValueData max)
{
    assert_uint_in_range(param.uint_val, 0, max.uint_val);
    return 1;
}

static void test_expect_check_data_range(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_range_checker_data,
                      assign_uint_to_cmocka_value(100));
    mock_test_a(50);
}

static void test_expect_check_data_range_edge_min(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_range_checker_data,
                      assign_uint_to_cmocka_value(100));
    mock_test_a(0); /* minimum value */
}

static void test_expect_check_data_range_edge_max(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_range_checker_data,
                      assign_uint_to_cmocka_value(100));
    mock_test_a(100); /* maximum value */
}

/* Custom not-equal checker */
static int custom_not_equal_checker_data(CMockaValueData param,
                                         CMockaValueData check)
{
    assert_uint_not_equal(param.uint_val, check.uint_val);
    return 1;
}

static void test_expect_check_data_not_equal(void **state)
{
    (void)state;

    expect_check_data(mock_test_a,
                      value,
                      custom_not_equal_checker_data,
                      assign_uint_to_cmocka_value(0));
    mock_test_a(42);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_expect_check_data_integer),
        cmocka_unit_test(test_expect_check_data_integer_nonzero),
        cmocka_unit_test(test_expect_check_data_count),
        cmocka_unit_test(test_expect_check_data_count_multiple_values),
        cmocka_unit_test(test_expect_check_data_signed_int),
        cmocka_unit_test(test_expect_check_data_signed_int_zero),
        cmocka_unit_test(test_expect_check_data_float),
        cmocka_unit_test(test_expect_check_data_float_negative),
        cmocka_unit_test(test_expect_check_data_double),
        cmocka_unit_test(test_expect_check_data_double_negative),
        cmocka_unit_test(test_expect_check_data_pointer),
        cmocka_unit_test(test_expect_check_data_pointer_null),
        cmocka_unit_test(test_expect_check_data_range),
        cmocka_unit_test(test_expect_check_data_range_edge_min),
        cmocka_unit_test(test_expect_check_data_range_edge_max),
        cmocka_unit_test(test_expect_check_data_not_equal),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
