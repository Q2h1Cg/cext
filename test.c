#include <pthread.h>

#include "cext.h"

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

run_tests({
    test_defer,
    test_thread_local,
})
