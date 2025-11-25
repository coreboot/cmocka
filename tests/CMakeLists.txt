project(tests C)

set(TEST_EXCEPTION_HANDLER FALSE)
if (HAVE_SIGNAL_H)
    set(TEST_EXCEPTION_HANDLER TRUE)
endif()
if (WIN32)
    # FIXME: The exception handler doesn't work on Windows
    set(TEST_EXCEPTION_HANDLER FALSE)
endif()
if (CMAKE_BUILD_TYPE)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
    if (CMAKE_BUILD_TYPE_LOWER STREQUAL "undefinedsanitizer")
        set(TEST_EXCEPTION_HANDLER FALSE)
    endif()
endif()

set(CMOCKA_TESTS
    test_alloc
    test_buffer
    test_expect_check
    test_expect_check_old_api
    test_expect_check_old_api_fail
    test_expect_check_new_api
    test_expect_check_new_api_fail
    test_expect_u_int_in_set
    test_expect_u_int_not_in_set
    test_expect_value
    test_expect_check_fail
    test_expect_in_range
    test_group_setup_assert
    test_group_setup_fail
    test_fixtures
    test_group_fixtures
    test_groups
    test_float_macros
    test_double_macros
    test_assert_double_float
    test_assert_double_float_fail
    test_assert_true
    test_assert_true_fail
    test_assert_false
    test_assert_false_fail
    test_assert_macros
    test_assert_macros_fail
    test_assert_memory
    test_assert_memory_fail
    test_assert_ptr
    test_assert_ptr_fail
    test_assert_ptr_msg
    test_assert_ptr_msg_fail
    test_assert_funcptr
    test_assert_u_int
    test_assert_u_int_fail
    test_assert_range
    test_assert_range_fail
    test_assert_set
    test_assert_set_fail
    test_basics
    test_has_mock
    test_log
    test_skip
    test_stop
    test_stop_fail
    test_strmatch
    test_strreplace
    test_setup_fail
    test_ordering
    test_ordering_fail
    test_returns
    test_returns_fail
    test_set_parameter
    test_set_parameter_fail
    test_set_errno
    test_set_errno_fail
    test_string
    test_wildcard
    test_skip_filter
    test_skip_filter_env
    test_mock_exit
    )

if (TEST_EXCEPTION_HANDLER)
    list(APPEND CMOCKA_TESTS test_exception_handler)
endif()

foreach(_CMOCKA_TEST ${CMOCKA_TESTS})
    add_cmocka_test(${_CMOCKA_TEST}
                    SOURCES ${_CMOCKA_TEST}.c
                    COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS} -D_GNU_SOURCE -D_XOPEN_SOURCE=700
                    LINK_LIBRARIES cmocka::static
                    LINK_OPTIONS ${DEFAULT_LINK_FLAGS})
    target_include_directories(${_CMOCKA_TEST} PRIVATE ${cmocka_BINARY_DIR})

    add_cmocka_test_environment(${_CMOCKA_TEST})
endforeach()

### Environment

# test_skip_filter_env
set_property(TEST test_skip_filter_env
    PROPERTY ENVIRONMENT "CMOCKA_TEST_FILTER=test_skip*;CMOCKA_SKIP_FILTER=test_skip2")

### Exceptions

# test_skip
set_tests_properties(
    test_skip
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  SKIPPED \\] test_check_skip"
)

# test_assert_true
set_tests_properties(
    test_assert_true
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  PASSED  \\] 1 test\\(s\\)."
)

# test_assert_true_fail
set_tests_properties(
    test_assert_true_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "x != 10 is not true"
)

# test_assert_false
set_tests_properties(
    test_assert_false
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  PASSED  \\] 1 test\\(s\\)."
)

# test_assert_false_fail
set_tests_properties(
    test_assert_false_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "x == 10 is not false"
)

# test_assert_macros_fail
set_tests_properties(
    test_assert_macros_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] tests: 1 test"
)

# test_assert_macros_fail
set_tests_properties(
    test_assert_double_float_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] float_tests_fail: 4 test"
)

set_tests_properties(
    test_assert_memory_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] memory_tests: 1 test"
)

set_tests_properties(
    test_assert_ptr_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] ptr_tests: 4 test"
)

set_tests_properties(
    test_assert_ptr_msg_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] ptr_msg_tests: 4 test"
)

set_tests_properties(
    test_assert_u_int_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] integer_tests: 4 test"
)

set_tests_properties(
    test_assert_range_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] range_fail_tests: 12 test"
)

set_tests_properties(
    test_assert_set_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] set_fail_tests: 7 test"
)

# test_expect_check_fail
set_tests_properties(
    test_expect_check_fail
    PROPERTIES
    PASS_REGULAR_EXPRESSION
    "\\[  FAILED  \\] tests: 3 test"
)

# test_expect_check_old_api_fail
set_tests_properties(
    test_expect_check_old_api_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] tests: 5 test"
)

# test_expect_check_new_api_fail
set_tests_properties(
    test_expect_check_new_api_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] tests: 8 test"
)


# stop_fail
set_tests_properties(
    test_stop_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] tests: 3 test"
)

# test_ordering_fail ensure proper failures
set_tests_properties(
    test_ordering_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] tests: 8 test"
        FAIL_REGULAR_EXPRESSION
        "Test failed with exception: (Segmentation fault|Segmentation Fault|11|Illegal instruction)"
)

# test_returns_fail ensure proper failures
set_tests_properties(
    test_returns_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] will_return_mock_tests: 6 test"
)
# test_named_returns_fail ensure proper failures
set_tests_properties(
    test_set_parameter_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] will_set_parameter_mock_tests: 6 test"
)
# test_set_errno_fail ensure proper failures
set_tests_properties(
    test_set_errno_fail
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  FAILED  \\] will_set_errno_tests: 3 test"
)
# test_exception_handler
if (TEST_EXCEPTION_HANDLER)
    set_tests_properties(test_exception_handler
                         PROPERTIES
                             PASS_REGULAR_EXPRESSION
                             "Test failed with exception")
endif (TEST_EXCEPTION_HANDLER)

# test_mock_exit - verify error message is printed when mock() called outside test
set_tests_properties(
    test_mock_exit
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "Could not get value to mock function"
)

set_tests_properties(
    test_setup_fail
        PROPERTIES
        WILL_FAIL
        1
)

set_tests_properties(
    test_group_setup_assert
        PROPERTIES
        WILL_FAIL
        1
)

set_tests_properties(
    test_group_setup_fail
        PROPERTIES
        WILL_FAIL
        1
)

add_test(NAME test_setup_fail_1_failed COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:test_setup_fail>)
add_cmocka_test_environment(test_setup_fail_1_failed)
set_tests_properties(
    test_setup_fail_1_failed
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  ERROR   \\] int_test_ignored"
)

add_test(NAME test_setup_fail_1_passed COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:test_setup_fail>)
add_cmocka_test_environment(test_setup_fail_1_passed)
set_tests_properties(
    test_setup_fail_1_passed
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  PASSED  \\] 1 test\\(s\\)."
)

add_test(NAME test_setup_fail_match_failed COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:test_setup_fail>)
add_cmocka_test_environment(test_setup_fail_match_failed)
set_tests_properties(
    test_setup_fail_match_failed
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[  ERROR   \\] int_test_ignored"
)

add_test(NAME test_setup_fail_match_passed COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:test_setup_fail>)
add_cmocka_test_environment(test_setup_fail_match_passed)
set_tests_properties(
    test_setup_fail_match_passed
        PROPERTIES
        PASS_REGULAR_EXPRESSION
        "\\[       OK \\] int_test_success"
)

### Output formats

# test output of success, failure, skip, fixture failure
set(OUTPUT_TESTS
    test_basics
    test_assert_macros_fail
    test_groups
    test_skip
    test_setup_fail)

set(TEST_OUTPUT_FMTS
    subunit
    xml)

set(test_basics_subunit_out
    "^test: null_test_success"
    "success: null_test_success")
set(test_assert_macros_fail_subunit_out
    "failure: test_assert_return_code_fail \\[")
set(test_groups_subunit_out
    "^test: null_test_success"
    "success: null_test_success")
set(test_skip_subunit_out
    "^test: test_check_skip"
    "skip: test_check_skip")
set(test_setup_fail_subunit_out
    "error: int_test_ignored \\[ Could not run test: Test setup failed \\]")

set(test_basics_xml_out
    "<testsuite name=\"tests\" time=\"[0-9.]+\" tests=\"2\" failures=\"0\" errors=\"0\" skipped=\"0\" >"
    "<testcase name=\"null_test_success\" time=\"[0-9.]+\" >.*</testcase>")
set(test_assert_macros_fail_xml_out
    "<testcase name=\"test_assert_return_code_fail\" time=\"[0-9.]+\" >"
    "<failure>")
set(test_groups_xml_out
    "^<\\?xml version=\"1.0\" encoding=\"UTF-8\" \\?>"
    "<testsuites>"
    "<testsuite name=\"test_group1\" time=\"[0-9.]+\" tests=\"1\" failures=\"0\" errors=\"0\" skipped=\"0\" >"
    "<testcase name=\"null_test_success\" time=\"[0-9.]+\" >"
    "</testcase>"
    "</testsuite>"
    ".*<testsuite name=\"test_group2 num=&lt[;]&apos[;]&quot[;]2&amp[;]2&apos[;]&quot[;]&gt[;]\" time=\"[0-9.]+\" tests=\"1\" failures=\"0\" errors=\"0\" skipped=\"0\" >"
    "<testcase name=\"int_test_success\" time=\"[0-9.]+\" >"
    "</testcase>"
    "</testsuite>"
    "</testsuites>")
set(test_skip_xml_out
    "<testcase name=\"test_check_skip\" time=\"[0-9.]+\" >"
    "<skipped/>")
set(test_setup_fail_xml_out
    "<testcase name=\"int_test_ignored\" time=\"[0-9.]+\" >"
    "<failure><!\\[CDATA\\[Test setup failed\\]\\]></failure>")

foreach(_TEST_OUTPUT_FMT ${TEST_OUTPUT_FMTS})
    foreach(_OUTPUT_TEST ${OUTPUT_TESTS})
        set(TEST_NAME ${_OUTPUT_TEST}_${_TEST_OUTPUT_FMT})
        add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${_OUTPUT_TEST}>)
        add_cmocka_test_environment(${TEST_NAME})

        set_property(
            TEST
            ${TEST_NAME}
            APPEND
            PROPERTY
                ENVIRONMENT CMOCKA_MESSAGE_OUTPUT=${_TEST_OUTPUT_FMT}
            )

        list(LENGTH ${TEST_NAME}_out len)
        list(GET ${TEST_NAME}_out 0 output)
        if(len GREATER 1)
            list(REMOVE_AT ${TEST_NAME}_out 0)
            foreach(line ${${TEST_NAME}_out})
                set(output "${output}[ \n\r]+${line}")
            endforeach()
        endif()

        set_tests_properties(
            ${TEST_NAME}
            PROPERTIES
            PASS_REGULAR_EXPRESSION
            ${output}
        )
    endforeach()
endforeach()

### TAP 14 test

add_cmocka_test(test_tap14
                SOURCES test_tap14.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS} -D_GNU_SOURCE -D_XOPEN_SOURCE=700
                LINK_LIBRARIES cmocka::static
                LINK_OPTIONS ${DEFAULT_LINK_FLAGS})
target_include_directories(test_tap14 PRIVATE ${cmocka_BINARY_DIR})

set_tests_properties(
    test_tap14
    PROPERTIES
    PASS_REGULAR_EXPRESSION
    "^TAP version 14[ \n\r]+1\\.\\.5[ \n\r]+ok 1 - test_success[ \n\r]+not ok 2 - test_failure[ \n\r]+  ---[ \n\r]+  message: \\|-[ \n\r]+.*[ \n\r]+  severity: fail[ \n\r]+  \\.\\.\\.[ \n\r]+not ok 3 - test_with_setup_error[ \n\r]+  ---[ \n\r]+  message: '.*'[ \n\r]+  severity: error[ \n\r]+  \\.\\.\\.[ \n\r]+ok 4 - test_skipped # SKIP[ \n\r]+not ok 5 - test_multiline_failure"
)
