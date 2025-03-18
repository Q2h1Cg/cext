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

run_tests({
    test_defer,
})
