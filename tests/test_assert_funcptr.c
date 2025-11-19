#include "config.h"

#include <cmocka.h>

/* Test structure with function pointers similar to cipher struct in libssh */
struct test_ops {
    int (*set_encrypt_key)(void *ctx, void *key);
    int (*set_decrypt_key)(void *ctx, void *key);
    void (*encrypt)(void *ctx, void *in, void *out, size_t len);
    void (*decrypt)(void *ctx, void *in, void *out, size_t len);
};

/* Some dummy functions for testing */
static int dummy_set_key(void *ctx, void *key)
{
    (void)ctx;
    (void)key;
    return 0;
}

static void dummy_crypt(void *ctx, void *in, void *out, size_t len)
{
    (void)ctx;
    (void)in;
    (void)out;
    (void)len;
}

/* Test assert_non_null with function pointer */
static void test_funcptr_non_null(void **state)
{
    struct test_ops ops = {
        .set_encrypt_key = dummy_set_key,
        .set_decrypt_key = dummy_set_key,
        .encrypt = dummy_crypt,
        .decrypt = dummy_crypt,
    };

    (void)state; /* unused */

    /* These should pass - function pointers are not NULL */
    assert_non_null(ops.set_encrypt_key);
    assert_non_null(ops.set_decrypt_key);
    assert_non_null(ops.encrypt);
    assert_non_null(ops.decrypt);
}

/* Test assert_non_null_msg with function pointer */
static void test_funcptr_non_null_msg(void **state)
{
    struct test_ops ops = {
        .set_encrypt_key = dummy_set_key,
        .set_decrypt_key = NULL,
        .encrypt = dummy_crypt,
        .decrypt = NULL,
    };

    (void)state; /* unused */

    /* These should pass */
    assert_non_null_msg(ops.set_encrypt_key,
                        "set_encrypt_key should not be NULL");
    assert_non_null_msg(ops.encrypt, "encrypt should not be NULL");
}

/* Test assert_null with function pointer */
static void test_funcptr_null(void **state)
{
    struct test_ops ops = {
        .set_encrypt_key = NULL,
        .set_decrypt_key = NULL,
        .encrypt = NULL,
        .decrypt = NULL,
    };

    (void)state; /* unused */

    /* These should pass - function pointers are NULL */
    assert_null(ops.set_encrypt_key);
    assert_null(ops.set_decrypt_key);
    assert_null(ops.encrypt);
    assert_null(ops.decrypt);
}

/* Test assert_null_msg with function pointer */
static void test_funcptr_null_msg(void **state)
{
    struct test_ops ops = {
        .set_encrypt_key = NULL,
        .set_decrypt_key = NULL,
        .encrypt = NULL,
        .decrypt = NULL,
    };

    (void)state; /* unused */

    /* These should pass */
    assert_null_msg(ops.set_encrypt_key, "set_encrypt_key should be NULL");
    assert_null_msg(ops.decrypt, "decrypt should be NULL");
}

/* Test assert_ptr_equal with function pointers */
static void test_funcptr_ptr_equal(void **state)
{
    struct test_ops ops1 = {
        .set_encrypt_key = dummy_set_key,
        .set_decrypt_key = dummy_set_key,
        .encrypt = dummy_crypt,
        .decrypt = dummy_crypt,
    };

    struct test_ops ops2 = {
        .set_encrypt_key = dummy_set_key,
        .set_decrypt_key = dummy_set_key,
        .encrypt = dummy_crypt,
        .decrypt = dummy_crypt,
    };

    (void)state; /* unused */

    /* These should pass - same function pointers */
    assert_ptr_equal(ops1.set_encrypt_key, ops2.set_encrypt_key);
    assert_ptr_equal(ops1.encrypt, ops2.encrypt);
}

/* Test assert_ptr_not_equal with function pointers */
static void test_funcptr_ptr_not_equal(void **state)
{
    struct test_ops ops = {
        .set_encrypt_key = dummy_set_key,
        .set_decrypt_key = dummy_set_key,
        .encrypt = dummy_crypt,
        .decrypt = dummy_crypt,
    };

    (void)state; /* unused */

    /* These should pass - different function pointers */
    assert_ptr_not_equal(ops.set_encrypt_key, ops.encrypt);
}

int main(void)
{
    const struct CMUnitTest funcptr_tests[] = {
        cmocka_unit_test(test_funcptr_non_null),
        cmocka_unit_test(test_funcptr_non_null_msg),
        cmocka_unit_test(test_funcptr_null),
        cmocka_unit_test(test_funcptr_null_msg),
        cmocka_unit_test(test_funcptr_ptr_equal),
        cmocka_unit_test(test_funcptr_ptr_not_equal),
    };

    return cmocka_run_group_tests(funcptr_tests, NULL, NULL);
}
