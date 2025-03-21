#include <pthread.h>

#include "cext.h"

define_test(test_typeof) {
    int a = 1;
    assert(__builtin_types_compatible_p(typeof(a), int));
}

define_test(test_let) {
    let a = 1;
    assert(__builtin_types_compatible_p(typeof(a), int));
    let b = 1.1;
    assert(__builtin_types_compatible_p(typeof(b), double));
    let c = "a";
    assert(__builtin_types_compatible_p(typeof(c), char *));
}

define_test(test_lambda) {
    let max2 = lambda((int a, int b), int, {
        if (a > b) {
            return a;
        } else {
            return b;
        }
    });
    assert_eq(max2(1, 2), 2);

    int nums[] = {1, 4, 3, 2};
    let cmp = lambda((const void *a, const void *b), int,
                     { return *(int *)a - *(int *)b; });
    qsort(nums, len(nums), sizeof(int), cmp);
    int want[] = {1, 2, 3, 4};
    assert_mem_eq(nums, want, sizeof(nums));
}

define_test(test_defer) {
    let a = 0;
    let b = 0;

    {
        defer { a++; }
        defer { b = a; }
    }

    assert_eq(a, 1);
    assert_eq(b, 0);
}

static void *get_thread_local(void *arg) {
    static thread_local int var = 0;

    var++;
    *(int *)arg = var;
    return NULL;
}

define_test(test_thread_local) {
    pthread_t th1;
    int var1;
    pthread_create(&th1, NULL, get_thread_local, &var1);
    pthread_join(th1, NULL);

    pthread_t th2;
    int var2;
    pthread_create(&th2, NULL, get_thread_local, &var2);
    pthread_join(th2, NULL);

    assert_eq(var1, 1);
    assert_eq(var2, 1);
}

define_test(test_array) {
    array_t arr;
    array_init(&arr, int);
    assert_eq(len(&arr), 0);
    assert(!arr.data);

    for (let i = 0; i < 5; i++) {
        let p = (int *)array_push(&arr);
        *p = i;
    }
    assert_eq(len(&arr), 5);

    for (let i = 0; i < 5; i++) {
        let p = (int *)array_get(&arr, i);
        assert_eq(p, arr.data + i * sizeof(int));
        assert_eq(*p, i);
    }

    for (let i = 0; i < 5; i++) {
        let p = (int *)array_pop(&arr);
        assert_eq(*p, 4 - i);
    }
    assert_eq(len(&arr), 0);

    array_cleanup(&arr);
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
    let a = 1;
    let b = 2;
    swap(a, b);
    assert_eq(a, 2);
    assert_eq(b, 1);
}

define_test(test_len) {
    int arr[1] = {0};
    assert_eq(len(arr), 1);

    assert_eq(len("123"), 3);

    let s1 = "123";
    assert_eq(len(s1), 3);

    char s2[] = {1, 2, 3};
    assert_eq(len(s2), 3);
}

run_tests({
    test_typeof,
    test_let,
    test_lambda,
    test_defer,
    test_thread_local,
    test_array,
    test_min,
    test_max,
    test_swap,
    test_len,
})
