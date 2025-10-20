/*
 * Copyright 2019 Arnaud Gelas <arnaud.gelas@sensefly.com>
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
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <cmocka_private.h>

/* A test case that does check if double is equal. */
static void double_test_success(void **state)
{
    (void)state; /* unused */

    assert_double_equal(0.5, 1. / 2., 0.000001);
    assert_double_not_equal(0.5, 0.499, 0.000001);

    /* Check that finite numbers do not test equal to INFINITY or NAN */
    assert_double_not_equal(123., INFINITY, 0.0);
    assert_double_not_equal(123., -INFINITY, 0.0);
    assert_double_not_equal(123., NAN, 0.0);
    assert_double_not_equal(123., -NAN, 0.0);

    /* Check that INFINITY and NAN test equal to themselves */
    assert_double_equal(INFINITY, INFINITY, 0.0);
    assert_double_equal(-INFINITY, -INFINITY, 0.0);
    assert_double_equal(NAN, NAN, 0.0);
    assert_double_equal(-NAN, -NAN, 0.0);

    /* Check that NAN sign doesn't matter in equality test  */
    assert_double_equal(NAN, -NAN, 0.0);

    /* Check that INFINITY sign does matter in equality test */
    assert_double_not_equal(INFINITY, -INFINITY, 0.0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(double_test_success),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
