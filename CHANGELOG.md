# CHANGELOG

All notable changes to cmocka since version 0.1 are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/).

## [Unreleased]

### Added

#### Test Filtering
- Environment variable support for test filtering:
  - `CMOCKA_TEST_FILTER`: Filter tests to run by pattern
  - `CMOCKA_SKIP_FILTER`: Filter tests to skip by pattern
- Improved `cmocka_set_test_filter()` and `cmocka_set_skip_filter()` functions

#### Output Formats
- **TAP 14 support**: Updated Test Anything Protocol output to
  version 14 with YAML diagnostics
  - YAML-formatted error messages with proper indentation
  - `severity` field for distinguishing failures from errors
  - Improved SKIP directive format

#### Type-Safe Assertions and Mocking
- Type-safe assertion macros for integers:
  - `assert_int_equal()` / `assert_int_not_equal()`
  - `assert_uint_equal()` / `assert_uint_not_equal()`
  - `assert_int_in_range()` / `assert_int_not_in_range()`
  - `assert_uint_in_range()` / `assert_uint_not_in_range()`
  - `assert_int_in_set()` / `assert_int_not_in_set()`
  - `assert_uint_in_set()` / `assert_uint_not_in_set()`

- Type-safe mocking macros for different types:
  - `will_return_int()`, `will_return_uint()`,
    `will_return_float()`, `will_return_double()`,
    `will_return_ptr()`
  - `will_return_*_count()`, `will_return_*_always()`,
    `will_return_*_maybe()` variants
  - `will_set_parameter_int()`, `will_set_parameter_uint()`,
    `will_set_parameter_float()`, `will_set_parameter_double()`,
    `will_set_parameter_ptr()`
  - `will_set_parameter_*_count()`,
    `will_set_parameter_*_always()`,
    `will_set_parameter_*_maybe()` variants
  - `mock_int()`, `mock_uint()`, `mock_float()`, `mock_double()`,
    `mock_ptr()`

- Type-safe expect macros:
  - `expect_int_value()` / `expect_uint_value()`
  - `expect_int_not_value()` / `expect_uint_not_value()`
  - `expect_int_in_range()` / `expect_uint_in_range()`
  - `expect_int_not_in_range()` / `expect_uint_not_in_range()`
  - `expect_int_in_set()` / `expect_uint_in_set()`
  - `expect_int_not_in_set()` / `expect_uint_not_in_set()`
  - `expect_check_int()` / `expect_check_uint()`

#### Float and Double Support
- Floating-point assertion macros:
  - `assert_float_equal()` / `assert_float_not_equal()`
  - `assert_double_equal()` / `assert_double_not_equal()` with
    epsilon parameter
  - `assert_float_in_range()` / `assert_float_not_in_range()`
  - `assert_float_in_set()` / `assert_float_not_in_set()`

- Floating-point expect macros:
  - `expect_float()` / `expect_not_float()`
  - `expect_double()` / `expect_not_double()`
  - `expect_float_in_range()` / `expect_float_not_in_range()`
  - `expect_float_in_set()` / `expect_float_not_in_set()`

- `mock_double()` / `mock_parameter_double()` for mocking double
  values

#### Assertion Enhancements
- Pointer assertion macros with custom error messages:
  - `assert_ptr_equal_msg()` / `assert_ptr_not_equal_msg()`
  - `assert_null_msg()` / `assert_non_null_msg()`

- `assert_true()` and `assert_false()` now provide more verbose
  error messages

- Memory comparison improvements with better error display

#### Testing Features
- `has_mock()`: Check if a mock value is available before calling
  `mock()`
- `stop()`: New feature to stop test execution while allowing
  remaining tests to run
- `expect_check_data()`: New API for parameter validation with
  custom data (replaces deprecated `expect_check()`)
- `expect_check_data_any()`: Variant for checking pointer/struct
  parameters
- Errno mocking convenience macros: `will_set_errno()`,
  `will_set_errno_always()`

#### Output Customization
- Multiple simultaneous output formats support (can combine
  STANDARD, SUBUNIT, TAP, XML)
- Output function override capability via `cmocka_set_callbacks()`
- `cmocka_print_error()`: Public API for printing errors using
  configured output format

#### Documentation and Examples
- Improved API documentation with better examples
- Added `expect_check_data()` examples
- Added errno mocking examples
- Added parameter setting examples
- Better mock object examples

#### Build System and Compatibility
- CMake minimum required version bumped to 3.13
- CMake namespace support (`cmocka::cmocka`, `cmocka::static`)
- Meson build system support
- `extern "C"` wrapper for C++ compatibility
- Version header (`cmocka_version.h`)

### Changed

#### API Improvements
- Changed internal value handling to use `CMockaValueData` union
  for better type safety
- `_mock()` now returns `CMockaValueData` instead of raw values
- Improved error messages throughout the library
- Error messages now print to stdout instead of stderr
- Better display of memory comparison errors
- Enhanced string replacement implementation

#### Code Quality
- Use `bool` instead of `int` for boolean values throughout
- Use `stdint.h` types (`intmax_t`, `uintmax_t`) instead of
  custom types
- Reduced call stack consumption in printf functions
- Added compiler attributes to non-returning functions
- Use `__builtin_align_down` when available
- Better alignment handling (MALLOC_ALIGNMENT set to 16)

#### Build Configuration
- Require C99 standard
- C extensions enabled by default
- Windows DLL export improvements with `CMOCKA_DLLEXTERN`
- `WINDOWS_EXPORT_ALL_SYMBOLS` support
- Address sanitizer support for MSVC
- Stack protector flags always passed to linker

#### Documentation Improvements
- Enhanced API documentation with better organization
- Cleaned up internal functions from public documentation
- Better grouping of macros and functions
- Improved page layout and styling
- Updated README with more details

#### Examples
- Modernized and rebuilt calculator example
- Added new assert_macro examples demonstrating common assertions
- Removed deprecated allocate module example (use AddressSanitizer instead)

### Deprecated

- `expect_check()`: Use `expect_check_data()` instead
- `check_expected()`: Use `check_expected_int()` or
  `check_expected_uint()` instead
- `assert_in_range()`: Use `assert_int_in_range()` or
  `assert_uint_in_range()` instead
- `assert_not_in_range()`: Use `assert_int_not_in_range()` or
  `assert_uint_not_in_range()` instead
- `assert_in_set()`: Use `assert_int_in_set()` or
  `assert_uint_in_set()` instead
- `expect_in_range()`: Use `expect_int_in_range()` or
  `expect_uint_in_range()` instead
- `expect_not_in_range()`: Use `expect_int_not_in_range()` or
  `expect_uint_not_in_range()` instead
- `expect_value()`: Use `expect_int_value()` or
  `expect_uint_value()` instead
- `expect_not_value()`: Use `expect_int_not_value()` or
  `expect_uint_not_value()` instead
- `will_return()`: Use type-specific variants
  (`will_return_int()`, etc.) instead
- `will_return_count()`: Use type-specific variants instead
- `will_return_always()`: Use type-specific variants instead
- `will_set_parameter()`: Use type-specific variants instead
- Old MSVC support (pre-2008) dropped
- Cmockery legacy support removed

### Fixed

- Fixed missing `cmocka_version.h` in documentation
- Fixed TAP output for skipped tests
- Fixed floating-point comparison for `INFINITY` and `NAN`
- Fixed `expect_not_float()` implementation
- Fixed `mock_float()` implementation
- Fixed `assert_double_not_equal()` float handling
- Fixed `expect_function_calls()` for counts of 0
- Fixed NULL pointer dereference in `_function_called()`
- Fixed pointer assertions with function pointers
- Fixed segmentation fault handling in tests
- Fixed setjmp in `expect_assert_failure` macro
- Fixed unexpanded `%s` in leftover values error messages
- Fixed XML output duration formatting
- Fixed XML string sanitization (escape slashes and special
  characters)
- Fixed Windows x64 builds
- Fixed Windows UWP build errors (C4703)
- Fixed MinGW pkgconfig path relocation
- Fixed test calloc overflow checking
- Fixed symbol map freeing to avoid undefined behavior
- Fixed memory equal display implementation
- Fixed comparison between pointer and integer issues
- Fixed possible data loss on MSVC
- Fixed conversion warnings from `time_t` to `double`

### Infrastructure

- Added GitLab CI pipeline with multiple platforms:
  - Linux (Ubuntu, Fedora, Debian)
  - Windows (MinGW, MSVC)
  - FreeBSD
  - Code coverage analysis
  - Static analysis (Coverity, cppcheck)
  - Address and UB sanitizers
- Added SBOM template in CycloneDX format
- Added clang-format configuration
- Added .editorconfig
- Added CONTRIBUTING.md
- Added Coverity modeling files

## [1.1.8] - 2025-07-17

### Added
- Set CMOCKA_LIBRARIES in package config for backwards
  compatibility

### Changed
- Require cmake >= 3.10

### Fixed
- Improved c_strreplace implementation
- Sanitize XML strings
- Update check for uintptr_t

## [1.1.7] - 2023-02-23

### Fixed
- Update ignore list for source tarball generation

## [1.1.6] - 2023-02-16

### Added
- New assert macros to compare 2 doubles given an epsilon
- Meson build system
- Header with version to TAP13 output
- CMake generated configs for find_package(cmocka)

### Fixed
- Issues with MSVC
- TAP output for skipped tests
- Issue with fail_msg
- Documentation improvements

## [1.1.5] - 2019-03-28

### Added
- `cmocka_set_skip_filter()`: Filter tests to skip by pattern

## [1.1.4] - 2019-03-28

### Added
- `assert_float_equal()` / `assert_float_not_equal()`
- `expect_any_always()`

### Fixed
- Small bug fixes

## [1.1.3] - 2018-09-26

### Fixed
- Subunit output on failures
- Do not abort if a test is skipped
- Switched to Modern CMake

## [1.1.2] - 2018-08-29

### Added
- Function to filter tests (`cmocka_set_test_filter()`)
- New mocking example (uptime)

### Fixed
- Fixture error reporting
- Compiler flags detection
- Some improvement for API documentation

## [1.1.1] - 2016-04-07

### Fixed
- TAP output
- cmocka on Windows x64
- xUnit output durations

## [1.1.0] - 2016-09-21

### Added
- Support to catch multiple exceptions
- Support to verify call ordering
- Support to pass initial data to test cases
- `will_return_maybe()` for ignoring mock returns
- Subtests for groups using TAP output
- Support to write multiple XML files for groups

### Changed
- Improved documentation

### Fixed
- XML output generation
- Windows builds with VS2015

## [1.0.1] - 2015-03-12

### Added
- Macro for `assert_ptr_equal()`

### Fixed
- `test_realloc()` if 0 size is passed
- Objects packaging bug
- Building with newer gcc versions

## [1.0.0] - 2015-02-16

### Added
- New test runner with group fixtures (old runner deprecated)
- Extensible message output formatter
- jUnit XML message output
- Subunit message output
- Test Anything Protocol message output
- `skip()` command
- `test_realloc()`
- cmockery compat header

### Fixed
- A lot of bugs on Windows

## [0.4.1] - 2014-05-22

### Added
- CMOCKA_TEST_ABORT env variable to leave threading apps

### Fixed
- Count parameter of `expect_check()` macro
- Reporting the number of tests
- CMake config files

## [0.4.0] - 2014-04-11

### Added
- Support for group testing
- `assert_return_code()`
- Better messages for errors
- CMake config mode support

### Fixed
- Bug with `unit_test_setup()` and `unit_test_teardown()`
- A lot of small bugs

## [0.3.2] - 2013-11-06

### Fixed
- FindNSIS detection
- `unit_test_setup()` and `unit_test_teardown()`
- GTest and GCC message style conformance
- Stringification in `will_return_always()`

## [0.3.1] - 2013-07-10

### Fixed
- Pointer conversion on s390 and ppc (32bit big endian)
- The customer_database test on big endian

## [0.3.0] - 2013-06-05

### Added
- Better mock object example
- pkgconfig file
- New macros `mock_type()` and `mock_ptr_type()`
- More documentation

### Fixed
- Installation problems on some platforms

## [0.2.0] - 2013-01-14

### Added
- Doxygen API documentation
- New CMake build system
- Support to create Windows NSIS installer

### Fixed
- Examples which didn't work
- A huge amount of bugs

## Previous Versions (cmockery)

### [0.12] - 2008-09-15
- Made it possible to specify additional compiler, lib tool and
  link flags on Windows
- Added Windows makefile to the tar ball

### [0.1.1] - 2008-08-29
- Made it possible to specify executable, library and object
  output directories

### [0.1] - 2008-08-26
- Initial cmockery release: A lightweight library to simplify and
  generalize the process of writing unit tests for C applications

[Unreleased]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.8...HEAD
[1.1.8]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.7...cmocka-1.1.8
[1.1.7]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.6...cmocka-1.1.7
[1.1.6]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.5...cmocka-1.1.6
[1.1.5]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.4...cmocka-1.1.5
[1.1.4]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.3...cmocka-1.1.4
[1.1.3]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.2...cmocka-1.1.3
[1.1.2]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.1...cmocka-1.1.2
[1.1.1]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.1.0...cmocka-1.1.1
[1.1.0]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.0.1...cmocka-1.1.0
[1.0.1]: https://gitlab.com/cmocka/cmocka/compare/cmocka-1.0.0...cmocka-1.0.1
[1.0.0]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.4.1...cmocka-1.0.0
[0.4.1]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.4.0...cmocka-0.4.1
[0.4.0]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.3.2...cmocka-0.4.0
[0.3.2]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.3.1...cmocka-0.3.2
[0.3.1]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.3.0...cmocka-0.3.1
[0.3.0]: https://gitlab.com/cmocka/cmocka/compare/cmocka-0.2.0...cmocka-0.3.0
[0.2.0]: https://gitlab.com/cmocka/cmocka/tags/cmocka-0.2.0
