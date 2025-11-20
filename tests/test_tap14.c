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

#include <cmocka.h>
#include <cmocka_private.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <fcntl.h>

/* Test that succeeds */
static void test_success(void **state)
{
    (void)state; /* unused */

    assert_true(1);
}

/* Test that fails with an assertion */
static void test_failure(void **state)
{
    (void)state; /* unused */

    assert_int_equal(42, 24);
}

/* Test that errors with a setup failure */
static int setup_that_fails(void **state)
{
    (void)state; /* unused */

    return -1;
}

static void test_with_setup_error(void **state)
{
    (void)state; /* unused */

    /* This should not be reached due to setup failure */
    assert_true(1);
}

/* Test that is skipped */
static void test_skipped(void **state)
{
    (void)state; /* unused */

    skip();

    /* This should not be reached */
    assert_true(0);
}

/* Test that has a multi-line error message */
static void test_multiline_failure(void **state)
{
    int fd;

    (void)state; /* unused */

    fd = open("this_file_doesnt_exist.cmocka", 0);
    assert_return_code(fd, errno);

    if (fd >= 0) {
        close(fd);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_success),
        cmocka_unit_test(test_failure),
        cmocka_unit_test_setup(test_with_setup_error, setup_that_fails),
        cmocka_unit_test(test_skipped),
        cmocka_unit_test(test_multiline_failure),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);

    return cmocka_run_group_tests(tests, NULL, NULL);
}
