# CMocka cppcheck configuration

## Overview

The `cmocka.cfg` file is a cppcheck library configuration. This configuration
helps cppcheck understand CMocka's assertion model, particularly that assertion
failures do not return, which eliminates false positives in static analysis.

## Usage

### Command Line

To use the configuration with cppcheck directly:

```bash
cppcheck --library=./cppcheck/cmocka.cfg [other options] <source files>
```

Example:

```bash
cppcheck --library=./cppcheck/cmocka.cfg --enable=all src/
```

### Using with csbuild

[csbuild](https://github.com/csutils/csmock) is a wrapper tool for static
analyzers that can also run cppcheck. To pass the CMocka configuration to
cppcheck when using csbuild, set the `CSCPPC_ADD_OPTS` environment variable:

```bash
export CSCPPC_ADD_OPTS="--library=./cppcheck/cmocka.cfg"
csbuild [csbuild options]
```

#### GitLab CI Example

See `.gitlab-ci.yml` for an example of how this is used in the CMocka project's
CI pipeline:

```yaml
- export CSCPPC_ADD_OPTS="--library=./cppcheck/cmocka.cfg"
- csbuild
    --build-dir=obj-csbuild
    --prep-cmd="cmake -DCMAKE_BUILD_TYPE=Debug -DPICKY_DEVELOPER=ON -DUNIT_TESTING=ON @SRCDIR@"
    --build-cmd "make clean && make -j$(nproc)"
    --git-commit-range $CI_COMMIT_RANGE
    --color
    --print-current --print-fixed
```

## Configuration Details

The `cmocka.cfg` file defines:

- **Core functions**: `_fail()` - marked as `noreturn`
- **Assertion macros**: All CMocka assertion macros with their argument
  constraints
- **Function models**: Information about pointer validity, range checks, and
  expected values

This helps cppcheck perform more accurate control flow and value flow analysis
when analyzing code that uses CMocka.

## References

- [cppcheck manual - Library Configuration](https://cppcheck.sourceforge.io/manual.html#library-configuration)
- [csbuild documentation](https://github.com/csutils/csmock) - see also `man csbuild`
- [CMocka documentation](https://cmocka.org/)
