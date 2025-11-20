/*
 * Advanced examples of expect_check_data() usage.
 *
 * This file demonstrates sophisticated patterns for complex testing scenarios:
 *
 * 1. FLOATING-POINT VALIDATION - Testing graphics APIs with epsilon tolerance
 * 2. STATEFUL CHECKERS - Validating call sequences and ordering
 * 3. PARTIAL STRUCT VALIDATION - Selectively checking struct fields
 * 4. COMPLEX RANGE CHECKING - Combining absolute and relative tolerances
 * 5. MULTIPLE INDEPENDENT CHECKERS - Different validators for different params
 *
 * These patterns go beyond basic parameter checking to enable testing of
 * complex, real-world APIs.
 */

#include <cmocka.h>
#include <string.h>
#include <math.h>

/*
 * ===========================================================================
 * PATTERN 1: Floating-Point Struct Validation with Epsilon
 * ===========================================================================
 *
 * Problem: Comparing floating-point values with == is unreliable due to
 * rounding errors. Graphics APIs often use floating-point coordinates.
 *
 * Solution: Use epsilon tolerance when comparing float/double values.
 */

/* Graphics API structure with floating-point coordinates */
typedef struct {
    float x;      /* X coordinate */
    float y;      /* Y coordinate */
    float width;  /* Rectangle width */
    float height; /* Rectangle height */
} Rectangle;

/* Mock graphics function */
void draw_rectangle(const Rectangle *rect, float rotation, const char *color);

void draw_rectangle(const Rectangle *rect, float rotation, const char *color)
{
    check_expected_ptr(rect);
    check_expected_float(rotation);
    check_expected_ptr(color);
}

/*
 * Checker configuration for rectangle validation
 *
 * This structure holds both the expected rectangle and the tolerance
 * (epsilon) to use when comparing floating-point values.
 */
typedef struct {
    Rectangle expected; /* Expected rectangle values */
    float epsilon;      /* Tolerance for floating-point comparison */
} RectangleChecker;

/*
 * Custom checker: validates rectangle with epsilon tolerance
 *
 * Floating-point comparison requires tolerance because:
 *   10.0f + 0.1f + 0.1f might not exactly equal 10.2f
 *
 * This checker compares each field with epsilon tolerance using fabsf().
 *
 * IMPORTANT: For floating-point types, actual.ptr points to the struct.
 * We can't use actual.float_val here because we're checking a pointer
 * to a Rectangle, not a single float.
 */
static int check_rectangle(CMockaValueData actual, CMockaValueData expected)
{
    RectangleChecker *checker = (RectangleChecker *)expected.ptr;
    Rectangle *actual_rect = (Rectangle *)actual.ptr;
    Rectangle *expected_rect = &checker->expected;
    float epsilon = checker->epsilon;

    /*
     * Compare each field with epsilon tolerance
     * fabsf() returns absolute value of the difference
     * If difference < epsilon, values are "equal enough"
     */
    return (fabsf(actual_rect->x - expected_rect->x) < epsilon &&
            fabsf(actual_rect->y - expected_rect->y) < epsilon &&
            fabsf(actual_rect->width - expected_rect->width) < epsilon &&
            fabsf(actual_rect->height - expected_rect->height) < epsilon);
}

/*
 * Test: Rectangle validation with epsilon
 *
 * This demonstrates validating a complex struct with floating-point fields.
 * Without epsilon tolerance, even small rounding errors would cause failures.
 */
static void test_rectangle_drawing(void **state)
{
    (void)state; /* unused */

    /* Define the expected rectangle */
    Rectangle default_rect = {.x = 10.0f,
                              .y = 20.0f,
                              .width = 100.0f,
                              .height = 50.0f};

    /*
     * Set up checker with 0.001f tolerance
     * This means values within 0.001 of expected are considered equal
     * E.g., 10.0001f would match 10.0f
     */
    RectangleChecker rect_checker = {.expected = default_rect,
                                     .epsilon = 0.001f};

    /* Use the epsilon-aware checker for the rectangle parameter */
    expect_check_data_count(draw_rectangle,
                            rect,
                            check_rectangle,
                            cast_ptr_to_cmocka_value(&rect_checker),
                            EXPECT_ALWAYS);

    /* Don't care about rotation or color for this test */
    expect_any_count(draw_rectangle, rotation, 3);
    expect_any_count(draw_rectangle, color, 3);

    /*
     * Make multiple calls with the same rectangle but different rotations
     * The rectangle checker validates all fields match within epsilon
     */
    draw_rectangle(&default_rect, 0.0f, "red");
    draw_rectangle(&default_rect, 45.0f, "blue");
    draw_rectangle(&default_rect, 90.0f, "green");
}

/*
 * ===========================================================================
 * PATTERN 2: Stateful Checker for Call Sequence Validation
 * ===========================================================================
 *
 * Problem: Sometimes you need to verify calls happen in a specific order
 * with specific values at each step (e.g., priority must increase).
 *
 * Solution: Maintain state in the checker struct to track call sequence.
 */

/*
 * Stateful sequence checker structure
 *
 * This maintains state across multiple checker invocations to validate
 * that function calls occur in a specific sequence.
 */
typedef struct {
    int call_count;            /* Current call number (incremented each call) */
    int max_calls;             /* Maximum expected calls (safety check) */
    int expected_sequence[10]; /* Array of expected values in order */
} SequenceChecker;

/*
 * Stateful checker: validates call sequence
 *
 * This checker maintains state (call_count) and validates that each
 * call provides the expected value for that position in the sequence.
 *
 * KEY INSIGHT: The checker struct persists across calls, so we can
 * increment call_count and use it to index into expected_sequence.
 *
 * Example: If expected_sequence = {1, 3, 5, 7, 9}, then:
 *   - First call must have priority = 1
 *   - Second call must have priority = 3
 *   - Third call must have priority = 5, etc.
 */
static int check_increasing_priority_sequence(CMockaValueData actual,
                                              CMockaValueData expected)
{
    SequenceChecker *checker = (SequenceChecker *)expected.ptr;
    int actual_priority = actual.int_val;

    /* Safety check: detect if more calls than expected */
    if (checker->call_count >= checker->max_calls) {
        return 0; /* Too many calls */
    }

    /* Get the expected value for this call position */
    int expected_priority = checker->expected_sequence[checker->call_count];

    /*
     * Increment call counter for next invocation
     * IMPORTANT: This modifies the checker state!
     */
    checker->call_count++;

    /* Validate the priority matches the expected sequence value */
    return (actual_priority == expected_priority);
}

/* Function to test sequence */
void process_task(int task_id, int priority);

void process_task(int task_id, int priority)
{
    check_expected_int(task_id);
    check_expected_int(priority);
}

/* Test: Validate call sequence */
static void test_priority_sequence(void **state)
{
    (void)state; /* unused */

    SequenceChecker seq_checker = {.call_count = 0,
                                   .max_calls = 5,
                                   .expected_sequence = {1, 3, 5, 7, 9}};

    expect_check_data_count(process_task,
                            priority,
                            check_increasing_priority_sequence,
                            cast_ptr_to_cmocka_value(&seq_checker),
                            EXPECT_ALWAYS);

    expect_any_count(process_task, task_id, 5);

    /* Priorities must be in the expected sequence */
    process_task(100, 1);
    process_task(101, 3);
    process_task(102, 5);
    process_task(103, 7);
    process_task(104, 9);

    /* Verify all expected calls were made */
    assert_int_equal(seq_checker.call_count, 5);
}

/* Example: Configuration struct validation */
typedef struct {
    int max_connections;
    int timeout_ms;
    const char *server_name;
    bool enable_ssl;
    int port;
} ServerConfig;

void configure_server(const ServerConfig *config);

void configure_server(const ServerConfig *config)
{
    check_expected_ptr(config);
}

typedef struct {
    ServerConfig expected;
    bool check_ssl;
    bool check_port;
} ConfigChecker;

static int check_server_config(CMockaValueData actual, CMockaValueData expected)
{
    ConfigChecker *checker = (ConfigChecker *)expected.ptr;
    const ServerConfig *actual_config = (const ServerConfig *)actual.ptr;
    const ServerConfig *expected_config = &checker->expected;

    /* Always check these fields */
    if (actual_config->max_connections != expected_config->max_connections) {
        return 0;
    }
    if (actual_config->timeout_ms != expected_config->timeout_ms) {
        return 0;
    }
    if (strcmp(actual_config->server_name, expected_config->server_name) != 0) {
        return 0;
    }

    /* Conditionally check these fields */
    if (checker->check_ssl) {
        if (actual_config->enable_ssl != expected_config->enable_ssl) {
            return 0;
        }
    }

    if (checker->check_port) {
        if (actual_config->port != expected_config->port) {
            return 0;
        }
    }

    return 1;
}

/* Test: Partial struct validation */
static void test_server_config(void **state)
{
    (void)state; /* unused */

    ServerConfig default_config = {.max_connections = 100,
                                   .timeout_ms = 5000,
                                   .server_name = "test-server",
                                   .enable_ssl = true,
                                   .port = 8080};

    ConfigChecker config_checker = {
        .expected = default_config,
        .check_ssl = true,
        .check_port = false /* Don't care about port for these tests */
    };

    expect_check_data_count(configure_server,
                            config,
                            check_server_config,
                            cast_ptr_to_cmocka_value(&config_checker),
                            EXPECT_ALWAYS);

    /* These should pass even with different ports */
    ServerConfig cfg1 = default_config;
    cfg1.port = 9090;
    configure_server(&cfg1);

    ServerConfig cfg2 = default_config;
    cfg2.port = 7070;
    configure_server(&cfg2);
}

/* Example: Multiple data types in one checker */
typedef struct {
    double expected_value;
    double tolerance_percent;
    double min_absolute;
    double max_absolute;
} DoubleRangeChecker;

static int check_double_in_range_or_near(CMockaValueData actual,
                                         CMockaValueData expected)
{
    DoubleRangeChecker *checker = (DoubleRangeChecker *)expected.ptr;
    double actual_val = actual.real_val;

    /* Check absolute range */
    if (actual_val < checker->min_absolute ||
        actual_val > checker->max_absolute)
    {
        return 0;
    }

    /* Check relative tolerance */
    double diff = fabs(actual_val - checker->expected_value);
    double tolerance = fabs(checker->expected_value *
                            checker->tolerance_percent / 100.0);

    return (diff <= tolerance);
}

void set_temperature(double celsius);

void set_temperature(double celsius)
{
    check_expected_double(celsius);
}

/* Test: Complex double validation */
static void test_temperature_range(void **state)
{
    (void)state; /* unused */

    /* Accept values near 20.0°C (±5%) but within -10 to 50 absolute range */
    DoubleRangeChecker temp_checker = {.expected_value = 20.0,
                                       .tolerance_percent = 5.0,
                                       .min_absolute = -10.0,
                                       .max_absolute = 50.0};

    expect_check_data_count(set_temperature,
                            celsius,
                            check_double_in_range_or_near,
                            cast_ptr_to_cmocka_value(&temp_checker),
                            EXPECT_ALWAYS);

    /* These should all pass */
    set_temperature(20.0); /* Exact match */
    set_temperature(19.0); /* Within 5% */
    set_temperature(21.0); /* Within 5% */
}

/*
 * ===========================================================================
 * PATTERN 5: Multiple Independent Checkers
 * ===========================================================================
 *
 * Problem: Different parameters need different validation logic.
 *
 * Solution: Use different checkers for different parameters, each with
 * their own validation rules and data.
 */

/* Checker for validating task IDs are in valid range */
typedef struct {
    int min_id;
    int max_id;
    const char *allowed_prefix; /* Not used in this example, but could be */
} IdChecker;

/*
 * ID range validator
 *
 * Simple range check for integer IDs.
 * Could be extended to check prefix, checksums, etc.
 */
static int check_valid_task_id(CMockaValueData actual, CMockaValueData expected)
{
    IdChecker *checker = (IdChecker *)expected.ptr;
    int actual_id = actual.int_val;

    /* ID must be in range */
    if (actual_id < checker->min_id || actual_id > checker->max_id) {
        return 0;
    }

    /* For this example, we'll just check the range */
    /* In a real scenario, you might validate ID format, checksums, etc. */
    return 1;
}

typedef struct {
    const char *allowed_prefixes[5];
    size_t num_prefixes;
} StringPrefixChecker;

static int check_string_prefix(CMockaValueData actual, CMockaValueData expected)
{
    StringPrefixChecker *checker = (StringPrefixChecker *)expected.ptr;
    const char *actual_str = (const char *)actual.ptr;

    for (size_t i = 0; i < checker->num_prefixes; i++) {
        const char *prefix = checker->allowed_prefixes[i];
        size_t prefix_len = strlen(prefix);

        if (strncmp(actual_str, prefix, prefix_len) == 0) {
            return 1;
        }
    }

    return 0;
}

void execute_command(int task_id, const char *command);

void execute_command(int task_id, const char *command)
{
    check_expected_int(task_id);
    check_expected_ptr(command);
}

/*
 * Test: Multiple independent checkers
 *
 * This demonstrates using completely different validation logic for
 * different parameters. The task_id uses range checking while the
 * command uses prefix matching - two separate checkers with different
 * data structures and logic.
 *
 * KEY INSIGHT: You can mix and match checkers freely. Each parameter
 * gets its own checker with its own validation rules.
 */
static void test_command_validation(void **state)
{
    (void)state; /* unused */

    /* Set up ID range checker (1000-9999) */
    IdChecker id_checker = {.min_id = 1000,
                            .max_id = 9999,
                            .allowed_prefix = "TASK"};

    /* Set up command prefix checker (must start with allowed prefix) */
    StringPrefixChecker cmd_checker = {
        .allowed_prefixes = {"GET_", "SET_", "DELETE_", "UPDATE_", NULL},
        .num_prefixes = 4};

    /*
     * Apply DIFFERENT checkers to DIFFERENT parameters
     * task_id: uses range validation
     * command: uses prefix validation
     */
    expect_check_data_count(execute_command,
                            task_id,
                            check_valid_task_id,
                            cast_ptr_to_cmocka_value(&id_checker),
                            EXPECT_ALWAYS);

    expect_check_data_count(execute_command,
                            command,
                            check_string_prefix,
                            cast_ptr_to_cmocka_value(&cmd_checker),
                            EXPECT_ALWAYS);

    /*
     * All these combinations should pass:
     * - IDs are all in [1000, 9999]
     * - Commands all start with allowed prefixes
     */
    execute_command(1000, "GET_STATUS");
    execute_command(5000, "SET_CONFIG");
    execute_command(9999, "DELETE_TEMP");
    execute_command(7777, "UPDATE_CACHE");
}

/*
 * ===========================================================================
 * SUMMARY OF ADVANCED PATTERNS
 * ===========================================================================
 *
 * This file demonstrated five sophisticated testing patterns:
 *
 * 1. FLOATING-POINT VALIDATION (test_rectangle_drawing)
 *    - Use epsilon tolerance for float/double comparison
 *    - Validate complex structs field-by-field
 *    - Essential for graphics, physics, scientific computing
 *
 * 2. STATEFUL SEQUENCE VALIDATION (test_priority_sequence)
 *    - Maintain state across checker calls
 *    - Validate call ordering and sequences
 *    - Useful for protocol testing, state machines
 *
 * 3. PARTIAL STRUCT VALIDATION (test_server_config)
 *    - Selectively validate only certain struct fields
 *    - Ignore fields that don't matter for the test
 *    - Reduces test brittleness
 *
 * 4. COMPLEX RANGE VALIDATION (test_temperature_range)
 *    - Combine absolute and relative tolerances
 *    - More sophisticated than simple min/max checking
 *    - Useful for testing with realistic constraints
 *
 * 5. MULTIPLE INDEPENDENT CHECKERS (test_command_validation)
 *    - Different parameters get different validators
 *    - Mix range checking, prefix checking, etc.
 *    - Maximum flexibility
 *
 * KEY TAKEAWAYS:
 * - Checkers can maintain state between calls
 * - Checkers can implement arbitrarily complex logic
 * - Different parameters can use completely different checkers
 * - Floating-point requires epsilon tolerance
 * - Checker structs must remain valid until all calls complete
 *
 * See expect_check_data_example.c for simpler patterns and basic usage.
 */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rectangle_drawing),
        cmocka_unit_test(test_priority_sequence),
        cmocka_unit_test(test_server_config),
        cmocka_unit_test(test_temperature_range),
        cmocka_unit_test(test_command_validation),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
