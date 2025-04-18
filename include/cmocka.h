/*
 * Copyright 2008 Google Inc.
 * Copyright 2014-2022 Andreas Schneider <asn@cryptomilk.org>
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
#ifndef CMOCKA_H_
#define CMOCKA_H_

#ifdef _WIN32
# ifdef _MSC_VER

#  ifndef CMOCKA_STATIC
#   ifdef CMOCKA_EXPORTS
#define CMOCKA_DLLEXTERN __declspec(dllexport)
#   else
#define CMOCKA_DLLEXTERN __declspec(dllimport)
#   endif /* CMOCKA_EXPORTS */
#  endif /* ndef CMOCKA_STATIC */

#define __func__ __FUNCTION__

# ifndef inline
#define inline __inline
# endif /* inline */

#  if _MSC_VER < 1500
#   ifdef __cplusplus
extern "C" {
#   endif   /* __cplusplus */
int __stdcall IsDebuggerPresent();
#   ifdef __cplusplus
} /* extern "C" */
#   endif   /* __cplusplus */
#  endif  /* _MSC_VER < 1500 */
# endif /* _MSC_VER */
#endif  /* _WIN32 */

/**
 * @def CMOCKA_DLLEXTERN
 * This attribute is needed when dynamically linking to a data object in a DLL.
 * It's optional (but increases performance) for dynamically linking to
 * functions in a DLL.
 * @see
 * https://github.com/MicrosoftDocs/cpp-docs/blob/bd5a4fbd8ea3dd47b5c7a228c266cdddcaca0e00/docs/cpp/dllexport-dllimport.md
 */
#ifndef CMOCKA_DLLEXTERN
#define CMOCKA_DLLEXTERN // only needed on MSVC compiler when using a DLL
#endif /* ndef CMOCKA_DLLEXTERN */

/**
 * @defgroup cmocka The CMocka API
 *
 * These headers or their equivalents MUST be included prior to including
 * this header file.
 * @code
 * #include <stdarg.h>
 * #include <stdbool.h>
 * #include <stddef.h>
 * #include <stdint.h>
 * #include <setjmp.h>
 * @endcode
 *
 * This allows test applications to use custom definitions of C standard
 * library functions and types.
 *
 * @{
 */

/* Perform an signed cast to intmax_t. */
#define cast_to_intmax_type(value) \
    ((intmax_t)(value))

/* Perform an unsigned cast to uintmax_t. */
#define cast_to_uintmax_type(value) \
    ((uintmax_t)(value))

/* Perform cast to double. */
#define cast_to_long_double_type(value) \
    ((double)(value))

/**
 * Perform a cast from an integer to CMockaValueData.
 *
 * For backwards compatibility reasons, this explicitly casts to `uintmax_t`.
 * For most compilers, this will suppress warnings about passing float/intmax_t
 * to this macro.
 */
#define cast_int_to_cmocka_value(value) \
    (CMockaValueData)                   \
    {                                   \
        .uint_val = (uintmax_t)(value)  \
    }

/** Perform a cast from a pointer to CMockaValueData. */
#define cast_ptr_to_cmocka_value(value) \
    (CMockaValueData)                   \
    {                                   \
        .ptr = (value)                  \
    }

/** Assign an integer value to CMockaValueData. */
#define assign_int_to_cmocka_value(value) \
    (CMockaValueData)                     \
    {                                     \
        .int_val = (value)                \
    }

/** Assign an unsigned integer value to CMockaValueData. */
#define assign_uint_to_cmocka_value(value) \
    (CMockaValueData)                      \
    {                                      \
        .uint_val = (value)                \
    }

/** Assign a floating point value to CMockaValueData. */
#define assign_double_to_cmocka_value(value) \
    (CMockaValueData)                        \
    {                                        \
        .real_val = (value)                  \
    }

/* Nested macros are not expanded when they appear along with # or ## */
#define cmocka_tostring(val) #val

/* GCC have printf type attribute check.  */
#ifdef __GNUC__
#define CMOCKA_PRINTF_ATTRIBUTE(a,b) \
    __attribute__ ((__format__ (__printf__, a, b)))
#else
#define CMOCKA_PRINTF_ATTRIBUTE(a,b)
#endif /* __GNUC__ */

#if defined(__GNUC__)
#define CMOCKA_DEPRECATED __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define CMOCKA_DEPRECATED __declspec(deprecated)
#else
#define CMOCKA_DEPRECATED
#endif

#if defined(__GNUC__)
#define CMOCKA_NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
#define CMOCKA_NORETURN __declspec(noreturn)
#else
#define CMOCKA_NORETURN
#endif

/**
 * @def CMOCKA_NO_ACCESS_ATTRIBUTE
 *
 * Function attribute that tells the compiler that we never access the value
 * of a/b, just the pointer address.
 *
 * Without this, newer compilers like GCC-12 will print
 * `-Wmaybe-uninitialized` warnings.
 *
 * @see
 * https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Common-Function-Attributes.html#Common-Function-Attributes
 */
#ifdef __has_attribute
#if __has_attribute(access)
#define CMOCKA_NO_ACCESS_ATTRIBUTE \
    __attribute__((access(none, 1), access(none, 2)))
#endif
#endif
#ifndef CMOCKA_NO_ACCESS_ATTRIBUTE
#define CMOCKA_NO_ACCESS_ATTRIBUTE
#endif

#define WILL_RETURN_ALWAYS -1
#define WILL_RETURN_ONCE -2

#define EXPECT_ALWAYS -1
#define EXPECT_MAYBE -2

/**
 * @defgroup cmocka_mock Mock Objects
 * @ingroup cmocka
 *
 * Mock objects are simulated objects that mimic the behavior of
 * real objects. Instead of calling the real objects, the tested object calls a
 * mock object that merely asserts that the correct methods were called, with
 * the expected parameters, in the correct order.
 *
 * <ul>
 * <li><strong>will_return(function, value)</strong> - The will_return() macro
 * pushes a value onto a queue of mock values. This macro is intended to be
 * used by the unit test itself, while programming the behaviour of the mocked
 * object.</li>
 *
 * <li><strong>mock()</strong> - the mock macro pops a value from a queue of
 * test values. The user of the mock() macro is the mocked object that uses it
 * to learn how it should behave.</li>
 * </ul>
 *
 * Because the will_return() and mock() are intended to be used in pairs, the
 * cmocka library would fail the test if there are more values pushed onto the
 * queue using will_return() than consumed with mock() and vice-versa.
 *
 * The following unit test stub illustrates how would a unit test instruct the
 * mock object to return a particular value:
 *
 * @code
 * will_return_ptr_type(chef_cook, "hotdog", const char *);
 * will_return_int(chef_cook, 0);
 * @endcode
 *
 * Now the mock object can check if the parameter it received is the parameter
 * which is expected by the test driver. This can be done the following way:
 *
 * @code
 * int chef_cook(const char *order, char **dish_out)
 * {
 *     *dish_out = mock_ptr_type(char *); // "hotdog"
 *     int return_code = mock_int(); // 0
 *     return return_code;
 * }
 * @endcode
 *
 * For a complete example please take a look
 * <a
 * href="https://git.cryptomilk.org/projects/cmocka.git/tree/example/mock">here</a>.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Retrieve a return value of the current function.
 *
 * @return The value which was stored to return by this function.
 *
 * @see will_return()
 */
uintmax_t mock(void);
#else
#define mock() (_mock(__func__, __FILE__, __LINE__, NULL)).uint_val
#endif


#ifdef DOXYGEN
/**
 * @brief Retrieve a value of the current function and cast it to given type.
 *
 * The value would be casted to type internally to avoid having the
 * caller to do the cast manually. Type safety checks are disabled with that
 * functoin.
 *
 * @param[in]  #type  The expected type of the return value
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * int param;
 *
 * param = mock_type(int);
 * @endcode
 *
 * @see will_return()
 */
#type mock_type(#type);
#else
#define mock_type(type) ((type) mock())
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve an integer return value of the current function.
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * intmax_t param;
 *
 * param = mock_int();
 * @endcode
 *
 * @see will_return_int()
 */
intmax_t mock_int();
#else
/* TODO: Enable type safety check by passing intmax_t instead of NULL */
#define mock_int() (_mock(__func__, __FILE__, __LINE__, NULL)).int_val
#endif


#ifdef DOXYGEN
/**
 * @brief Retrieve an unsigned integer return value of the current function.
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * uintmax_t param;
 *
 * param = mock_uint();
 * @endcode
 *
 * @see will_return_uint()
 */
uintmax_t mock_uint(void);
#else
#define mock_uint() (_mock(__func__, __FILE__, __LINE__, "uintmax_t")).uint_val
#endif


#ifdef DOXYGEN
/**
 * @brief Retrieve an unsigned integer return value of the current function.
 *
 * @return The value which was stored to return by this function.
 *
 * @see will_return_float()
 */
double mock_float(void);
#else
#define mock_float() (_mock(__func__, __FILE__, __LINE__, NULL)).real_val
#endif


#ifdef DOXYGEN
/**
 * @brief Retrieve a typed return value of the current function.
 *
 * The value would be casted to type internally to avoid having the
 * caller to do the cast manually.
 *
 * @param[in]  #type  The expected type of the return value
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * char *param;
 *
 * param = mock_ptr_type(char *);
 * @endcode
 *
 * @see will_return_ptr_type()
 */
type mock_ptr_type(#type);
#else
#define mock_ptr_type(type) \
    ((type)(_mock(__func__, __FILE__, __LINE__, #type)).ptr)
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named value for the current function.
 *
 * @param[in]  #name  The name under which to look for the value
 *
 * @return The value which was stored under the given name for this function.
 *
 * @code
 * int param;
 * param = (int)mock_parameter(number);
 * @endcode
 *
 * @see mock()
 * @see mock_parameter_type()
 * @see mock_parameter_int()
 * @see mock_parameter_uint()
 * @see mock_parameter_float()
 * @see mock_parameter_ptr()
 * @see mock_parameter_ptr_type()
 * @see will_return()
 * @see will_set_parameter()
 * @see will_set_parameter_int()
 * @see will_set_parameter_uint()
 * @see will_set_parameter_float()
 * @see will_set_parameter_count()
 * @see will_set_parameter_always()
 * @see will_set_parameter_maybe()
 * @see will_set_parameter_ptr()
 * @see will_set_parameter_ptr_type()
 * @see will_set_parameter_ptr_coint()
 * @see will_set_parameter_ptr_always()
 * @see will_set_parameter_ptr_maybe()
 */
uintmax_t mock_parameter(#name);
#else
#define mock_parameter(name) \
    (_mock_parameter(__func__, #name, __FILE__, __LINE__, NULL)).uint_val
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named value for the current function and cast it to given type.
 *
 * The value would be casted to type internally to avoid having the
 * caller to do the cast manually. Type safety checks are disabled with that
 * function.
 *
 * @param[in]  #name  The name under which to look for the value
 *
 * @param[in]  #type  The expected type of the named value
 *
 * @return The value which was stored under name for this function.
 *
 * @code
 * int param;
 *
 * param = mock_parameter_type(param, int);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 */
#type mock_parameter_type(#name, #type);
#else
#define mock_parameter_type(name, type) ((type) mock_parameter(#name))
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named integer value for the current function.
 *
 * @param[in]  #name  The name under which to look for the value
 *
 * @return The integer value which was stored under the given name for this function.
 *
 * @code
 * intmax_t param;
 *
 * param = mock_parameter_int(param);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_int()
 */
intmax_t mock_parameter_int(#name);
#else
#define mock_parameter_int(name) \
    (_mock_parameter(__func__, #name, __FILE__, __LINE__, "intmax_t")).int_val
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve an unsigned integer return value of the current function.
 *
 * @param[in]  #name  The name under which to look for the value
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * uintmax_t param;
 *
 * param = mock_parameter_uint(param);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_uint()
 */
uintmax_t mock_parameter_uint(#name);
#else
#define mock_parameter_uint(name) \
    (_mock_parameter(__func__, #name, __FILE__, __LINE__, "uintmax_t")).uint_val
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named double value for the current function.
 *
 * @param[in]  #name  The name under which to look for the value
 *
 * @return The value which was stored to return by this function.
 *
 * @code
 * double param;
 *
 * param = mock_parameter_float(param);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_float()
 */
double mock_parameter_float(#name);
#else
#define mock_parameter_float(name) \
    (_mock_parameter(__func__, #name, __FILE__, __LINE__, "double")).real_val
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named pointer for the current function.
 *
 * @param[in]  #name  The name under which to look for the pointer
 *
 * @return The pointer which was stored to return by this function.
 *
 * @code
 * int *result
 * result = (int*)mock_parameter_ptr(result);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_ptr()
 */
void *mock_parameter_ptr(#name)
#else
#define mock_parameter_ptr(name) \
    ((_mock_parameter(__func__, #name, __FILE__, __LINE__, NULL)).ptr)
#endif

#ifdef DOXYGEN
/**
 * @brief Retrieve a named pointer for the current function.
 *
 * In addition it checks if if the type specified by the call to
 * will_return_ptr_type() is the same.
 * And casts it to that type.
 *
 * @param[in]  #name  The name under which to look for the pointer
 *
 * @return The pointer which was stored to return by this function.
 *
 * @code
 * int *result
 * result = mock_parameter_ptr_type(result, int*);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_ptr_type()
 */
type mock_parameter_ptr_type(#name, #type)
#else
#define mock_parameter_ptr_type(name, type) \
    ((type)(_mock_parameter(__func__, #name, __FILE__, __LINE__, #type)).ptr)
#endif

#ifdef DOXYGEN
/**
 * @brief set errno for the current function.
 *
 * @code
 * mock_errno();
 * @endcode
 *
 * @see will_set_errno()
 */
void mock_errno()
#else
#define mock_errno()                      \
    do {                                  \
        intmax_t err = (_mock_parameter(  \
                    __func__,             \
                    "/errno",             \
                    __FILE__,             \
                    __LINE__,             \
                    "errno")).int_val;    \
        if (err != 0) {                   \
            errno = err;                  \
        }                                 \
    } while (0)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @code
 * int return_integer(void)
 * {
 *      return (int)mock();
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_return(return_integer, 42);
 *
 *      assert_int_equal(my_function_calling_return_integer(), 42);
 * }
 * @endcode
 *
 * @see mock()
 * @see mock_int()
 * @see mock_uint()
 * @see mock_float()
 * @see will_return_int()
 * @see will_return_uint()
 * @see will_return_float()
 * @see will_return_ptr_type()
 * @see will_return_count()
 * @see will_return_always()
 * @see will_return_ptr_always()
 */
void will_return(#function, uintmax_t value);
#else
#define will_return(function, value)              \
    _will_return(cmocka_tostring(function),       \
                 __FILE__,                        \
                 __LINE__,                        \
                 NULL,                            \
                 cast_int_to_cmocka_value(value), \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store an integer value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @code
 * int32_t return_int32(void)
 * {
 *      return (int32_t)mock_int();
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_return_int(return_int32, -42);
 *
 *      assert_int_equal(my_function_calling_return_int32(), -42);
 * }
 * @endcode
 *
 * @see mock_int()
 */
void will_return_int(#function, intmax_t value);
#else
#define will_return_int(function, value)            \
    _will_return(#function,                         \
                 __FILE__,                          \
                 __LINE__,                          \
                 "intmax_t",                        \
                 assign_int_to_cmocka_value(value), \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a unsigned integer value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @code
 * uint32_t return_uint32(void)
 * {
 *      return (uint32_t)mock_uint();
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_return_int(return_uint32, 42);
 *
 *      assert_uint_equal(my_function_calling_return_uint32(), 42);
 * }
 * @endcode
 *
 * @see mock_uint()
 * @see will_return_count()
 */
void will_return_uint(#function, uintmax_t value);
#else
#define will_return_uint(function, value)            \
    _will_return(#function,                          \
                 __FILE__,                           \
                 __LINE__,                           \
                 "uintmax_t",                        \
                 assign_uint_to_cmocka_value(value), \
                 1)
#endif


#ifdef DOXYGEN
/**
 * @brief Store a floating point value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @code
 * float return_float(void)
 * {
 *      return (float)mock_double();
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_return_int(return_float, 1.0);
 *
 *      assert_float_equal(my_function_calling_return_float(), 1.0);
 * }
 * @endcode
 *
 * @see mock_float()
 */
void will_return_float(#function, intmax_t value);
#else
#define will_return_float(function, value)             \
    _will_return(#function,                            \
                 __FILE__,                             \
                 __LINE__,                             \
                 "float",                              \
                 assign_double_to_cmocka_value(value), \
                 1)
#endif


#ifdef DOXYGEN
/**
 * @brief Store a value to be returned by mock() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock().
 *
 * @param[in]  count The parameter indicates the number of times the value should
 *                   be returned by mock(). If count is set to -1, the value
 *                   will always be returned but must be returned at least once.
 *                   If count is set to -2, the value will always be returned
 *                   by mock(), but is not required to be returned.
 *
 * @see mock()
 */
void will_return_count(#function, uintmax_t value, int count);
#else
#define will_return_count(function, value, count) \
    _will_return(cmocka_tostring(function),       \
                 __FILE__,                        \
                 __LINE__,                        \
                 NULL,                            \
                 cast_int_to_cmocka_value(value), \
                 count)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value that will be always returned by mock().
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  #value The value to be returned by mock().
 *
 * This is equivalent to:
 * @code
 * will_return_count(function, value, -1);
 * @endcode
 *
 * @see will_return_count()
 * @see mock()
 */
void will_return_always(#function, uintmax_t value);
#else
#define will_return_always(function, value) \
    will_return_count(function, (value), WILL_RETURN_ALWAYS)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value that may be always returned by mock().
 *
 * This stores a value which will always be returned by mock() but is not
 * required to be returned by at least one call to mock(). Therefore,
 * in contrast to will_return_always() which causes a test failure if it
 * is not returned at least once, will_return_maybe() will never cause a test
 * to fail if its value is not returned.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  #value The value to be returned by mock().
 *
 * This is equivalent to:
 * @code
 * will_return_count(function, value, -2);
 * @endcode
 *
 * @see will_return_count()
 * @see mock()
 */
void will_return_maybe(#function, uintmax_t value);
#else
#define will_return_maybe(function, value) \
    will_return_count(function, (value), WILL_RETURN_ONCE)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a pointer value to be returned by mock_ptr_type() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock_ptr_type().
 *
 * @code
 * const char * return_pointer(void)
 * {
 *      return mock_ptr_type(const char *);
 * }
 *
 * static void test_pointer_return(void **state)
 * {
 *      will_return(return_pointer, "hello world");
 *
 *      assert_string_equal(my_function_calling_return_integer(), 42);
 * }
 * @endcode
 *
 * @see mock_ptr_type()
 * @see will_return_ptr_count()
 */
void will_return_ptr(#function, void *value);
#else
#define will_return_ptr(function, value)          \
    _will_return(#function,                       \
                 __FILE__,                        \
                 __LINE__,                        \
                 NULL,                            \
                 cast_ptr_to_cmocka_value(value), \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a pointer value to be returned by mock_ptr_type() later.
 *
 * This will also check that the type matches and if not will fail().
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock_ptr_type().
 *
 * @param[in]  value The type of the pointer.
 * @code
 * const char *return_pointer(void)
 * {
 *      return mock_ptr_type(const char *);
 * }
 *
 * static void test_pointer_return(void **state)
 * {
 *      will_return_ptr_type(return_pointer, "hello world", const char *);
 *
 *      assert_string_equal(my_func_calling_return_pointer(), "hello world");
 * }
 * @endcode
 *
 * @see mock_ptr_type()
 */
void will_return_ptr_type(#function, void *value, type);
#else
#define will_return_ptr_type(function, value, type) \
    _will_return(#function,                         \
                 __FILE__,                          \
                 __LINE__,                          \
                 #type,                             \
                 cast_ptr_to_cmocka_value(value),   \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a pointer value to be returned by mock_ptr_type() later.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  value The value to be returned by mock_ptr_type().
 *
 * @param[in]  count The parameter indicates the number of times the value should
 *                   be returned by mock(). If count is set to -1, the value
 *                   will always be returned but must be returned at least once.
 *                   If count is set to -2, the value will always be returned
 *                   by mock(), but is not required to be returned.
 *
 * @see mock_ptr_type()
 */
void will_return_ptr_count(#function, void *value, int count);
#else
#define will_return_ptr_count(function, value, count) \
    _will_return(#function,                           \
                 __FILE__,                            \
                 __LINE__,                            \
                 NULL,                                \
                 cast_ptr_to_cmocka_value(value),     \
                 count)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value that will be always returned by mock_ptr_type().
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  #value The value to be returned by mock_ptr_type().
 *
 * This is equivalent to:
 * @code
 * will_return_ptr_count(function, value, -1);
 * @endcode
 *
 * @see will_return_ptr_count()
 * @see mock_ptr_type()
 */
void will_return_ptr_always(#function, void *value);
#else
#define will_return_ptr_always(function, value) \
    will_return_ptr_count(function, (value), WILL_RETURN_ALWAYS)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a value that may be always returned by mock_ptr_type().
 *
 * This stores a value which will always be returned by mock_ptr_type() but is
 * not required to be returned by at least one call to mock_ptr_type().
 * Therefore, in contrast to will_return_ptr_always() which causes a test
 * failure if it is not returned at least once, will_return_ptr_maybe() will
 * never cause a test to fail if its value is not returned.
 *
 * @param[in]  #function  The function which should return the given value.
 *
 * @param[in]  #value The value to be returned by mock_ptr_type().
 *
 * This is equivalent to:
 * @code
 * will_return_ptr_count(function, value, -2);
 * @endcode
 *
 * @see will_return_ptr_count()
 * @see mock_ptr_type()
 */
void will_return_ptr_maybe(#function, void *value);
#else
#define will_return_ptr_maybe(function, value) \
    will_return_ptr_count(function, (value), WILL_RETURN_ONCE)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named value to be returned by mock_parameter() later.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @code
 * void return_integer(int *result)
 * {
 *      *result = (int)mock_parameter(result);
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_set_parameter(return_integer, result, 42);
 *
 *      int retVal = 0;
 *      my_function_calling_return_integer(&retVal);
 *      assert_int_equal(result, 42);
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter()
 * @see mock_parameter_int()
 * @see mock_parameter_uint()
 * @see mock_parameter_float()
 * @see mock_parameter_ptr()
 * @see mock_parameter_ptr_type()
 * @see will_set_parameter_int()
 * @see will_set_parameter_uint()
 * @see will_set_parameter_float()
 * @see will_set_parameter_ptr()
 * @see will_set_parameter_ptr_type()
 * @see will_set_parameter_count()
 * @see will_set_parameter_always()
 * @see will_set_parameter_maybe()
 * @see will_set_parameter_ptr_count()
 * @see will_set_parameter_ptr_always()
 * @see will_set_parameter_ptr_maybe()
 */
void will_set_parameter(#function, #name, uintmax_t value);
#else
#define will_set_parameter(function, name, value)  \
    _will_set_parameter(cmocka_tostring(function), \
                 #name,                           \
                 __FILE__,                        \
                 __LINE__,                        \
                 NULL,                            \
                 cast_int_to_cmocka_value(value), \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named integer value to be returned by mock_parameter() later.
 *
 * And adds some type checking information to be able to check
 * with call to mock_parameter_int().
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @code
 * void return_int32(int32_t *result)
 * {
 *      *result = (int32_t)mock_parameter_int(result);
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_set_parameter_int(return_int32, result, -42);
 *      int32_t result_param = 0;
 *      return_int32(&result_param);
 *      assert_int_equal(result_param, -42);
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_int()
 * @see will_set_parameter()
 */
void will_set_parameter_int(#function, #name, intmax_t value);
#else
#define will_set_parameter_int(function, name, value) \
    _will_set_parameter(#function,                    \
                 #name,                              \
                 __FILE__,                           \
                 __LINE__,                           \
                 "intmax_t",                         \
                 assign_int_to_cmocka_value(value),  \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named unsigned integer value to be returned by mock_parameter() later.
 *
 * And adds some type checking information to be able to check
 * with call to mock_parameter_uint().
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @code
 * void return_uint32(uint32_t *result)
 * {
 *      *result =(uint32_t)mock_parameter_uint(result);
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_set_parameter_uint(return_uint32, result 42);
 *      int32_t result_param = 0;
 *      return_uint32(&result_param);
 *      assert_uint_equal(result_param, 42);
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_uint()
 * @see will_set_parameter()
 */
void will_return_uint(#function, #name, uintmax_t value);
#else
#define will_set_parameter_uint(function, name, value) \
    _will_set_parameter(#function,                     \
                 #name,                               \
                 __FILE__,                            \
                 __LINE__,                            \
                 "uintmax_t",                         \
                 assign_uint_to_cmocka_value(value),  \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named double value to be returned by mock_parameter() later.
 *
 * And adds some type checking information to be able to check
 * with call to mock_parameter_float().
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @code
 * void return_float(double *result)
 * {
 *      *result = mock_parameter_float(result);
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_set_parameter_float(return_float, 34.7);
 *      double result_param = NAN;
 *      return_float(&result_param);
 *      assert_float_equal(result_param, 34.7, 0.0);
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_float()
 * @see will_set_parameter()
 */
void will_set_parameter_float(#function, #name, double value);
#else
#define will_set_parameter_float(function, name, value) \
    _will_set_parameter(#function,                      \
                 #name,                                \
                 __FILE__,                             \
                 __LINE__,                             \
                 "double",                             \
                 assign_double_to_cmocka_value(value), \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named value to be returned a specified number of times
 * by mock_parameter() later.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @param[in]  count The parameter indicates the number of times the value should
 *                   be returned by mock(). If count is set to -1, the value
 *                   will always be returned but must be returned at least once.
 *                   If count is set to -2, the value will always be returned
 *                   by mock(), but is not required to be returned.
 *
 * @code
 * void return_two_int(int *resultA, int *resultB)
 * {
 *      *resultA = mock_parameter_float(result);
 *      *resultB = mock_parameter_float(result);
 * }
 *
 * static void test_integer_return(void **state)
 * {
 *      will_set_parameter_count(return_two_int, result, 3, 2);
 *      int result_paramA = 0;
 *      int result_paramB = 0;
 *      return_two_int(&result_paramA, &result_paramB);
 *      assert_int_equal(result_paramA, 3);
 *      assert_int_equal(result_paramB, 3);
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 */
void will_set_parameter_count(#function, #name, uintmax_t value, int count);
#else
#define will_set_parameter_count(function, name, value, count) \
    _will_set_parameter(cmocka_tostring(function),             \
                 #name,                                       \
                 __FILE__,                                    \
                 __LINE__,                                    \
                 NULL,                                        \
                 cast_int_to_cmocka_value(value),             \
                 count)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named value that will be always be returned by mock_parameter().
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * This is equivalent to:
 * @code
 * will_return_count(function, value, -1);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_set_parameter()
 * @see will_set_parameter_count()
 */
void will_set_parameter_always(#function, #name, uintmax_t value);
#else
#define will_set_parameter_always(function, name, value) \
    will_set_parameter_count(function, name, (value), WILL_RETURN_ALWAYS)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named value that may be always returned by mock_parameter().
 *
 * This stores a value which will always be returned by mock_parameter() but is not
 * required to be returned by at least one call to mock_parameter(). Therefore,
 * in contrast to will_set_parameter_always() which causes a test failure if it
 * is not returned at least once, will_set_parameter_maybe() will never cause a test
 * to fail if its value is not returned.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * This is equivalent to:
 * @code
 * will_set_parameter_count(function, name, value, -2);
 * @endcode
 *
 * @see mock_parameter()
 * @see will_return()
 * @see will_return_count()
 */
void will_set_parameter_maybe(#function, #name, uintmax_t value);
#else
#define will_set_parameter_maybe(function, name, value) \
    will_set_parameter_count(function, name, (value), WILL_RETURN_ONCE)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named pointer value to be returned by mock_parameter() later.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @code
 * void return_pointer(const char **result)
 * {
 *      *result = (const char *)mock_parameter_ptr(result);
 * }
 * static void test_pointer_return(void **state)
 * {
 *      will_set_parameter ptr(return_pointer, result, "hello world");
 *      const char *returned = NULL;
 *      my_func_calling_return_pointer(&returned);
 *      assert_string_equal(returned, "hello world");
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_ptr()
 * @see will_set_parameter()
 */
void will_set_parameter_ptr(#function, #name, void *value);
#else
#define will_set_parameter_ptr(function, name, value) \
    _will_set_parameter(#function,                    \
                 #name,                              \
                 __FILE__,                           \
                 __LINE__,                           \
                 NULL,                               \
                 cast_ptr_to_cmocka_value(value),    \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named pointer value to be returned by mock_parameter() later.
 *
 * This will also check that the type matches and if not will fail().
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value The value to be returned by mock_parameter().
 *
 * @param[in]  type The type of the pointer.
 *
 * @code
 * void return_pointer(const char **result)
 * {
 *      *result = mock_parameter_ptr_typed(result, const char*);
 * }
 * static void test_pointer_return(void **state)
 * {
 *      will_set_parameter_ptr_type(return_pointer, result, "hello world", const char*);
 *      const char *returned = NULL;
 *      my_func_calling_return_pointer(&returned);
 *      assert_string_equal(returned, "hello world");
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_ptr()
 * @see mock_parameter_ptr_type()
 * @see will_set_parameter()
 * @see will_set_parameter_ptr()
 */
void will_set_parameter_ptr_type(#function, #name, void *value, #type);
#else
#define will_set_parameter_ptr_type(function, name, value, type) \
    _will_set_parameter(#function,                               \
                 #name,                                         \
                 __FILE__,                                      \
                 __LINE__,                                      \
                 #type,                                         \
                 cast_ptr_to_cmocka_value(value),               \
                 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named pointer value to be returned a specified number of times
 * by mock_parameter_ptr() later.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value  The value to be returned by mock_parameter_ptr().
 *
 * @param[in]  count The parameter indicates the number of times the value should
 *                   be returned by mock_parameter_ptr(). If count is set to -1, the value
 *                   will always be returned but must be returned at least once.
 *                   If count is set to -2, the value will always be returned
 *                   by mock_parameter_ptr(), but is not required to be returned.
 *
 * @code
 * void return_pointer(const char **resultA, const char **resultB)
 * {
 *      *resultA = (const char *)mock_parameter_ptr(result);
 *      *resultB = (const char *)mock_parameter_ptr(result);
 * }
 * static void test_pointer_return(void **state)
 * {
 *      will_set_parameter_ptr_count(return_pointer, result, "hello world", const char*, 2);
 *      const char *returnedA = NULL;
 *      const char *returnedB = NULL;
 *      my_func_calling_return_pointer(&returnedA, &returnedB);
 *      assert_string_equal(returnedA, "hello world");
 *      assert_string_equal(returnedB, "hello world");
 * }
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_ptr()
 * @see will_named_return()
 * @see will_named_return_ptr()
 */
void will_set_parameter_ptr_count(#function, #name, void *value, #type, count);
#else
#define will_set_parameter_ptr_count(function, name, value, count) \
    _will_set_parameter(#function,                                 \
                 #name,                                           \
                 __FILE__,                                        \
                 __LINE__,                                        \
                 NULL,                                            \
                 cast_ptr_to_cmocka_value(value),                 \
                 count)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named pointer value that may be always returned by mock_parameter_ptr().
 *
 * This stores a value which will always be returned by mock_parameter_ptr() is
 * not required to be returned by at least one call to mock_parameter_ptr().
 * If it is not returned at least once the test will fail.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value  The value to be returned by mock_parameter_ptr().
 *
 * This is equivalent to:
 * @code
 * will_return_ptr_count(function, value, -1);
 * @endcode
 *
 * @see mock_parameter()
 * @see mock_parameter_ptr()
 * @see will_set_parameter()
 * @see will_set_parameter_ptr()
 * @see will_set_parameter_ptr_count()
 */
void will_set_parameter_ptr_always(#function, #name, void *value);
#else
#define will_set_parameter_ptr_always(function, name, value) \
    will_set_parameter_ptr_count(function, name, (value), WILL_RETURN_ALWAYS)
#endif

#ifdef DOXYGEN
/**
 * @brief Store a named pointer value that may be always returned by mock_parameter_ptr().
 *
 * This stores a value which will always be returned by mock_parameter_ptr() but is
 * not required to be returned by at least one call to mock_parameter_ptr().
 * Therefore, in contrast to will_set_parameter_ptr_always() which causes a test
 * failure if it is not returned at least once, will_set_parameter_ptr() will
 * never cause a test to fail if its value is not returned.
 *
 * @param[in]  #function  The function in which the given value should be return.
 *
 * @param[in]  #name  The name under which the given value should be returned.
 *
 * @param[in]  value  The value to be returned by mock_parameter_ptr().
 *
 * This is equivalent to:
 * @code
 * will_return_ptr_count(function, value, -2);
 * @endcode
 *
 * @see mock_name()
 * @see mock_name_ptr()
 * @see will_set_parameter()
 * @see will_set_parameter_ptr()
 * @see will_set_parameter_ptr_count()
 */
void will_set_parameter_ptr_maybe(#function, #name, void *value);
#else
#define will_set_parameter_ptr_maybe(function, name, value) \
    will_set_parameter_ptr_count(function, name, (value), WILL_RETURN_ONCE)
#endif

#ifdef DOXYGEN
/**
 * @brief Store an integer value to set errno to by mock_errno() later.
 *
 * @param[in]  #function  The function in which errno
 * should be set to the given value.
 *
 * @param[in]  value The value to set errno to by the call to mock_errno().
 *
 * @code
 * void sets_errno(void)
 * {
 *      mock_errno();
 * }
 *
 * static void test_sets_errno(void **state)
 * {
 *      will_set_errno(sets_errno, -3);
 *
 *      assert_int_equal(errno, -3);
 * }
 * @endcode
 *
 * @see mock_errno()
 */
void will_set_errno(#function, intmax_t value);
#else
#define will_set_errno(function, value)         \
    _will_set_parameter(#function,              \
            "/errno",                           \
            __FILE__,                           \
            __LINE__,                           \
            "errno",                            \
            assign_int_to_cmocka_value(value),  \
            1)
#endif

#ifdef DOXYGEN
/**
 * @brief Store an integer value to always set errno to by mock_errno().
 *
 *
 *
 * @param[in]  #function  The function in which errno
 * should be set to the given value.
 *
 * @param[in]  value The value to set errno to by the call to mock_errno().
 *
 * @param[in]  count  The count parameter gives the number of times the value
 *                    should be validated by check_expected(). If count is set
 *                    to @ref EXPECT_ALWAYS the value will always be returned,
 *                    and cmocka expects check_expected() to be issued at least
 *                    once. If count is set to @ref EXPECT_MAYBE, any number of
 *                    calls to check_expected() is accepted, including zero.
 *
 * @code
 * void sets_errno(void)
 * {
 *      mock_errno();
 * }
 * static void test_sets_errno(void **state)
 * {
 *      will_set_errno_count(sets_errno, -3, 2);
 *      sets_errno();
 *      assert_int_equal(errno, -3);
 *      errno = 0;
 *      sets_errno();
 *      assert_int_equal(errno, -3);
 * }
 * @endcode
 *
 * @see mock_errno()
 * @see will_set_errno()
 * @see will_set_errno_always()
 * @see will_set_errno_maybe()
 */
void will_set_errno_count(#function, intmax_t value, size_t cont);
#else
#define will_set_errno_count(function, value, count)    \
    _will_set_parameter(#function,                      \
            "/errno",                                   \
            __FILE__,                                   \
            __LINE__,                                   \
            "errno",                                    \
            assign_int_to_cmocka_value(value),          \
            (count))
#endif

#ifdef DOXYGEN
/**
 * @brief Store an integer value to set errno to by mock_errno() later,
 * for a specified number of times.
 *
 * This stores a value which will errno will always be set to by mock_errno()
 * but is required to be set least once by a call to mock_errno().
 *
 * @param[in]  #function  The function in which errno
 * should be set to the given value.
 *
 * @param[in]  value The value to set errno to by the call to mock_errno().
 *
 * This is equivalent to:
 * @code
 * will_set_parameter_count(function, name, value, -1);
 * @endcode
 *
 * @see mock_errno()
 * @see will_set_errno()
 * @see will_set_errno_count()
 * @see will_set_errno_maybe()
 */
void will_set_errno_always(#function, intmax_t value);
#else
#define will_set_errno_always(function, value)    \
    will_set_errno_count(function, (value), WILL_RETURN_ALWAYS);
#endif

#ifdef DOXYGEN
/**
 * @brief Store an integer value to set errno to by mock_errno() later,
 * for a specified number of times.
 *
 * This stores a value which will errno will always be set to by mock_errno()
 * and won't fail if mock_errno() is never called.
 *
 * @param[in]  #function  The function in which errno
 * should be set to the given value.
 *
 * @param[in]  value The value to set errno to by the call to mock_errno().
 *
 * This is equivalent to:
 * @code
 * will_set_parameter_count(function, name, value, -2);
 * @endcode
 *
 * @see mock_errno()
 * @see will_set_errno()
 * @see will_set_errno_count()
 * @see will_set_errno_always()
 */
void will_set_errno_maybe(#function, intmax_t value);
#else
#define will_set_errno_maybe(function, value)    \
    will_set_errno_count(function, (value), WILL_RETURN_ONCE);
#endif
/** @} */

/**
 * @defgroup cmocka_param Checking Parameters
 * @ingroup cmocka
 *
 * Functionality to store expected values for mock function parameters.
 *
 * In addition to storing the return values of mock functions, cmocka provides
 * functionality to store expected values for mock function parameters using
 * the expect_*() functions provided. A mock function parameter can then be
 * validated using the check_expected() macro.
 *
 * Successive calls to expect_*() macros for a parameter queues values to check
 * the specified parameter. check_expected() checks a function parameter
 * against the next value queued using expect_*(), if the parameter check fails
 * a test failure is signalled. In addition if check_expected() is called and
 * no more parameter values are queued a test failure occurs.
 *
 * The following test stub illustrates how to do this. First is the the function
 * we call in the test driver:
 *
 * @code
 * static void test_driver(void **state)
 * {
 *     expect_string(chef_cook, order, "hotdog");
 * }
 * @endcode
 *
 * Now the chef_cook function can check if the parameter we got passed is the
 * parameter which is expected by the test driver. This can be done the
 * following way:
 *
 * @code
 * int chef_cook(const char *order, char **dish_out)
 * {
 *     check_expected(order);
 * }
 * @endcode
 *
 * For a complete example please take a look
 * <a href="https://git.cryptomilk.org/projects/cmocka.git/tree/example/mock">here</a>
 *
 * @{
 */


#ifdef DOXYGEN
/**
 * @brief Add a custom parameter checking function.
 *
 * If the event parameter is NULL the event structure is allocated internally
 * by this function. If the parameter is provided it must be allocated on the
 * heap and doesn't need to be deallocated by the caller.
 *
 * @param[in]  #function  The function to add a custom parameter checking
 *                        function for.
 *
 * @param[in]  #parameter The parameters passed to the function.
 *
 * @param[in]  #check_function  The check function to call.
 *
 * @param[in]  check_data       The data to pass to the check function.
 */
void expect_check(function,
                  parameter,
                  CheckParameterValue check_function,
                  CMockaValueData check_data);
#else
#define expect_check(function, parameter, check_function, check_data) \
    _expect_check(cmocka_tostring(function),                          \
                  cmocka_tostring(parameter),                         \
                  __FILE__,                                           \
                  __LINE__,                                           \
                  check_function,                                     \
                  check_data,                                         \
                  NULL,                                               \
                  1)
#endif


#ifdef DOXYGEN
/**
 * @brief Add a custom parameter checking function.
 *
 * If the event parameter is NULL the event structure is allocated internally
 * by this function. If the parameter is provided it must be allocated on the
 * heap and doesn't need to be deallocated by the caller.
 *
 * @param[in]  #function  The function to add a custom parameter checking
 *                        function for.
 *
 * @param[in]  #parameter The parameters passed to the function.
 *
 * @param[in]  #check_function  The check function to call.
 *
 * @param[in]  check_data       The data to pass to the check function.
 *
 * @param[in]  count  The count parameter gives the number of times the value
 *                    should be validated by check_expected(). If count is set
 *                    to @ref EXPECT_ALWAYS the value will always be returned,
 *                    and cmocka expects check_expected() to be issued at least
 *                    once. If count is set to @ref EXPECT_MAYBE, any number of
 *                    calls to check_expected() is accepted, including zero.
 *
 */
void expect_check_count(function,
                        parameter,
                        CheckParameterValue check_function,
                        CMockaValueData check_data,
                        size_t count);
#else
#define expect_check_count(function,          \
                           parameter,         \
                           check_function,    \
                           check_data,        \
                           count)             \
    _expect_check(cmocka_tostring(function),  \
                  cmocka_tostring(parameter), \
                  __FILE__,                   \
                  __LINE__,                   \
                  check_function,             \
                  check_data,                 \
                  NULL,                       \
                  count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected().
 */
void expect_in_set(#function, #parameter, uintmax_t value_array[]);
#else
#define expect_in_set(function, parameter, value_array) \
    expect_in_set_count(function, parameter, value_array, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        integer array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected().
 */
void expect_in_set(#function, #parameter, intmax_t value_array[]);
#else
#define expect_int_in_set(function, parameter, value_array) \
    expect_int_in_set_count(function, parameter, value_array, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        unsigned integer array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected().
 */
void expect_in_set(#function, #parameter, intmax_t value_array[]);
#else
#define expect_uint_in_set(function, parameter, value_array) \
    expect_uint_in_set_count(function, parameter, value_array, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_in_set_count(#function, #parameter, uintmax_t value_array[], size_t count);
#else
#define expect_in_set_count(function, parameter, value_array, count)    \
    _expect_uint_in_set(cmocka_tostring(function),                      \
                        cmocka_tostring(parameter),                     \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        value_array,                                    \
                        sizeof(value_array) / sizeof((value_array)[0]), \
                        count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        integer array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_int_in_set_count(#function, #parameter, intmax_t value_array[], size_t count);
#else
#define expect_int_in_set_count(function, parameter, value_array, count) \
    _expect_int_in_set(cmocka_tostring(function),                        \
                       cmocka_tostring(parameter),                       \
                       __FILE__,                                         \
                       __LINE__,                                         \
                       value_array,                                      \
                       sizeof(value_array) / sizeof((value_array)[0]),   \
                       count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is part of the provided
 *        unsigned integer array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_int_in_set_count(#function, #parameter, uintmax_t value_array[], size_t count);
#else
#define expect_uint_in_set_count(function, parameter, value_array, count) \
    _expect_uint_in_set(cmocka_tostring(function),                        \
                        cmocka_tostring(parameter),                       \
                        __FILE__,                                         \
                        __LINE__,                                         \
                        value_array,                                      \
                        sizeof(value_array) / sizeof((value_array)[0]),   \
                        count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is not part of the
 *        provided array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected().
 */
void expect_not_in_set(#function, #parameter, uintmax_t value_array[]);
#else
#define expect_not_in_set(function, parameter, value_array) \
    expect_not_in_set_count(function, parameter, value_array, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is not part of the
 *        provided array.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_not_in_set_count(#function, #parameter, uintmax_t value_array[], size_t count);
#else
#define expect_not_in_set_count(function, parameter, value_array, count) \
    _expect_not_in_set( \
        cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, value_array, \
        sizeof(value_array) / sizeof((value_array)[0]), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the float parameter value is part of the
 * provided array.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected_float().
 */
void expect_float_in_set(#function, #parameter, double value_array[], double epsilon);
#else
#define expect_float_in_set(function, parameter, value_array, epsilon) \
    expect_float_in_set_count(function, parameter, value_array, epsilon, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the float parameter value is part of the
 *        provided integer array.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected_float().
 */
void expect_float_in_set_count(#function, #parameter, double value_array[], double epsilon, size_t count);
#else
#define expect_float_in_set_count(function, parameter, value_array, epsilon, count) \
    _expect_float_in_set(cmocka_tostring(function),                                 \
                         cmocka_tostring(parameter),                                \
                         __FILE__,                                                  \
                         __LINE__,                                                  \
                         value_array,                                               \
                         sizeof(value_array) / sizeof((value_array)[0]),            \
                         epsilon,                                                   \
                         count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the float parameter value is not part of the
 * provided array.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @see check_expected_float().
 */
void expect_float_not_in_set(#function, #parameter, double value_array[], double epsilon);
#else
#define expect_float_not_in_set(function, parameter, value_array, epsilon) \
    expect_float_not_in_set_count(function, parameter, value_array, epsilon, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the float parameter value is not part of the
 *        provided integer array.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value_array[] The array to check for the value.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected_float().
 */
void expect_float_not_in_set_count(#function, #parameter, double value_array[], double epsilon, size_t count);
#else
#define expect_float_not_in_set_count(function, parameter, value_array, epsilon, count) \
    _expect_float_not_in_set(cmocka_tostring(function),                                 \
                             cmocka_tostring(parameter),                                \
                             __FILE__,                                                  \
                             __LINE__,                                                  \
                             value_array,                                               \
                             sizeof(value_array) / sizeof((value_array)[0]),            \
                             epsilon,                                                   \
                             count)
#endif


#ifdef DOXYGEN
/**
 * @brief Add an event to check a parameter is inside a numerical range.
 * The check would succeed if minimum <= value <= maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @see check_expected().
 */
void expect_in_range(#function, #parameter, uintmax_t minimum, uintmax_t maximum);
#else
#define expect_in_range(function, parameter, minimum, maximum) \
    expect_in_range_count(function, parameter, minimum, maximum, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check a parameter is inside a
 * numerical range. The check would succeed if minimum <= value <= maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_in_range_count(#function, #parameter, uintmax_t minimum, uintmax_t maximum, size_t count);
#else
#define expect_in_range_count(function, parameter, minimum, maximum, count) \
    _expect_in_range(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, minimum, \
                     maximum, count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check a parameter is outside a numerical range.
 * The check would succeed if minimum > value > maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @see check_expected().
 */
void expect_not_in_range(#function, #parameter, uintmax_t minimum, uintmax_t maximum);
#else
#define expect_not_in_range(function, parameter, minimum, maximum) \
    expect_not_in_range_count(function, parameter, minimum, maximum, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check a parameter is outside a
 * numerical range. The check would succeed if minimum > value > maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_not_in_range_count(#function, #parameter, uintmax_t minimum, uintmax_t maximum, size_t count);
#else
#define expect_not_in_range_count(function, parameter, minimum, maximum, \
                                  count) \
    _expect_not_in_range(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                         minimum, maximum, count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check a parameter is inside a numerical range.
 * The check would succeed if minimum <= value <= maximum.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @see check_expected_float().
 */
void expect_float_in_range(#function, #parameter, double minimum, double maximum, double epsilon);
#else
#define expect_float_in_range(function, parameter, minimum, maximum, epsilon) \
    expect_float_in_range_count(function, parameter, minimum, maximum, epsilon, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check a parameter is inside a
 * numerical range. The check would succeed if minimum <= value <= maximum.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected_float(
 */
void expect_float_in_range_count(#function, #parameter, double minimum, double maximum, double epsilon, size_t count);
#else
#define expect_float_in_range_count(function, parameter, minimum, maximum, epsilon, count) \
    _expect_float_in_range(cmocka_tostring(function),           \
                           cmocka_tostring(parameter),          \
                           __FILE__,                            \
                           __LINE__,                            \
                           cast_to_long_double_type(minimum),   \
                           cast_to_long_double_type(maximum),   \
                           cast_to_long_double_type(epsilon),   \
                           count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check a parameter is outside a numerical range.
 * The check would succeed if minimum > value > maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @see check_expected().
 */
void expect_float_not_in_range(#function, #parameter, double minimum, double maximum, double epsilon);
#else
#define expect_float_not_in_range(function, parameter, minimum, maximum, epsilon) \
    expect_float_not_in_range_count(function, parameter, minimum, maximum, epsilon, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check a parameter is outside a
 * numerical range. The check would succeed if minimum > value > maximum.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  minimum  The lower boundary of the interval to check against.
 *
 * @param[in]  maximum  The upper boundary of the interval to check against.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_float_not_in_range_count(#function, #parameter, double minimum, double maximum, double epsilon, size_t count);
#else
#define expect_float_not_in_range_count(function, parameter, minimum, maximum, \
                                  epsilon, count) \
    _expect_float_not_in_range(cmocka_tostring(function),           \
                               cmocka_tostring(parameter),          \
                               __FILE__,                            \
                               __LINE__,                            \
                               cast_to_long_double_type(minimum),   \
                               cast_to_long_double_type(maximum),   \
                               cast_to_long_double_type(epsilon),   \
                               count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if a parameter is the given integer based value.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @see check_expected()
 * @see expect_string()
 * @see expect_memory()
 * @see expect_any()
 */
void expect_value(#function, #parameter, uintmax_t value);
#else
#define expect_value(function, parameter, value) \
    expect_value_count(function, parameter, value, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if a parameter is the given integer
 * based value.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 * @see expect_not_string()
 * @see expect_not_memory()
 */
void expect_value_count(#function, #parameter, uintmax_t value, size_t count);
#else
#define expect_value_count(function, parameter, value, count) \
    _expect_value(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                  cast_to_uintmax_type(value), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if a parameter isn't the given value.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @see check_expected().
 */
void expect_not_value(#function, #parameter, uintmax_t value);
#else
#define expect_not_value(function, parameter, value) \
    expect_not_value_count(function, parameter, value, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if a parameter isn't the given value.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_not_value_count(#function, #parameter, uintmax_t value, size_t count);
#else
#define expect_not_value_count(function, parameter, value, count) \
    _expect_not_value(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                      cast_to_uintmax_type(value), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if a parameter is the given floating point value.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @see check_expected_float()
 * @see expect_string()
 * @see expect_memory()
 * @see expect_any()
 */
void expect_float(#function, #parameter, double value, double epsilon);
#else
#define expect_float(function, parameter, value, epsilon) \
    expect_float_count(function, parameter, cast_to_long_double_type(value), \
                       cast_to_long_double_type(epsilon), 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if a parameter is the given floating
 * point value.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected_float().
 * @see expect_not_string()
 * @see expect_not_memory()
 */
void expect_float_count(#function, #parameter, double value, double epsilon, size_t count);
#else
#define expect_float_count(function, parameter, value, epsilon, count) \
    _expect_float(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                  cast_to_long_double_type(value), cast_to_long_double_type(epsilon), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if a parameter isn't the given floating point
 * value.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @see check_expected_float()
 * @see expect_string()
 * @see expect_memory()
 * @see expect_any()
 */
void expect_not_float(#function, #parameter, double value, double epsilon);
#else
#define expect_not_float(function, parameter, epsilon, value) \
    expect_not_float_count(function, parameter, cast_to_long_double_type(value), \
                           cast_to_long_double_type(epsilon), 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if a parameter isn't the floating
 * point value.
 *
 * The event is triggered by calling check_expected_float() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected_float().
 * @see expect_not_string()
 * @see expect_not_memory()
 */
void expect_not_float_count(#function, #parameter, double value, double epsilon, size_t count);
#else
#define expect_not_float_count(function, parameter, value, epsilon, count) \
    _expect_not_float(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                      cast_to_long_double_type(value), cast_to_long_double_type(epsilon), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is equal to the
 *        provided string.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  string   The string value to compare.
 *
 * @see check_expected().
 */
void expect_string(#function, #parameter, const char *string);
#else
#define expect_string(function, parameter, string) \
    expect_string_count(function, parameter, string, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value is equal to the
 *        provided string.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  string   The string value to compare.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_string_count(#function, #parameter, const char *string, size_t count);
#else
#define expect_string_count(function, parameter, string, count) \
    _expect_string(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                   (const char*)(string), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value isn't equal to the
 *        provided string.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  string   The string value to compare.
 *
 * @see check_expected().
 */
void expect_not_string(#function, #parameter, const char *string);
#else
#define expect_not_string(function, parameter, string) \
    expect_not_string_count(function, parameter, string, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter value isn't equal to the
 *        provided string.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  string   The string value to compare.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_not_string_count(#function, #parameter, const char *string, size_t count);
#else
#define expect_not_string_count(function, parameter, string, count) \
    _expect_not_string(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                       (const char*)(string), count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter does match an area of memory.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  memory  The memory to compare.
 *
 * @param[in]  size  The size of the memory to compare.
 *
 * @see check_expected().
 */
void expect_memory(#function, #parameter, void *memory, size_t size);
#else
#define expect_memory(function, parameter, memory, size) \
    expect_memory_count(function, parameter, memory, size, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if the parameter does match an area
 *        of memory.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  memory  The memory to compare.
 *
 * @param[in]  size  The size of the memory to compare.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_memory_count(#function, #parameter, void *memory, size_t size, size_t count);
#else
#define expect_memory_count(function, parameter, memory, size, count) \
    _expect_memory(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                   (const void*)(memory), size, count)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to check if the parameter doesn't match an area of
 *        memory.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  memory  The memory to compare.
 *
 * @param[in]  size  The size of the memory to compare.
 *
 * @see check_expected().
 */
void expect_not_memory(#function, #parameter, void *memory, size_t size);
#else
#define expect_not_memory(function, parameter, memory, size) \
    expect_not_memory_count(function, parameter, memory, size, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if the parameter doesn't match an
 *        area of memory.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  memory  The memory to compare.
 *
 * @param[in]  size  The size of the memory to compare.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_not_memory_count(#function, #parameter, void *memory, size_t size, size_t count);
#else
#define expect_not_memory_count(function, parameter, memory, size, count) \
    _expect_not_memory(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, \
                       (const void*)(memory), size, count)
#endif


#ifdef DOXYGEN
/**
 * @brief Add an event to check if a parameter (of any value) has been passed.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @see check_expected().
 */
void expect_any(#function, #parameter);
#else
#define expect_any(function, parameter) \
    expect_any_count(function, parameter, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to always check if a parameter (of any value) has been passed.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @see check_expected().
 */
void expect_any_always(#function, #parameter);
#else
#define expect_any_always(function, parameter) \
        expect_any_count(function, parameter, WILL_RETURN_ALWAYS)
#endif

#ifdef DOXYGEN
/**
 * @brief Add an event to repeatedly check if a parameter (of any value) has
 *        been passed.
 *
 * The event is triggered by calling check_expected() in the mocked function.
 *
 * @param[in]  #function  The function to add the check for.
 *
 * @param[in]  #parameter The name of the parameter passed to the function.
 *
 * @param[in]  count  The count parameter returns the number of times the value
 *                    should be returned by check_expected(). If count is set
 *                    to -1 the value will always be returned.
 *
 * @see check_expected().
 */
void expect_any_count(#function, #parameter, size_t count);
#else
#define expect_any_count(function, parameter, count) \
    _expect_any(cmocka_tostring(function), cmocka_tostring(parameter), __FILE__, __LINE__, count)
#endif

#ifdef DOXYGEN
/**
 * @brief Determine whether a function parameter is correct.
 *
 * This ensures the next value queued by one of the expect_*() macros matches
 * the specified variable.
 *
 * This function needs to be called in the mock object.
 *
 * @param[in]  #parameter  The parameter to check.
 */
void check_expected(#parameter);
#else
#define check_expected(parameter) \
    _check_expected(__func__, #parameter, __FILE__, __LINE__, \
                    cast_int_to_cmocka_value(parameter))
#endif

#ifdef DOXYGEN
/**
 * @brief Determine whether a function parameter is correct.
 *
 * This ensures the next value queued by one of the expect_*() macros matches
 * the specified variable.
 *
 * This function needs to be called in the mock object.
 *
 * @param[in]  #parameter  The pointer to check.
 */
void check_expected_ptr(#parameter);
#else
#define check_expected_ptr(parameter) \
    _check_expected(__func__, #parameter, __FILE__, __LINE__, \
                    cast_ptr_to_cmocka_value(parameter))
#endif

#ifdef DOXYGEN
/**
 * @brief Determine whether a function parameter is correct.
 *
 * This ensures the next value queued by one of the expect*_float() macros matches
 * the specified variable.
 *
 * This function needs to be called in the mock object.
 *
 * @param[in]  #parameter  The parameter to check.
 *
 * @see expect_float
 * @see expect_not_float
 * @see expect_float_count
 * @see expect_not_float_count
 */
void check_expected_float(#parameter);
#else
#define check_expected_float(parameter) \
    _check_expected(__func__, #parameter, __FILE__, __LINE__, \
                    assign_double_to_cmocka_value(parameter))
#endif

/** @} */

/**
 * @defgroup cmocka_asserts Assert Macros
 * @ingroup cmocka
 *
 * This is a set of useful assert macros like the standard C libary's
 * assert(3) macro.
 *
 * On an assertion failure a cmocka assert macro will write the failure to the
 * standard error stream and signal a test failure. Due to limitations of the C
 * language the general C standard library assert() and cmocka's assert_true()
 * and assert_false() macros can only display the expression that caused the
 * assert failure. cmocka's type specific assert macros, assert_{type}_equal()
 * and assert_{type}_not_equal(), display the data that caused the assertion
 * failure which increases data visibility aiding debugging of failing test
 * cases.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Assert that the given expression is true.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if expression is false (i.e., compares equal to
 * zero).
 *
 * @param[in]  expression  The expression to evaluate.
 *
 * @see assert_int_equal()
 * @see assert_string_equal()
 */
void assert_true(scalar expression);
#else
#define assert_true(c) _assert_true(cast_to_uintmax_type(c), #c, \
                                    __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the given expression is false.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if expression is true.
 *
 * @param[in]  expression  The expression to evaluate.
 *
 * @see assert_int_equal()
 * @see assert_string_equal()
 */
void assert_false(scalar expression);
#else
#define assert_false(c) _assert_false(cast_to_uintmax_type(c), #c, \
                                      __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the return_code is greater than or equal to 0.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the return code is smaller than 0. If the function
 * you check sets an errno if it fails you can pass it to the function and
 * it will be printed as part of the error message.
 *
 * @param[in]  rc       The return code to evaluate.
 *
 * @param[in]  error    Pass errno here or 0.
 */
void assert_return_code(intmax_t rc, int32_t error);
#else
#define assert_return_code(rc, error) \
    _assert_return_code((rc), \
                        (error), \
                        #rc, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the given pointer is non-NULL.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the pointer is NULL.
 *
 * @param[in]  pointer  The pointer to evaluate.
 *
 * @see assert_null()
 */
void assert_non_null(void *pointer);
#else
#define assert_non_null(c) assert_ptr_not_equal((c), NULL)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the given pointer is non-NULL.
 *
 * The function prints an error message extended by message to standard error
 * and terminates the test by calling fail() if the pointer is NULL.
 *
 * @param[in]  pointer  The pointer to evaluate.
 *
 * @param[in]  message  The message to print when the pointer is NULL.
 *
 * @see assert_null_msg()
 */
void assert_non_null_msg(void *pointer, const char *const message);
#else
#define assert_non_null_msg(c, msg) assert_ptr_not_equal_msg((c), NULL, (msg))
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the given pointer is NULL.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the pointer is non-NULL.
 *
 * @param[in]  pointer  The pointer to evaluate.
 *
 * @see assert_non_null()
 */
void assert_null(void *pointer);
#else
#define assert_null(c) assert_ptr_equal((c), NULL)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the given pointer is NULL.
 *
 * The function prints an error message extended by message to standard error
 * and terminates the test by calling fail() if the pointer is non-NULL.
 *
 * @param[in]  pointer  The pointer to evaluate.
 *
 * @param[in]  message  The message to print when the pointer is not NULL.
 *
 * @see assert_non_null_msg()
 */
void assert_null_msg(void *pointer, const char *const message);
#else
#define assert_null_msg(c, msg) assert_ptr_equal_msg((c), NULL, (msg))
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given pointers are equal.
 *
 * The function prints an error message and terminates the test by calling
 * fail() if the pointers are not equal.
 *
 * @param[in]  a        The first pointer to compare.
 *
 * @param[in]  b        The pointer to compare against the first one.
 */
void assert_ptr_equal(void *a, void *b);
#else
#define assert_ptr_equal(a, b) assert_ptr_equal_msg((a), (b), NULL)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given pointers are equal.
 *
 * The function prints the failing comparison and the error message given by the user
 * and then terminates the test by calling fail() if the pointers are not equal.
 *
 * @param[in]  a        The first pointer to compare.
 *
 * @param[in]  b        The pointer to compare against the first one.
 *
 * @param[in]  msg      The error message to print when a & b are not equal.
 */
void assert_ptr_equal_msg(void *a, void *b, const char *const msg);
#else
#define assert_ptr_equal_msg(a, b, msg) \
    _assert_ptr_equal_msg((a), (b), __FILE__, __LINE__, (msg))
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given pointers are not equal.
 *
 * The function prints an error message and terminates the test by calling
 * fail() if the pointers are equal.
 *
 * @param[in]  a        The first pointer to compare.
 *
 * @param[in]  b        The pointer to compare against the first one.
 */
void assert_ptr_not_equal(void *a, void *b);
#else
#define assert_ptr_not_equal(a, b) \
    assert_ptr_not_equal_msg((a), (b), NULL)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given pointers are not equal.
 *
 * The function prints the failing comparison and the error message given by the user
 * and then terminates the test by calling fail() if the pointers are equal.
 *
 * @param[in]  a        The first pointer to compare.
 *
 * @param[in]  b        The pointer to compare against the first one.
 *
 * @param[in]  msg      The error message to print when a & b are equal.
 */
void assert_ptr_not_equal_msg(void *a, void *b, const char *const msg);
#else
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
#define assert_ptr_not_equal_msg(a, b, msg) \
    do { const void *p1 = (a), *p2 = (b); \
	 _assert_ptr_not_equal_msg((a), (b), __FILE__, __LINE__, (msg)); \
	 if (p1 == p2) __builtin_unreachable(); } while(0)
#endif
#else
#define assert_ptr_not_equal_msg(a, b, msg) \
    _assert_ptr_not_equal_msg((a), (b), __FILE__, __LINE__, (msg))
#endif
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given integers are equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the integers are not equal.
 *
 * @param[in]  a  The first integer to compare.
 *
 * @param[in]  b  The integer to compare against the first one.
 */
void assert_int_equal(intmax_t a, intmax_t b);
#else
#define assert_int_equal(a, b) \
    _assert_int_equal(cast_to_intmax_type(a), \
                      cast_to_intmax_type(b), \
                      __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given unsigned integers are equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the integers are not equal.
 *
 * @param[in]  a  The first unsigned integer to compare.
 *
 * @param[in]  b  The unsigned integer to compare against the first one.
 */
void assert_uint_equal(uintmax_t a, uintmax_t b);
#else
#define assert_uint_equal(a, b) \
    _assert_uint_equal(cast_to_uintmax_type(a), \
                       cast_to_uintmax_type(b), \
                      __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given integers are not equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the integers are equal.
 *
 * @param[in]  a  The first integer to compare.
 *
 * @param[in]  b  The integer to compare against the first one.
 *
 * @see assert_int_equal()
 */
void assert_int_not_equal(intmax_t a, intmax_t b);
#else
#define assert_int_not_equal(a, b) \
    _assert_int_not_equal(cast_to_intmax_type(a), \
                          cast_to_intmax_type(b), \
                          __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given unsigned integers are not equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the integers are not equal.
 *
 * @param[in]  a  The first unsigned integer to compare.
 *
 * @param[in]  b  The unsigned integer to compare against the first one.
 */
void assert_uint_not_equal(uintmax_t a, uintmax_t b);
#else
#define assert_uint_not_equal(a, b) \
    _assert_uint_not_equal(cast_to_uintmax_type(a), \
                           cast_to_uintmax_type(b), \
                           __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given float are equal given an epsilon.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the float are not equal (given an epsilon).
 *
 * @param[in]  a        The first float to compare.
 *
 * @param[in]  b        The float to compare against the first one.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_equal(float a, float b, float epsilon);
#else
#define assert_float_equal(a, b, epsilon) \
	_assert_float_equal((float)a, \
			(float)b, \
			(float)epsilon, \
			__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given float are not equal given an epsilon.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the float are not equal (given an epsilon).
 *
 * @param[in]  a        The first float to compare.
 *
 * @param[in]  b        The float to compare against the first one.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_not_equal(float a, float b, float epsilon);
#else
#define assert_float_not_equal(a, b, epsilon) \
	_assert_float_not_equal((float)a, \
			(float)b, \
			(float)epsilon, \
			__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given double are equal given an epsilon.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the double are not equal (given an epsilon).
 *
 * @param[in]  a        The first double to compare.
 *
 * @param[in]  b        The double to compare against the first one.
 *
 * @param[in]  epsilon  The epsilon used as margin for double comparison.
 */
void assert_double_equal(double a, double b, double epsilon);
#else
#define assert_double_equal(a, b, epsilon) \
	_assert_double_equal((double)a, \
			(double)b, \
			(double)epsilon, \
			__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given double are not equal given an epsilon.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the double are not equal (given an epsilon).
 *
 * @param[in]  a        The first double to compare.
 *
 * @param[in]  b        The double to compare against the first one.
 *
 * @param[in]  epsilon  The epsilon used as margin for double comparison.
 */
void assert_double_not_equal(double a, double b, double epsilon);
#else
#define assert_double_not_equal(a, b, epsilon) \
	_assert_double_not_equal((double)a, \
			(double)b, \
			(double)epsilon, \
			__FILE__, __LINE__)
#endif


#ifdef DOXYGEN
/**
 * @brief Assert that the two given strings are equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the strings are not equal.
 *
 * @param[in]  a  The string to check.
 *
 * @param[in]  b  The other string to compare.
 */
void assert_string_equal(const char *a, const char *b);
#else
#define assert_string_equal(a, b) \
    _assert_string_equal((a), (b), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given strings are not equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the strings are equal.
 *
 * @param[in]  a  The string to check.
 *
 * @param[in]  b  The other string to compare.
 */
void assert_string_not_equal(const char *a, const char *b);
#else
#define assert_string_not_equal(a, b) \
    _assert_string_not_equal((a), (b), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given areas of memory are equal, otherwise fail.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the memory is not equal.
 *
 * @param[in]  a  The first memory area to compare
 *                (interpreted as unsigned char).
 *
 * @param[in]  b  The second memory area to compare
 *                (interpreted as unsigned char).
 *
 * @param[in]  size  The first n bytes of the memory areas to compare.
 */
void assert_memory_equal(const void *a, const void *b, size_t size);
#else
#define assert_memory_equal(a, b, size) \
    _assert_memory_equal((const void*)(a), (const void*)(b), size, __FILE__, \
                         __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the two given areas of memory are not equal.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if the memory is equal.
 *
 * @param[in]  a  The first memory area to compare
 *                (interpreted as unsigned char).
 *
 * @param[in]  b  The second memory area to compare
 *                (interpreted as unsigned char).
 *
 * @param[in]  size  The first n bytes of the memory areas to compare.
 */
void assert_memory_not_equal(const void *a, const void *b, size_t size);
#else
#define assert_memory_not_equal(a, b, size) \
    _assert_memory_not_equal((const void*)(a), (const void*)(b), size, \
                             __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified integer value is not smaller than the
 * minimum and and not greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 */
void assert_int_in_range(intmax_t value, intmax_t minimum, intmax_t maximum);
#else
#define assert_int_in_range(value, minimum, maximum) \
    _assert_int_in_range( \
        cast_to_intmax_type(value), \
        cast_to_intmax_type(minimum), \
        cast_to_intmax_type(maximum), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified unsigned integer value is not smaller than
 * the minimum and and not greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 */
void assert_uint_in_range(uintmax_t value, uintmax_t minimum, uintmax_t maximum);
#else
#define assert_uint_in_range(value, minimum, maximum) \
    _assert_uint_in_range( \
        cast_to_intmax_type(value), \
        cast_to_intmax_type(minimum), \
        cast_to_intmax_type(maximum), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified value is smaller than the minimum or
 * greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is in range.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 */
void assert_int_not_in_range(intmax_t value,
                             intmax_t minimum,
                             intmax_t maximum);
#else
#define assert_int_not_in_range(value, minimum, maximum)   \
    _assert_int_not_in_range(cast_to_intmax_type(value),   \
                             cast_to_intmax_type(minimum), \
                             cast_to_intmax_type(maximum), \
                             __FILE__,                     \
                             __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified value is smaller than the minimum or
 * greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is in range.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 */
void assert_uint_not_in_range(uintmax_t value,
                             uintmax_t minimum,
                             uintmax_t maximum);
#else
#define assert_uint_not_in_range(value, minimum, maximum)   \
    _assert_uint_not_in_range(cast_to_uintmax_type(value),   \
                             cast_to_uintmax_type(minimum), \
                             cast_to_uintmax_type(maximum), \
                             __FILE__,                     \
                             __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @deprecated Use assert_int_in_range() and assert_uint_in_range()
 */
void assert_in_range(uintmax_t value, uintmax_t minimum, uintmax_t maximum);
#else
#define assert_in_range(value, minimum, maximum) \
    _assert_uint_in_range( \
        cast_to_uintmax_type(value), \
        cast_to_uintmax_type(minimum), \
        cast_to_uintmax_type(maximum), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @deprecated Use assert_int_not_in_range() or assert_uint_not_in_range()
 */
void assert_not_in_range(uintmax_t value, uintmax_t minimum, uintmax_t maximum);
#else
#define assert_not_in_range(value, minimum, maximum) \
    _assert_uint_not_in_range( \
        cast_to_uintmax_type(value), \
        cast_to_uintmax_type(minimum), \
        cast_to_uintmax_type(maximum), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified float value is smaller than the minimum or
 * greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is in range.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_not_in_range(double value, double minimum, double maximum, double epsilon);
#else
#define assert_float_not_in_range(value, minimum, maximum, epsilon)   \
    _assert_float_not_in_range(cast_to_long_double_type(value),       \
                               cast_to_long_double_type(minimum),     \
                               cast_to_long_double_type(maximum),     \
                               cast_to_long_double_type(epsilon),     \
                               __FILE__,                              \
                               __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified float value is not smaller than
 * the minimum and and not greater than the maximum.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not in range.
 *
 * @param[in]  value  The value to check.
 *
 * @param[in]  minimum  The minimum value allowed.
 *
 * @param[in]  maximum  The maximum value allowed.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_in_range(double value, double minimum, double maximum, double epsilon);
#else
#define assert_float_in_range(value, minimum, maximum, epsilon) \
    _assert_float_in_range(                                     \
        cast_to_long_double_type(value),                        \
        cast_to_long_double_type(minimum),                      \
        cast_to_long_double_type(maximum),                      \
        cast_to_long_double_type(epsilon), __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @deprecated Use assert_int_in_set() or assert_uint_in_set()
 */
void assert_in_set(uintmax_t value, uintmax_t values[], size_t count);
#else
#define assert_in_set(value, values, number_of_values) \
    _assert_uint_in_set(value, values, number_of_values, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified value is not within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 */
void assert_not_in_set(uintmax_t value, uintmax_t values[], size_t count);
#else
#define assert_not_in_set(value, values, number_of_values) \
    _assert_not_in_set(value, values, number_of_values, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified integer value is within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 */
void assert_int_in_set(intmax_t value, intmax_t values[], size_t count);
#else
#define assert_int_in_set(value, values, number_of_values) \
    if (number_of_values > 0) { \
        intmax_t _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_int_in_set(value, _cmocka_set, number_of_values, __FILE__, __LINE__); \
    }
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified value is not within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 */
void assert_int_not_in_set(intmax_t value, intmax_t values[], size_t count);
#else
#define assert_int_not_in_set(value, values, number_of_values) \
    if (number_of_values > 0) { \
        intmax_t _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_int_not_in_set(value, _cmocka_set, number_of_values, __FILE__, __LINE__); \
    }
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified unsigned integer value is within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 */
void assert_uint_in_set(uintmax_t value, uintmax_t values[], size_t count);
#else
#define assert_uint_in_set(value, values, number_of_values) \
    if (number_of_values > 0) { \
        uintmax_t _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_uint_in_set(value, _cmocka_set, number_of_values, __FILE__, __LINE__); \
    }
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified unsigned integer value is not within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 */
void assert_uint_not_in_set(uintmax_t value, uintmax_t values[], size_t count);
#else
#define assert_uint_not_in_set(value, values, number_of_values) \
    if (number_of_values > 0) { \
        uintmax_t _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_uint_not_in_set(value, _cmocka_set, number_of_values, __FILE__, __LINE__); \
    }
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified float value is within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_in_set(double value, double values[], size_t count, double epsilon);
#else
#define assert_float_in_set(value, values, number_of_values, epsilon) \
    if (number_of_values > 0) { \
        double _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_float_in_set(value, _cmocka_set, number_of_values, epsilon, __FILE__, __LINE__); \
    }
#endif

#ifdef DOXYGEN
/**
 * @brief Assert that the specified float value is not within a set.
 *
 * The function prints an error message to standard error and terminates the
 * test by calling fail() if value is not within a set.
 *
 * @param[in]  value  The value to look up
 *
 * @param[in]  values[]  The array to check for the value.
 *
 * @param[in]  count  The size of the values array.
 *
 * @param[in]  epsilon  The epsilon used as margin for float comparison.
 */
void assert_float_not_in_set(double value, double values[], size_t count, double epsilon);
#else
#define assert_float_not_in_set(value, values, number_of_values, epsilon) \
    if (number_of_values > 0) { \
        double _cmocka_set[number_of_values]; \
        for (size_t _i = 0; _i < number_of_values; _i++) { \
            _cmocka_set[_i] = values[_i]; \
        } \
        _assert_float_not_in_set(value, _cmocka_set, number_of_values, epsilon, __FILE__, __LINE__); \
    }
#endif

/** @} */

/**
 * @defgroup cmocka_call_order Call Ordering
 * @ingroup cmocka
 *
 * It is often beneficial to  make sure that functions are called in an
 * order. This is independent of mock returns and parameter checking as both
 * of the aforementioned do not check the order in which they are called from
 * different functions.
 *
 * <ul>
 * <li><strong>expect_function_call(function)</strong> - The
 * expect_function_call() macro pushes an expectation onto the stack of
 * expected calls.</li>
 *
 * <li><strong>function_called()</strong> - pops a value from the stack of
 * expected calls. function_called() is invoked within the mock object
 * that uses it.
 * </ul>
 *
 * expect_function_call() and function_called() are intended to be used in
 * pairs. Cmocka will fail a test if there are more or less expected calls
 * created (e.g. expect_function_call()) than consumed with function_called().
 * There are provisions such as ignore_function_calls() which allow this
 * restriction to be circumvented in tests where mock calls for the code under
 * test are not the focus of the test. function_called() must be called from
 * the same thread as expect_function_call(), and that thread must have been
 * initialized for use by cmocka (see also the [Threading section of the main
 * documentation page](index.html#main-threads)).
 *
 * The following example illustrates how a unit test instructs cmocka
 * to expect a function_called() from a particular mock,
 * <strong>chef_sing()</strong>:
 *
 * @code
 * void chef_sing(void);
 *
 * void code_under_test()
 * {
 *   chef_sing();
 * }
 *
 * void some_test(void **state)
 * {
 *     expect_function_call(chef_sing);
 *     code_under_test();
 * }
 * @endcode
 *
 * The implementation of the mock then must check whether it was meant to
 * be called by invoking <strong>function_called()</strong>:
 *
 * @code
 * void chef_sing()
 * {
 *     function_called();
 * }
 * @endcode
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Check that current mocked function is being called in the expected
 *        order
 *
 * @see expect_function_call()
 */
void function_called(void);
#else
#define function_called() _function_called(__func__, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Store expected call(s) to a mock to be checked by function_called()
 *        later.
 *
 * @param[in]  #function  The function which should should be called
 *
 * @param[in]  times number of times this mock must be called
 *
 * @see function_called()
 */
void expect_function_calls(#function, const int times);
#else
#define expect_function_calls(function, times) \
    _expect_function_call(cmocka_tostring(function), __FILE__, __LINE__, times)
#endif

#ifdef DOXYGEN
/**
 * @brief Store expected single call to a mock to be checked by
 *        function_called() later.
 *
 * @param[in]  #function  The function which should should be called
 *
 * @see function_called()
 */
void expect_function_call(#function);
#else
#define expect_function_call(function) \
    _expect_function_call(cmocka_tostring(function), __FILE__, __LINE__, 1)
#endif

#ifdef DOXYGEN
/**
 * @brief Expects function_called() from given mock at least once
 *
 * @param[in]  #function  The function which should should be called
 *
 * @see function_called()
 */
void expect_function_call_any(#function);
#else
#define expect_function_call_any(function) \
    _expect_function_call(cmocka_tostring(function), __FILE__, __LINE__, -1)
#endif

#ifdef DOXYGEN
/**
 * @brief Ignores function_called() invocations from given mock function.
 *
 * @param[in]  #function  The function which should should be called
 *
 * @see function_called()
 */
void ignore_function_calls(#function);
#else
#define ignore_function_calls(function) \
    _expect_function_call(cmocka_tostring(function), __FILE__, __LINE__, -2)
#endif

/** @} */

/**
 * @defgroup cmocka_exec Running Tests
 * @ingroup cmocka
 *
 * This is the way tests are executed with CMocka.
 *
 * The following example illustrates this macro's use with the unit_test macro.
 *
 * @code
 * void Test0(void **state);
 * void Test1(void **state);
 *
 * int main(void)
 * {
 *     const struct CMUnitTest tests[] = {
 *         cmocka_unit_test(Test0),
 *         cmocka_unit_test(Test1),
 *     };
 *
 *     return cmocka_run_group_tests(tests, NULL, NULL);
 * }
 * @endcode
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Forces the test to fail immediately and quit.
 */
void fail(void);
#else
#define fail() _fail(__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Forces the test to not be executed, but marked as skipped.
 */
void skip(void);
#else
#define skip() _skip(__FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Forces the test to be stopped immediately.
 *
 * Call stop() to stop a running test.
 * The test is considered passed if there are no leftover values, otherwise a test failure
 * is signaled.
 * Calling stop() is especially useful in mocked functions that do not return, e.g reset the CPU.
 */
void stop(void);
#else
#define stop() _stop()
#endif

#ifdef DOXYGEN
/**
 * @brief Forces the test to fail immediately and quit, printing the reason.
 *
 * @code
 * fail_msg("This is some error message for test");
 * @endcode
 *
 * or
 *
 * @code
 * char *error_msg = "This is some error message for test";
 * fail_msg("%s", error_msg);
 * @endcode
 */
void fail_msg(const char *msg, ...);
#else
#define fail_msg(msg, ...) do { \
    cmocka_print_error("ERROR: " msg "\n", ##__VA_ARGS__); \
    fail(); \
} while (0)
#endif

static inline void _unit_test_dummy(void **state) {
    (void)state;
}

/** Initializes a UnitTest structure.
 *
 * @deprecated This function was deprecated in favor of cmocka_unit_test
 */
#define unit_test(f) { #f, f, UNIT_TEST_FUNCTION_TYPE_TEST }

#define _unit_test_setup(test, setup) \
    { #test "_" #setup, setup, UNIT_TEST_FUNCTION_TYPE_SETUP }

/** Initializes a UnitTest structure with a setup function.
 *
 * @deprecated This function was deprecated in favor of cmocka_unit_test_setup
 */
#define unit_test_setup(test, setup) \
    _unit_test_setup(test, setup), \
    unit_test(test), \
    _unit_test_teardown(test, _unit_test_dummy)

#define _unit_test_teardown(test, teardown) \
    { #test "_" #teardown, teardown, UNIT_TEST_FUNCTION_TYPE_TEARDOWN }

/** Initializes a UnitTest structure with a teardown function.
 *
 * @deprecated This function was deprecated in favor of cmocka_unit_test_teardown
 */
#define unit_test_teardown(test, teardown) \
    _unit_test_setup(test, _unit_test_dummy), \
    unit_test(test), \
    _unit_test_teardown(test, teardown)

/** Initializes a UnitTest structure for a group setup function.
 *
 * @deprecated This function was deprecated in favor of cmocka_run_group_tests
 */
#define group_test_setup(setup) \
    { "group_" #setup, setup, UNIT_TEST_FUNCTION_TYPE_GROUP_SETUP }

/** Initializes a UnitTest structure for a group teardown function.
 *
 * @deprecated This function was deprecated in favor of cmocka_run_group_tests
 */
#define group_test_teardown(teardown) \
    { "group_" #teardown, teardown, UNIT_TEST_FUNCTION_TYPE_GROUP_TEARDOWN }

/**
 * Initialize an array of UnitTest structures with a setup function for a test
 * and a teardown function.  Either setup or teardown can be NULL.
 *
 * @deprecated This function was deprecated in favor of
 * cmocka_unit_test_setup_teardown
 */
#define unit_test_setup_teardown(test, setup, teardown) \
    _unit_test_setup(test, setup), \
    unit_test(test), \
    _unit_test_teardown(test, teardown)


/** Initializes a CMUnitTest structure. */
#define cmocka_unit_test(f) { #f, f, NULL, NULL, NULL }

/** Initializes a CMUnitTest structure with a setup function. */
#define cmocka_unit_test_setup(f, setup) { #f, f, setup, NULL, NULL }

/** Initializes a CMUnitTest structure with a teardown function. */
#define cmocka_unit_test_teardown(f, teardown) { #f, f, NULL, teardown, NULL }

/**
 * Initialize an array of CMUnitTest structures with a setup function for a test
 * and a teardown function. Either setup or teardown can be NULL.
 */
#define cmocka_unit_test_setup_teardown(f, setup, teardown) { #f, f, setup, teardown, NULL }

/**
 * Initialize a CMUnitTest structure with given initial state. It will be passed
 * to test function as an argument later. It can be used when test state does
 * not need special initialization or was initialized already.
 * @note If the group setup function initialized the state already, it won't be
 * overridden by the initial state defined here.
 */
#define cmocka_unit_test_prestate(f, state) { #f, f, NULL, NULL, state }

/**
 * Initialize a CMUnitTest structure with given initial state, setup and
 * teardown function. Any of these values can be NULL. Initial state is passed
 * later to setup function, or directly to test if none was given.
 * @note If the group setup function initialized the state already, it won't be
 * overridden by the initial state defined here.
 */
#define cmocka_unit_test_prestate_setup_teardown(f, setup, teardown, state) { #f, f, setup, teardown, state }

#ifdef DOXYGEN
/**
 * @brief Run tests specified by an array of CMUnitTest structures.
 *
 * @param[in]  group_tests[]  The array of unit tests to execute.
 *
 * @param[in]  group_setup    The setup function which should be called before
 *                            all unit tests are executed.
 *
 * @param[in]  group_teardown The teardown function to be called after all
 *                            tests have finished.
 *
 * @return 0 on success, or the number of failed tests.
 *
 * @code
 * static int setup(void **state) {
 *      int *answer = malloc(sizeof(int));
 *      if (answer == NULL) {
 *          return -1;
 *      }
 *      *answer = 42;
 *
 *      *state = answer;
 *
 *      return 0;
 * }
 *
 * static int teardown(void **state) {
 *      free(*state);
 *
 *      return 0;
 * }
 *
 * static void null_test_success(void **state) {
 *     (void) state;
 * }
 *
 * static void int_test_success(void **state) {
 *      int *answer = *state;
 *      assert_int_equal(*answer, 42);
 * }
 *
 * int main(void) {
 *     const struct CMUnitTest tests[] = {
 *         cmocka_unit_test(null_test_success),
 *         cmocka_unit_test_setup_teardown(int_test_success, setup, teardown),
 *     };
 *
 *     return cmocka_run_group_tests(tests, NULL, NULL);
 * }
 * @endcode
 *
 * @see cmocka_unit_test
 * @see cmocka_unit_test_setup
 * @see cmocka_unit_test_teardown
 * @see cmocka_unit_test_setup_teardown
 */
int cmocka_run_group_tests(const struct CMUnitTest group_tests[],
                           CMFixtureFunction group_setup,
                           CMFixtureFunction group_teardown);
#else
# define cmocka_run_group_tests(group_tests, group_setup, group_teardown) \
        _cmocka_run_group_tests(#group_tests, group_tests, sizeof(group_tests) / sizeof((group_tests)[0]), group_setup, group_teardown)
#endif

#ifdef DOXYGEN
/**
 * @brief Run tests specified by an array of CMUnitTest structures and specify
 *        a name.
 *
 * @param[in]  group_name     The name of the group test.
 *
 * @param[in]  group_tests[]  The array of unit tests to execute.
 *
 * @param[in]  group_setup    The setup function which should be called before
 *                            all unit tests are executed.
 *
 * @param[in]  group_teardown The teardown function to be called after all
 *                            tests have finished.
 *
 * @return 0 on success, or the number of failed tests.
 *
 * @code
 * static int setup(void **state) {
 *      int *answer = malloc(sizeof(int));
 *      if (answer == NULL) {
 *          return -1;
 *      }
 *      *answer = 42;
 *
 *      *state = answer;
 *
 *      return 0;
 * }
 *
 * static int teardown(void **state) {
 *      free(*state);
 *
 *      return 0;
 * }
 *
 * static void null_test_success(void **state) {
 *     (void) state;
 * }
 *
 * static void int_test_success(void **state) {
 *      int *answer = *state;
 *      assert_int_equal(*answer, 42);
 * }
 *
 * int main(void) {
 *     const struct CMUnitTest tests[] = {
 *         cmocka_unit_test(null_test_success),
 *         cmocka_unit_test_setup_teardown(int_test_success, setup, teardown),
 *     };
 *
 *     return cmocka_run_group_tests_name("success_test", tests, NULL, NULL);
 * }
 * @endcode
 *
 * @see cmocka_unit_test
 * @see cmocka_unit_test_setup
 * @see cmocka_unit_test_teardown
 * @see cmocka_unit_test_setup_teardown
 */
int cmocka_run_group_tests_name(const char *group_name,
                                const struct CMUnitTest group_tests[],
                                CMFixtureFunction group_setup,
                                CMFixtureFunction group_teardown);
#else
# define cmocka_run_group_tests_name(group_name, group_tests, group_setup, group_teardown) \
        _cmocka_run_group_tests(group_name, group_tests, sizeof(group_tests) / sizeof((group_tests)[0]), group_setup, group_teardown)
#endif

/** @} */

/**
 * @defgroup cmocka_alloc Dynamic Memory Allocation
 * @ingroup cmocka
 *
 * Memory leaks, buffer overflows and underflows can be checked using cmocka.
 *
 * To test for memory leaks, buffer overflows and underflows a module being
 * tested by cmocka should replace calls to malloc(), calloc() and free() to
 * test_malloc(), test_calloc() and test_free() respectively. Each time a block
 * is deallocated using test_free() it is checked for corruption, if a corrupt
 * block is found a test failure is signalled. All blocks allocated using the
 * test_*() allocation functions are tracked by the cmocka library. When a test
 * completes if any allocated blocks (memory leaks) remain they are reported
 * and a test failure is signalled.
 *
 * For simplicity cmocka currently executes all tests in one process. Therefore
 * all test cases in a test application share a single address space which
 * means memory corruption from a single test case could potentially cause the
 * test application to exit prematurely.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Test function overriding malloc.
 *
 * @param[in]  size  The bytes which should be allocated.
 *
 * @return A pointer to the allocated memory or NULL on error.
 *
 * @code
 * #ifdef UNIT_TESTING
 * extern void* _test_malloc(const size_t size, const char* file, const int line);
 *
 * #define malloc(size) _test_malloc(size, __FILE__, __LINE__)
 * #endif
 *
 * void leak_memory() {
 *     int * const temporary = (int*)malloc(sizeof(int));
 *     *temporary = 0;
 * }
 * @endcode
 *
 * @see malloc(3)
 */
void *test_malloc(size_t size);
#else
#define test_malloc(size) _test_malloc(size, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Test function overriding calloc.
 *
 * The memory is set to zero.
 *
 * @param[in]  nmemb  The number of elements for an array to be allocated.
 *
 * @param[in]  size   The size in bytes of each array element to allocate.
 *
 * @return A pointer to the allocated memory, NULL on error.
 *
 * @see calloc(3)
 */
void *test_calloc(size_t nmemb, size_t size);
#else
#define test_calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Test function overriding realloc which detects buffer overruns
 *        and memory leaks.
 *
 * @param[in]  ptr   The memory block which should be changed.
 *
 * @param[in]  size  The bytes which should be allocated.
 *
 * @return           The newly allocated memory block, NULL on error.
 */
void *test_realloc(void *ptr, size_t size);
#else
#define test_realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
#endif

#ifdef DOXYGEN
/**
 * @brief Test function overriding free(3).
 *
 * @param[in]  ptr  The pointer to the memory space to free.
 *
 * @see free(3).
 */
void test_free(void *ptr);
#else
#define test_free(ptr) _test_free(ptr, __FILE__, __LINE__)
#endif

/* Redirect malloc, calloc and free to the unit test allocators. */
#ifdef UNIT_TESTING
#define malloc test_malloc
#define realloc test_realloc
#define calloc test_calloc
#define free test_free
#endif /* UNIT_TESTING */

/** @} */


/**
 * @defgroup cmocka_mock_assert Standard Assertions
 * @ingroup cmocka
 *
 * How to handle assert(3) of the standard C library.
 *
 * Runtime assert macros like the standard C library's assert() should be
 * redefined in modules being tested to use cmocka's mock_assert() function.
 * Normally mock_assert() signals a test failure. If a function is called using
 * the expect_assert_failure() macro, any calls to mock_assert() within the
 * function will result in the execution of the test. If no calls to
 * mock_assert() occur during the function called via expect_assert_failure() a
 * test failure is signalled.
 *
 * @{
 */

/**
 * @brief Function to replace assert(3) in tested code.
 *
 * In conjunction with check_assert() it's possible to determine whether an
 * assert condition has failed without stopping a test.
 *
 * @param[in]  result  The expression to assert.
 *
 * @param[in]  expression  The expression as string.
 *
 * @param[in]  file  The file mock_assert() is called.
 *
 * @param[in]  line  The line mock_assert() is called.
 *
 * @code
 * #ifdef UNIT_TESTING
 * extern void mock_assert(const int result, const char* const expression,
 *                         const char * const file, const int line);
 *
 * #undef assert
 * #define assert(expression) \
 *     mock_assert((int)(expression), #expression, __FILE__, __LINE__);
 * #endif
 *
 * void increment_value(int * const value) {
 *     assert(value);
 *     (*value) ++;
 * }
 * @endcode
 *
 * @see assert(3)
 * @see expect_assert_failure
 */
void mock_assert(const int result, const char* const expression,
                 const char * const file, const int line);

#ifdef DOXYGEN
/**
 * @brief Ensure that mock_assert() is called.
 *
 * If mock_assert() is called the assert expression string is returned.
 *
 * @param[in]  fn_call  The function will will call mock_assert().
 *
 * @code
 * #define assert mock_assert
 *
 * void showmessage(const char *message) {
 *   assert(message);
 * }
 *
 * int main(int argc, const char* argv[]) {
 *   expect_assert_failure(show_message(NULL));
 *   printf("succeeded\n");
 *   return 0;
 * }
 * @endcode
 *
 */
void expect_assert_failure(function fn_call);
#else
#define expect_assert_failure(function_call) \
  { \
    global_expecting_assert = 1; \
    if (setjmp(global_expect_assert_env) != 0) { \
      print_message("Expected assertion %s occurred\n", \
                    global_last_failed_assert); \
      global_expecting_assert = 0; \
    } else { \
      function_call ; \
      global_expecting_assert = 0; \
      print_error("Expected assert in %s\n", #function_call); \
      _fail(__FILE__, __LINE__); \
    } \
  }
#endif

/** @} */

/**
 * CMocka value data type.
 *
 * Allows storing multiple types of values in CMocka functions without using
 * undefined behavior.
 */
typedef union {
    /** Holds signed integral types */
    intmax_t int_val;
    /** Holds integral types */
    uintmax_t uint_val;
    /** Holds real/floating-pointing types*/
    double real_val; // TODO: Should we use `long double` instead
    /** Holds pointer data */
    const void *ptr;
    // The following aren't used by CMocka currently, but are added to avoid
    // breaking ABI compatibility in the future
    /** Holds function pointer data */
    void *(*func)(void);
} CMockaValueData;

/* Function prototype for setup, test and teardown functions. */
typedef void (*UnitTestFunction)(void **state);

/* Function that determines whether a function parameter value is correct. */
typedef int (*CheckParameterValue)(const CMockaValueData value,
                                   const CMockaValueData check_value_data);

/* Function that determines whether a function parameter value is correct. */
typedef int (*CheckIntParameterValue)(const intmax_t value,
                                      const intmax_t check_value_data);

/* Function that determines whether a function parameter value is correct. */
typedef int (*CheckUintParameterValue)(const uintmax_t value,
                                       const uintmax_t check_value_data);

/* Type of the unit test function. */
typedef enum UnitTestFunctionType {
    UNIT_TEST_FUNCTION_TYPE_TEST = 0,
    UNIT_TEST_FUNCTION_TYPE_SETUP,
    UNIT_TEST_FUNCTION_TYPE_TEARDOWN,
    UNIT_TEST_FUNCTION_TYPE_GROUP_SETUP,
    UNIT_TEST_FUNCTION_TYPE_GROUP_TEARDOWN,
} UnitTestFunctionType;

/*
 * Stores a unit test function with its name and type.
 * NOTE: Every setup function must be paired with a teardown function.  It's
 * possible to specify NULL function pointers.
 */
typedef struct UnitTest {
    const char* name;
    UnitTestFunction function;
    UnitTestFunctionType function_type;
} UnitTest;

typedef struct GroupTest {
    UnitTestFunction setup;
    UnitTestFunction teardown;
    const UnitTest *tests;
    const size_t number_of_tests;
} GroupTest;

/* Function prototype for test functions. */
typedef void (*CMUnitTestFunction)(void **state);

/* Function prototype for setup and teardown functions. */
typedef int (*CMFixtureFunction)(void **state);

struct CMUnitTest {
    const char *name;
    CMUnitTestFunction test_func;
    CMFixtureFunction setup_func;
    CMFixtureFunction teardown_func;
    void *initial_state;
};

/* Location within some source code. */
typedef struct SourceLocation {
    const char* file;
    int line;
} SourceLocation;

/* Event that's called to check a parameter value. */
typedef struct CheckParameterEvent {
    SourceLocation location;
    const char *parameter_name;
    CheckParameterValue check_value;
    CMockaValueData check_value_data;
} CheckParameterEvent;

/* Used by expect_assert_failure() and mock_assert(). */
CMOCKA_DLLEXTERN extern int global_expecting_assert;
CMOCKA_DLLEXTERN extern jmp_buf global_expect_assert_env;
CMOCKA_DLLEXTERN extern const char * global_last_failed_assert;

/* Retrieves a value for the given function, as set by "will_return". */
CMockaValueData _mock(const char *const function,
                      const char *const file,
                      const int line,
                      const char *name);

CMockaValueData _mock_parameter(const char *const function,
                      const char *name,
                      const char *const file,
                      const int line,
                      const char *type);

void _expect_function_call(
    const char * const function_name,
    const char * const file,
    const int line,
    const int count);

void _function_called(const char * const function, const char* const file,
                          const int line);

void _expect_check(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const CheckParameterValue check_function,
    const CMockaValueData check_data, CheckParameterEvent * const event,
    const int count);

void _expect_int_in_set(const char *const function,
                        const char *const parameter,
                        const char *const file,
                        const size_t line,
                        const intmax_t values[],
                        const size_t number_of_values,
                        const size_t count);
void _expect_uint_in_set(const char *const function,
                         const char *const parameter,
                         const char *const file,
                         const size_t line,
                         const uintmax_t values[],
                         const size_t number_of_values,
                         const size_t count);

void _expect_float_in_set(const char *const function,
                          const char *const parameter,
                          const char *const file,
                          const size_t line,
                          const double values[],
                          const size_t number_of_values,
                          const double epsilon,
                          const size_t count);

void _expect_not_in_set(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const uintmax_t values[],
    const size_t number_of_values, const int count);

void _expect_float_not_in_set(
    const char* const function, const char* const parameter,
    const char* const file, const size_t line, const double values[],
    const size_t number_of_values, const double epsilon, const size_t count);

void _expect_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const uintmax_t minimum,
    const uintmax_t maximum, const int count);
void _expect_not_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const uintmax_t minimum,
    const uintmax_t maximum, const int count);
void _expect_float_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const double minimum, const double maximum, const double epsilon,
    const int count);
void _expect_float_not_in_range(
    const char* const function, const char* const parameter,
    const char* const file, const int line,
    const double minimum, const double maximum, const double epsilon,
    const int count);

void _expect_value(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const uintmax_t value,
    const int count);
void _expect_not_value(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const uintmax_t value,
    const int count);

void _expect_float(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const double value,
    const double epsilon, const int count);
void _expect_not_float(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const double value,
    const double epsilon, const int count);

void _expect_string(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const char* string,
    const int count);
void _expect_not_string(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const char* string,
    const int count);

void _expect_memory(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const void* const memory,
    const size_t size, const int count);
void _expect_not_memory(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const void* const memory,
    const size_t size, const int count);

void _expect_any(
    const char* const function, const char* const parameter,
    const char* const file, const int line, const int count);

void _check_expected(
    const char * const function_name, const char * const parameter_name,
    const char* file, const int line, const CMockaValueData value);

void _will_return(const char *const function_name,
                  const char *const file,
                  const int line,
                  const char *name,
                  const CMockaValueData value,
                  const int count);
void _will_set_parameter(const char *const function_name,
                  const char *name,
                  const char *const file,
                  const int line,
                  const char *type,
                  const CMockaValueData value,
                  const int count);
void _assert_true(const uintmax_t result,
                  const char* const expression,
                  const char * const file, const int line);
void _assert_false(const uintmax_t result,
                   const char * const expression,
                   const char * const file, const int line);
void _assert_return_code(const intmax_t result,
                         const int32_t error,
                         const char * const expression,
                         const char * const file,
                         const int line);
void _assert_float_equal(const float a, const float n,
		const float epsilon, const char* const file,
		const int line);
void _assert_float_not_equal(const float a, const float n,
		const float epsilon, const char* const file,
		const int line);
void _assert_double_equal(const double a, const double n,
		const double epsilon, const char* const file,
		const int line);
void _assert_double_not_equal(const double a, const double n,
		const double epsilon, const char* const file,
		const int line);
void _assert_int_equal(const intmax_t a,
                       const intmax_t b,
                       const char * const file,
                       const int line);
void _assert_int_not_equal(const intmax_t a,
                           const intmax_t b,
                           const char * const file,
                           const int line);
void _assert_uint_equal(const uintmax_t a,
                        const uintmax_t b,
                        const char * const file,
                        const int line);
void _assert_uint_not_equal(const uintmax_t a,
                            const uintmax_t b,
                            const char * const file,
                            const int line);
CMOCKA_NO_ACCESS_ATTRIBUTE
void _assert_ptr_equal_msg(const void *a,
                           const void *b,
                           const char *const file,
                           const int line,
                           const char *const msg);
CMOCKA_NO_ACCESS_ATTRIBUTE
void _assert_ptr_not_equal_msg(const void *a,
                               const void *b,
                               const char *const file,
                               const int line,
                               const char *const msg);
void _assert_string_equal(const char * const a, const char * const b,
                          const char * const file, const int line);
void _assert_string_not_equal(const char * const a, const char * const b,
                              const char *file, const int line);
void _assert_memory_equal(const void * const a, const void * const b,
                          const size_t size, const char* const file,
                          const int line);
void _assert_memory_not_equal(const void * const a, const void * const b,
                              const size_t size, const char* const file,
                              const int line);
void _assert_int_in_range(const intmax_t value,
                          const intmax_t minimum,
                          const intmax_t maximum,
                          const char* const file,
                          const int line);
void _assert_int_not_in_range(const intmax_t value,
                              const intmax_t minimum,
                              const intmax_t maximum,
                              const char *const file,
                              const int line);
void _assert_uint_in_range(const uintmax_t value,
                           const uintmax_t minimum,
                           const uintmax_t maximum,
                           const char* const file,
                           const int line);
void _assert_uint_not_in_range(const uintmax_t value,
                           const uintmax_t minimum,
                           const uintmax_t maximum,
                           const char* const file,
                           const int line);
void _assert_float_in_range(const double value,
                            const double minimum,
                            const double maximum,
                            const double epsilon,
                            const char* const file,
                            const int line);
void _assert_float_not_in_range(const double value,
                                const double minimum,
                                const double maximum,
                                const double epsilon,
                                const char* const file,
                                const int line);
void _assert_not_in_set(
    const uintmax_t value, const uintmax_t values[],
    const size_t number_of_values, const char* const file, const int line);
void _assert_int_in_set(const intmax_t value,
                        const intmax_t values[],
                        const size_t number_of_values,
                        const char *const file,
                        const int line);
void _assert_int_not_in_set(const intmax_t value,
                            const intmax_t values[],
                            const size_t number_of_values,
                            const char *const file,
                            const int line);
void _assert_uint_in_set(const uintmax_t value,
                         const uintmax_t values[],
                         const size_t number_of_values,
                         const char *const file,
                         const int line);
void _assert_uint_not_in_set(const uintmax_t value,
                             const uintmax_t values[],
                             const size_t number_of_values,
                             const char *const file,
                             const int line);
void _assert_float_in_set(const double value,
                          const double values[],
                          const size_t number_of_values,
                          const double epsilon,
                          const char *const file,
                          const int line);
void _assert_float_not_in_set(const double value,
                              const double values[],
                              const size_t number_of_values,
                              const double epsilon,
                              const char *const file,
                              const int line);

void* _test_malloc(const size_t size, const char* file, const int line);
void* _test_realloc(void *ptr, const size_t size, const char* file, const int line);
void* _test_calloc(const size_t number_of_elements, const size_t size,
                   const char* file, const int line);
void _test_free(void* const ptr, const char* file, const int line);

CMOCKA_NORETURN void _fail(const char * const file, const int line);

CMOCKA_NORETURN void _skip(const char * const file, const int line);

CMOCKA_NORETURN void _stop(void);

/* Test runner */
int _cmocka_run_group_tests(const char *group_name,
                            const struct CMUnitTest * const tests,
                            const size_t num_tests,
                            CMFixtureFunction group_setup,
                            CMFixtureFunction group_teardown);

/* Standard output and error print methods. */
void print_message(const char* const format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);
void print_error(const char* const format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);
void vprint_message(const char* const format, va_list args) CMOCKA_PRINTF_ATTRIBUTE(1, 0);
void vprint_error(const char* const format, va_list args) CMOCKA_PRINTF_ATTRIBUTE(1, 0);

/** Callbacks which can be set via cmocka_set_callbacks().
 * @sa cmocka_set_callbacks()
 */
struct CMCallbacks {
    /** A callback for printing out standard messages.
     * The supplied callback function will be invoked by the standard output
     * print methods. If no callback has been supplied, the default action
     * is to print to `stdout`.
     *
     * The one exception at present is XML output, which is always written directly
     * to a file handle, even if that is set to `stdout`. */
    void (*vprint_message)(const char * const format, va_list args);

    /** A callback for printing out error messages.
     * The supplied callback function will be invoked by the standard output
     * print methods. If no callback has been supplied, the default action
     * is to print to `stdout`.
     *
     * The one exception at present is XML output, which is always written directly
     * to a file handle, even if that is set to `stdout`. */
    void (*vprint_error)(const char * const format, va_list args);
};

/**
 * @brief Set callback functions for CMocka.
 *
 * Input is a structure containing function pointers to one or more
 * user-supplied callback functions. A NULL pointer for a particular callback
 * will set that callback to the default implementation.
 *
 * See the CMCallbacks documentation for details of each callback.
 *
 * @param[in] f_callbacks A structure containing the user callbacks to use.
 *
 * @sa CMCallbacks
 */
void cmocka_set_callbacks(const struct CMCallbacks *f_callbacks);

enum cm_message_output {
    CM_OUTPUT_STANDARD = 1,
    CM_OUTPUT_STDOUT = 1, /* API compatibility */
    CM_OUTPUT_SUBUNIT = 2,
    CM_OUTPUT_TAP = 4,
    CM_OUTPUT_XML = 8,
};

#ifdef DOXYGEN
/**
 * @deprecated Use cmocka_print_error()
 */
void cm_print_error(const char* const format, ...);
#else
#define cm_print_error(format, ...) \
    cmocka_print_error(format, ##__VA_ARGS__)
#endif

/**
 * @brief Print error message using the cmocka output format.
 *
 * This prints an error message using the message output defined by the
 * environment variable CMOCKA_MESSAGE_OUTPUT or cmocka_set_message_output().
 *
 * @param format  The formant string fprintf(3) uses.

 * @param ...     The parameters used to fill format.
 */
void cmocka_print_error(const char* const format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * @brief Function to set the output format for a test.
 *
 * The output format(s) for the test can either be set globally using this
 * function or overwritten with environment variable CMOCKA_MESSAGE_OUTPUT.
 *
 * The environment variable can be set to STANDARD, SUBUNIT, TAP or XML.
 * Multiple outputs separated with comma are permitted.
 * (e.g. export CMOCKA_MESSAGE_OUTPUT=STANDARD,XML)
 *
 * @param[in] output    The output format from cm_message_output to use for the
 *                      test. For multiple outputs OR options together.
 *
 */
void cmocka_set_message_output(uint32_t output);


/**
 * @brief Set a pattern to only run the test matching the pattern.
 *
 * This allows to filter tests and only run the ones matching the pattern. The
 * pattern can include two wildards. The first is '*', a wildcard that matches
 * zero or more characters, or '?', a wildcard that matches exactly one
 * character.
 *
 * @param[in]  pattern    The pattern to match, e.g. "test_wurst*"
 */
void cmocka_set_test_filter(const char *pattern);

/**
 * @brief Set a pattern to skip tests matching the pattern.
 *
 * This allows to filter tests and skip the ones matching the pattern. The
 * pattern can include two wildards. The first is '*', a wildcard that matches
 * zero or more characters, or '?', a wildcard that matches exactly one
 * character.
 *
 * @param[in]  pattern    The pattern to match, e.g. "test_wurst*"
 */
void cmocka_set_skip_filter(const char *pattern);


/** @} */

#endif /* CMOCKA_H_ */
