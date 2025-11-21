cmocka
======

cmocka is an elegant unit testing framework for C with support for mock
objects. It only requires the standard C library, works on a range of computing
platforms (including embedded) and with different compilers.

## Features

- Support for mock objects
- Test fixtures (setup and teardown functions)
- Only requires a C library
- Exception handling for signals (SIGSEGV, SIGILL, ...)
- No fork()
- Very well tested
- Testing of memory leaks, buffer overflows and underflows
- A set of assert macros
- Several supported output formats (stdout, TAP, JUnit XML, Subunit)
- License: Apache License 2.0

## Supported Platforms

cmocka has been tested on:

- Linux (various distributions)
- BSD (FreeBSD, OpenBSD, NetBSD)
- Solaris
- macOS
- Windows

## Supported Compilers

- GCC
- Clang
- Microsoft Visual Studio
- MinGW

## Quick Start

### Simple Test Example

```c
#include <cmocka.h>

/* A test that will always pass */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

/* A test that will always fail */
static void null_test_fail(void **state) {
    (void) state; /* unused */
    assert_true(0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(null_test_fail),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### Building the Example

```bash
gcc -o example example.c -lcmocka
./example
```

## Building cmocka

### Requirements

- CMake >= 3.10.0
- C compiler (GCC, Clang, MSVC, etc.)

### Building on Linux/Unix

```bash
# Configure the build (creates build directory)
cmake -S . -B build

# Build the library
cmake --build build

# Run tests
cmake --build build --target test

# Install (optional)
sudo cmake --install build
```

### Building on Windows

The easiest way to use cmocka on Windows is with [vcpkg](https://vcpkg.link/ports/cmocka):

```cmd
vcpkg install cmocka
```

Alternatively, you can build from source using the CMake GUI or command line:

```cmd
cmake -S . -B build -G "Visual Studio 16 2019"
cmake --build build
```

## Testing

To run the cmocka test suite:

```bash
cmake -S . -B build -DUNIT_TESTING=ON
cmake --build build
cmake --build build --target test
```

For verbose test output:

```bash
cd build
ctest -V
```

## Documentation

- **API Documentation**: https://api.cmocka.org/
- **Website**: https://cmocka.org/
- **Local Documentation**: After building, see `build/doc/html/index.html`

To build the documentation locally:

```bash
cmake --build build --target docs
```

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

Quick overview:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add or update unit tests!
5. Ensure all tests pass
6. Submit a merge request

For bug reports and feature requests, please use the issue tracker at:
https://gitlab.com/cmocka/cmocka/

## License

cmocka is licensed under the Apache License 2.0. See the [COPYING](COPYING) file
for details.

## Resources

- **Website**: https://cmocka.org
- **GitLab**: https://gitlab.com/cmocka/cmocka
- **Mailing List**: https://listadmin.mudgum.io/postorius/lists/cmocka-devel.cmocka.org/
- **Changelog**: [CHANGELOG.md](CHANGELOG.md)
