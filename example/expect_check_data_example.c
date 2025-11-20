/*
 * Example demonstrating expect_check_data() for testing functions
 * with many parameters using default values.
 *
 * PROBLEM:
 * When testing functions with many parameters where most values remain constant
 * across multiple test calls, using standard expect_value() becomes repetitive:
 *
 *   expect_int_value(func, param1, 5);
 *   expect_string(func, param2, "admin");
 *   expect_string(func, param3, "read");
 *   // ... repeat for every parameter
 *   func(1, 5, "admin", "read", ...);
 *
 *   expect_int_value(func, param1, 5);  // Same values again!
 *   expect_string(func, param2, "admin");
 *   expect_string(func, param3, "read");
 *   func(2, 5, "admin", "read", ...);
 *
 * SOLUTION:
 * Use expect_check_data() with custom checker functions that validate against
 * default values. This allows you to:
 * 1. Define default expected values once
 * 2. Apply them to all function calls using EXPECT_ALWAYS
 * 3. Only set specific expectations for parameters that vary
 *
 * This example demonstrates four key patterns:
 * 1. Basic default parameter validation
 * 2. Range checking (min/max bounds)
 * 3. Whitelist validation (allowed values)
 * 4. Dynamic return values based on input parameters
 */

#include <cmocka.h>
#include <string.h>
#include <math.h>

/* Function under test - a typical API with many parameters */
int process_request(int request_id,
                    int priority,
                    const char *user,
                    const char *action,
                    int timeout,
                    int retries);

/*
 * Mock implementation
 *
 * In a real test, this would be the mock that replaces the actual
 * implementation. Each parameter is validated using check_expected_*() which
 * pulls values from the expectation queue that was set up via expect_*() calls.
 */
int process_request(int request_id,
                    int priority,
                    const char *user,
                    const char *action,
                    int timeout,
                    int retries)
{
    check_expected_int(request_id);
    check_expected_int(priority);
    check_expected_ptr(user);
    check_expected_ptr(action);
    check_expected_int(timeout);
    check_expected_int(retries);

    return mock_int();
}

/*
 * PATTERN 1: Default Parameter Validation
 *
 * This pattern uses a structure to hold default values for all parameters
 * that typically remain constant across test calls.
 */

/* Structure to hold default parameter values */
typedef struct {
    int priority;
    const char *user;
    const char *action;
    int timeout;
    int retries;
} RequestDefaults;

/*
 * Custom checker function for priority parameter
 *
 * PARAMETERS:
 *   actual   - The actual parameter value from the function call
 *              For integer types, access via actual.int_val
 *   expected - The check_data passed to expect_check_data()
 *              Here, it's a pointer to RequestDefaults struct
 *
 * RETURNS:
 *   1 (or any non-zero value) if the check passes
 *   0 if the check fails (will cause test failure)
 *
 * NOTE: This checker is called every time check_expected_int(priority) is
 * invoked in the mock function.
 */
static int check_priority_with_default(CMockaValueData actual,
                                       CMockaValueData expected)
{
    /* Extract the defaults structure from expected.ptr */
    RequestDefaults *defaults = (RequestDefaults *)expected.ptr;

    /* Extract the actual integer value */
    int actual_priority = actual.int_val;

    /* Compare against the default value */
    return (actual_priority == defaults->priority);
}

/*
 * Checker function for string parameter (user)
 *
 * For string/pointer types, access the actual value via actual.ptr
 * Cast it to the appropriate type (const char* for strings)
 */
static int check_user_with_default(CMockaValueData actual,
                                   CMockaValueData expected)
{
    RequestDefaults *defaults = (RequestDefaults *)expected.ptr;
    const char *actual_user = (const char *)actual.ptr;

    return (strcmp(actual_user, defaults->user) == 0);
}

/* Checker function for action parameter (similar to user) */
static int check_action_with_default(CMockaValueData actual,
                                     CMockaValueData expected)
{
    RequestDefaults *defaults = (RequestDefaults *)expected.ptr;
    const char *actual_action = (const char *)actual.ptr;

    return (strcmp(actual_action, defaults->action) == 0);
}

/* Checker function for timeout parameter (integer type) */
static int check_timeout_with_default(CMockaValueData actual,
                                      CMockaValueData expected)
{
    RequestDefaults *defaults = (RequestDefaults *)expected.ptr;
    int actual_timeout = actual.int_val;

    return (actual_timeout == defaults->timeout);
}

/* Checker function for retries parameter (integer type) */
static int check_retries_with_default(CMockaValueData actual,
                                      CMockaValueData expected)
{
    RequestDefaults *defaults = (RequestDefaults *)expected.ptr;
    int actual_retries = actual.int_val;

    return (actual_retries == defaults->retries);
}

/*
 * Test: Multiple calls with default parameters
 *
 * This demonstrates the key benefit of expect_check_data():
 * Instead of calling expect_*() for every parameter on every call,
 * we set up default checkers ONCE with EXPECT_ALWAYS, then only
 * specify expectations for parameters that vary (request_id).
 *
 * BEFORE (without expect_check_data): 18 expect_*() calls needed
 * AFTER (with expect_check_data): 5 setup calls + 3 for varying param = 8 total
 */
static void test_multiple_requests_with_defaults(void **state)
{
    (void)state; /* unused */

    /*
     * Step 1: Define default values in a structure
     * This structure must remain valid until all checkers have been called
     */
    RequestDefaults defaults = {.priority = 5,
                                .user = "admin",
                                .action = "read",
                                .timeout = 30,
                                .retries = 3};

    /*
     * Step 2: Set up custom checkers with EXPECT_ALWAYS
     *
     * expect_check_data_count() parameters:
     *   1. function name (process_request)
     *   2. parameter name (priority, user, etc.)
     *   3. checker function (check_priority_with_default, etc.)
     *   4. check_data (pointer to defaults struct, wrapped in CMockaValueData)
     *   5. count (EXPECT_ALWAYS = apply to all calls)
     *
     * EXPECT_ALWAYS means this checker will be used for EVERY call to
     * check_expected_int(priority) in the mock, regardless of how many calls
     */
    expect_check_data_count(process_request,
                            priority,
                            check_priority_with_default,
                            cast_ptr_to_cmocka_value(&defaults),
                            EXPECT_ALWAYS);

    expect_check_data_count(process_request,
                            user,
                            check_user_with_default,
                            cast_ptr_to_cmocka_value(&defaults),
                            EXPECT_ALWAYS);

    expect_check_data_count(process_request,
                            action,
                            check_action_with_default,
                            cast_ptr_to_cmocka_value(&defaults),
                            EXPECT_ALWAYS);

    expect_check_data_count(process_request,
                            timeout,
                            check_timeout_with_default,
                            cast_ptr_to_cmocka_value(&defaults),
                            EXPECT_ALWAYS);

    expect_check_data_count(process_request,
                            retries,
                            check_retries_with_default,
                            cast_ptr_to_cmocka_value(&defaults),
                            EXPECT_ALWAYS);

    /*
     * Step 3: Only set expectations for parameters that vary between calls
     * request_id is different for each call, so we use standard
     * expect_int_value()
     */
    expect_int_value(process_request, request_id, 1);
    expect_int_value(process_request, request_id, 2);
    expect_int_value(process_request, request_id, 3);

    /*
     * Step 4: Set up return values for each call
     * These are still required as normal
     */
    will_return_int(process_request, 0);
    will_return_int(process_request, 0);
    will_return_int(process_request, 0);

    /*
     * Step 5: Make the actual function calls
     * All parameters except request_id use the same values (our defaults)
     * The checkers validate them automatically
     */
    assert_int_equal(process_request(1, 5, "admin", "read", 30, 3), 0);
    assert_int_equal(process_request(2, 5, "admin", "read", 30, 3), 0);
    assert_int_equal(process_request(3, 5, "admin", "read", 30, 3), 0);
}

/*
 * PATTERN 2: Range Validation
 *
 * Instead of checking for exact values, validate that parameters
 * fall within acceptable ranges.
 *
 * USE CASE: Testing with boundary values, fuzzing, or when exact
 * values don't matter as long as they're within valid bounds.
 */

/* Structure to hold min/max range */
typedef struct {
    int min;
    int max;
} Range;

/*
 * Range checker: validates value is between min and max (inclusive)
 *
 * This is more flexible than exact-match checking and useful for:
 * - Boundary testing (min, max, mid values)
 * - Fuzzing with random inputs
 * - Testing invariants rather than exact values
 */
static int check_priority_in_range(CMockaValueData actual,
                                   CMockaValueData expected)
{
    Range *range = (Range *)expected.ptr;
    int actual_priority = actual.int_val;

    /* Check both bounds */
    return (actual_priority >= range->min && actual_priority <= range->max);
}

/*
 * Test: Using range checker
 *
 * This test validates that priority is within [1, 10] but doesn't
 * care about the exact value. All three calls use different priorities
 * (1, 5, 10) but all are valid because they're in range.
 */
static void test_priority_range(void **state)
{
    (void)state; /* unused */

    /* Define the acceptable range for priority */
    Range priority_range = {.min = 1, .max = 10};

    /* Use custom range checker for priority parameter */
    expect_check_data_count(process_request,
                            priority,
                            check_priority_in_range,
                            cast_ptr_to_cmocka_value(&priority_range),
                            EXPECT_ALWAYS);

    /*
     * Use expect_any_count() for other parameters we don't care about
     * The count (3) must match the number of function calls
     */
    expect_any_count(process_request, request_id, 3);
    expect_any_count(process_request, user, 3);
    expect_any_count(process_request, action, 3);
    expect_any_count(process_request, timeout, 3);
    expect_any_count(process_request, retries, 3);

    will_return_int(process_request, 0);
    will_return_int(process_request, 0);
    will_return_int(process_request, 0);

    /*
     * Test boundary values and mid-range
     * All should pass because priorities are in [1, 10]
     */
    process_request(1, 1, "user1", "read", 10, 1);    /* min priority */
    process_request(2, 5, "user2", "write", 20, 2);   /* mid priority */
    process_request(3, 10, "user3", "delete", 30, 3); /* max priority */
}

/*
 * PATTERN 3: Whitelist Validation
 *
 * Check that parameters match one of several allowed values.
 *
 * USE CASE: Authorization checks, enum validation, testing with
 * multiple valid inputs.
 */

/* Structure to hold a list of allowed values */
typedef struct {
    const char *const *allowed_users; /* Array of allowed strings */
    size_t num_users;                 /* Number of entries in array */
} UserWhitelist;

/*
 * Whitelist checker: validates value is in the allowed list
 *
 * This iterates through all allowed values and returns success
 * if any match. Useful for:
 * - Testing authorization/access control
 * - Validating enum-like string values
 * - Checking against multiple valid options
 */
static int check_user_in_whitelist(CMockaValueData actual,
                                   CMockaValueData expected)
{
    UserWhitelist *whitelist = (UserWhitelist *)expected.ptr;
    const char *actual_user = (const char *)actual.ptr;

    /* Check if actual user matches any allowed user */
    for (size_t i = 0; i < whitelist->num_users; i++) {
        if (strcmp(actual_user, whitelist->allowed_users[i]) == 0) {
            return 1; /* User is authorized */
        }
    }

    return 0; /* User not in whitelist */
}

/*
 * Test: User whitelist validation
 *
 * This validates that only specific users can make requests.
 * The test would fail if we called process_request() with a user
 * not in the whitelist (e.g., "hacker").
 */
static void test_user_whitelist(void **state)
{
    (void)state; /* unused */

    /* Define the list of authorized users */
    const char *allowed_users[] = {"admin", "operator", "viewer"};
    UserWhitelist whitelist = {.allowed_users = allowed_users, .num_users = 3};

    /* Set up the whitelist checker for the user parameter */
    expect_check_data_count(process_request,
                            user,
                            check_user_in_whitelist,
                            cast_ptr_to_cmocka_value(&whitelist),
                            EXPECT_ALWAYS);

    /* Don't care about other parameters */
    expect_any_count(process_request, request_id, 3);
    expect_any_count(process_request, priority, 3);
    expect_any_count(process_request, action, 3);
    expect_any_count(process_request, timeout, 3);
    expect_any_count(process_request, retries, 3);

    will_return_int(process_request, 0);
    will_return_int(process_request, 0);
    will_return_int(process_request, 0);

    /*
     * Test with different users from the whitelist
     * All should pass because they're authorized
     * If we used "hacker" instead, the test would fail
     */
    process_request(1, 5, "admin", "read", 30, 3);
    process_request(2, 5, "operator", "write", 30, 3);
    process_request(3, 5, "viewer", "read", 30, 3);
}

/*
 * PATTERN 4: Dynamic Return Values
 *
 * Checker functions can do more than just validate - they can also
 * set up return values dynamically based on input parameters.
 *
 * USE CASE: Simulating realistic behavior where return values depend
 * on inputs (e.g., priority queue, conditional success/failure).
 */

/*
 * Advanced checker: validates AND sets dynamic return values
 *
 * This demonstrates that checkers aren't limited to validation.
 * They can inspect parameters and set up mock behavior accordingly.
 *
 * In this example:
 * - High priority (>=10) requests succeed immediately (return 0)
 * - Medium priority (5-9) requests are delayed (return 1)
 * - Low priority (<5) requests are rejected (return -1)
 */
static int check_priority_and_set_return(CMockaValueData actual,
                                         CMockaValueData expected)
{
    (void)expected; /* unused - we don't need expected data for this */
    int actual_priority = actual.int_val;

    /*
     * Dynamically queue return values based on priority
     * This simulates a priority-based queueing system where
     * higher priority requests get better treatment
     */
    if (actual_priority >= 10) {
        will_return_int(process_request,
                        0); /* High priority: immediate success */
    } else if (actual_priority >= 5) {
        will_return_int(process_request, 1); /* Medium priority: delayed */
    } else {
        will_return_int(process_request, -1); /* Low priority: rejected */
    }

    /*
     * Always return 1 (pass) - we accept all priority values
     * The validation here is about setting up the right return value,
     * not rejecting certain priorities
     */
    return 1;
}

/*
 * Test: Dynamic return values
 *
 * This test demonstrates how return values can be set dynamically
 * based on input parameters. We don't pre-queue return values with
 * will_return_int() before the calls - instead, the checker sets
 * them up on-the-fly based on the priority value.
 */
static void test_dynamic_returns(void **state)
{
    (void)state; /* unused */

    /* Set up the dynamic checker for priority */
    expect_check_data_count(process_request,
                            priority,
                            check_priority_and_set_return,
                            cast_ptr_to_cmocka_value(NULL),
                            EXPECT_ALWAYS);

    /* Don't care about other parameters */
    expect_any_count(process_request, request_id, 3);
    expect_any_count(process_request, user, 3);
    expect_any_count(process_request, action, 3);
    expect_any_count(process_request, timeout, 3);
    expect_any_count(process_request, retries, 3);

    /*
     * NOTE: No will_return_int() calls here!
     * The checker sets them up dynamically based on priority.
     *
     * Each call gets a different return value based on its priority:
     * - Priority 15 (high) -> returns 0 (success)
     * - Priority 7 (medium) -> returns 1 (delayed)
     * - Priority 2 (low) -> returns -1 (rejected)
     */
    assert_int_equal(process_request(1, 15, "admin", "read", 30, 3),
                     0); /* High */
    assert_int_equal(process_request(2, 7, "admin", "read", 30, 3),
                     1); /* Medium */
    assert_int_equal(process_request(3, 2, "admin", "read", 30, 3),
                     -1); /* Low */
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multiple_requests_with_defaults),
        cmocka_unit_test(test_priority_range),
        cmocka_unit_test(test_user_whitelist),
        cmocka_unit_test(test_dynamic_returns),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
