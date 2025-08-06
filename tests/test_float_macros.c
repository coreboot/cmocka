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

#include <cmocka.h>
#include <cmocka_private.h>

/* A test case that does check if float is equal. */
static void float_test_success(void **state)
{
    (void)state; /* unused */

    assert_float_equal(0.5f, 1.f / 2.f, 0.000001f);
    assert_float_not_equal(0.5, 0.499f, 0.000001f);

    /* Check that finite numbers do not test equal to INFINITY or NAN */
    assert_float_not_equal(123.f, INFINITY, 0.0f);
    assert_float_not_equal(123.f, -INFINITY, 0.0f);
    assert_float_not_equal(123.f, NAN, 0.0f);
    assert_float_not_equal(123.f, -NAN, 0.0f);

    /* Check that INFINITY and NAN test equal to themselves */
    assert_float_equal(INFINITY, INFINITY, 0.0f);
    assert_float_equal(-INFINITY, -INFINITY, 0.0f);
    assert_float_equal(NAN, NAN, 0.0f);
    assert_float_equal(-NAN, -NAN, 0.0f);

    /* Check that NAN sign doesn't matter in equality test  */
    assert_float_equal(NAN, -NAN, 0.0f);

    /* Check that INFINITY sign does matter in equality test */
    assert_float_not_equal(INFINITY, -INFINITY, 0.0f);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(float_test_success),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
