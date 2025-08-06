#include <stdio.h>
#include <cmocka.h>

static char g_buffer[1*1024*1024] = { 0 };
static size_t idx = 0;

static size_t g_my_msg_count = 0;
static size_t g_my_err_count = 0;

/** Append the message to a global buffers. */
static void my_vprint(const char * const format, va_list args)
{
    /* Note: a real implementation would need to deal with threading as well
     * as running out of buffer space. */
    size_t remaining = sizeof(g_buffer) - idx;
    idx += vsnprintf(g_buffer + idx, remaining, format, args);
    g_my_msg_count++;
}

/** Append the message to a global buffers. */
static void my_vprint_error(const char * const format, va_list args)
{
    /* Note: a real implementation would need to deal with threading as well
     * as running out of buffer space. */
    size_t remaining = sizeof(g_buffer) - idx;
    idx += vsnprintf(g_buffer + idx, remaining, format, args);
    g_my_err_count++;
}

/** Generate output from a test which succeeds. */
static void true_test_succeeds(void **state)
{
    (void)state;
    assert_true(true);
}

/** Ensure a passed test generated output. */
static void true_test_generated_output(void **state)
{
    (void) state; /* unused */
    assert_uint_not_equal(g_my_msg_count, 0);
}

/** Generate output from a test which fails. */
static void false_test_fails(void **state)
{
    (void)state;

    /* Fail a test - this should invoke 'my_vprint_error' */
    assert_true(false);
}

/* Ensure the failed test generated some output */
static void false_test_generated_output(void **state)
{
    (void)state;
    assert_uint_not_equal(g_my_err_count, 0);
}

int main(void)
{
    int rc = 0;
    struct CMCallbacks my_callbacks = {
        .vprint_message = my_vprint,
        .vprint_error = my_vprint_error,
    };

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(true_test_succeeds),
        cmocka_unit_test(true_test_generated_output),
        cmocka_unit_test(false_test_fails),
        cmocka_unit_test(false_test_generated_output),
    };

    /* Override the output functions before calling any CMocka APIs. */
    cmocka_set_callbacks(&my_callbacks);

    rc = cmocka_run_group_tests(tests, NULL, NULL);

    printf("---- UNIT TEST OUTPUT ----\n");
    puts(g_buffer);
    printf("---- UNIT TEST OUTPUT ----\n");

    return rc;
}
