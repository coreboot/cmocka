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

if (NOT WIN32)
    add_subdirectory(mock)
endif()
