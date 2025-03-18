#include <pthread.h>

#include "cext.h"

static void *get_thread_local(void *arg) {
    static thread_local int var = 0;

    var++;
    *(int *)arg = var;
    return NULL;
}

define_test(test_thread_local) {
    pthread_t th1;
    pthread_t th2;
    int var1;
    int var2;

    pthread_create(&th1, NULL, get_thread_local, &var1);
    pthread_join(th1, NULL);
    pthread_create(&th2, NULL, get_thread_local, &var2);
    pthread_join(th2, NULL);
    assert_eq(var1, 1);
    assert_eq(var2, 1);
}

define_test(test_lambda) {
    int (*max2)(int, int) = lambda((int a, int b), int, {
        if (a > b) {
            return a;
        } else {
            return b;
        }
    });
    assert_eq(max2(1, 2), 2);

    int nums[] = {1, 4, 3, 2};
    qsort(nums, array_len(nums), sizeof(int),
          lambda((const void *a, const void *b), int,
                 { return *(int *)a - *(int *)b; }));
    int want[] = {1, 2, 3, 4};
    assert_mem_eq(nums, want, sizeof(nums));
}

define_test(test_defer) {
    int a;
    int b;

    a = 0;
    b = 0;

    {
        defer { a++; }
        defer { b = a; }
    }

    assert_eq(a, 1);
    assert_eq(b, 0);
}

define_test(test_min) {
    assert_eq(min(2, 1), 1);
    assert_eq(min(3, 2, 1), 1);
}

define_test(test_max) {
    assert_eq(max(1, 2), 2);
    assert_eq(max(1, 2, 3), 3);
}

define_test(test_swap) {
    int a = 1;
    int b = 2;
    swap(a, b);
    assert_eq(a, 2);
    assert_eq(b, 1);
}

define_test(test_array_len) {
    int arr[1];

    assert_eq(array_len(arr), 1);
}

define_test(test_string_len) {
    char *s;

    assert_eq(string_len("123"), 3);

    s = "123";
    assert_eq(string_len(s), 3);
}

run_tests({
    test_thread_local,
    test_lambda,
    test_defer,
    test_min,
    test_max,
    test_swap,
    test_array_len,
    test_string_len,
})
