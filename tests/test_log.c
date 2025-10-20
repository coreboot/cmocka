/*
 * Copyright 2018 Andreas Schneider <asn@cryptomilk.org>
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

#include "../src/cmocka.c"

static void test_natural_log(void **state)
{
    (void)state;
    assert_true(double_compare(ln(1.0), 0.0, 0.0000000001f));
    assert_true(double_compare(ln(2.0), 0.6931471805, 0.0000000001f));
    /*printf("log(0.0000000001f)=%.10f\n", log(0.0000000001f));*/
    /*printf("ln(0.0000000001f)=%.10f\n", ln(0.0000000001f));*/
    assert_true(
        double_compare(ln(0.0000000001f), -23.0258509299, 0.4f));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_natural_log),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
