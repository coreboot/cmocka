project(cmocka-examples C)

set_source_files_properties(calculator.c
                            allocate_module.c
                            assert_module.c
                            PROPERTIES
                                COMPILE_DEFINITIONS
                                    UNIT_TESTING=1)


### The most simple test
add_cmocka_test(simple_test
                SOURCES simple_test.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(simple_test)

### Allocate module test
add_cmocka_test(allocate_module_test
                SOURCES allocate_module.c allocate_module_test.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(allocate_module_test)

set_tests_properties(allocate_module_test
                     PROPERTIES
                        WILL_FAIL 1)

### Assert macro test
add_cmocka_test(assert_macro_test
                SOURCES assert_macro.c assert_macro_test.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(assert_macro_test)

set_tests_properties(assert_macro_test
                     PROPERTIES
                        WILL_FAIL 1)

### Assert module test
add_cmocka_test(assert_module_test
                SOURCES assert_module.c assert_module_test.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(assert_module_test)

set_tests_properties(assert_module_test
                     PROPERTIES
                        WILL_FAIL 1)

### Set parameter mock tests
add_cmocka_test(set_parameters_test
                SOURCES set_parameters.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(set_parameters_test)

### Errno mock tests
add_cmocka_test(set_errno_tests
                SOURCES set_errno.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(set_errno_tests)

### Output override test
add_cmocka_test(output_override_test
                SOURCES output_override_test.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(output_override_test)

set(output_override_test_output
"---- UNIT TEST OUTPUT ----\
[==========] tests: Running 4 test(s).\
[ RUN      ] true_test_succeeds\
\.*\
[==========] tests: 4 test(s) run.\
[  PASSED  ] 3 test(s).\
[  FAILED  ] tests: 1 test(s), listed below:\
[  FAILED  ] false_test_fails\
\
 1 FAILED TEST(S)\
\
---- UNIT TEST OUTPUT ----")

# Note that in this example we are demonstrating a failing test, so we expect
# the test to fail
set_tests_properties(output_override_test
                     PROPERTIES
                        WILL_FAIL 1
                        PASS_REGULAR_EXPRESSION ${output_override_test_output}
)

### Expect check data example
add_cmocka_test(expect_check_data_example
                SOURCES expect_check_data_example.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(expect_check_data_example)

### Expect check data advanced example
add_cmocka_test(expect_check_data_advanced
                SOURCES expect_check_data_advanced.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS}
                LINK_LIBRARIES cmocka::cmocka)
add_cmocka_test_environment(expect_check_data_advanced)

if (NOT WIN32)
    add_subdirectory(mock)
endif()
