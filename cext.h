#ifndef __CEXT__
#define __CEXT__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define __FILENAME                                                             \
    ({                                                                         \
        const char *__fname = strrchr(__FILE__, '\\');                         \
        __fname ? __fname + 1 : __FILE__;                                      \
    })
#else
#define __FILENAME                                                             \
    ({                                                                         \
        const char *__fname = strrchr(__FILE__, '/');                          \
        __fname ? __fname + 1 : __FILE__;                                      \
    })
#endif

#define __check(cond, message)                                                 \
    do {                                                                       \
        if (!(cond)) {                                                         \
            printf("%s:%d %s: %s\n", __FILENAME, __LINE__, __func__, message); \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#define assert(cond) __check(cond, #cond " is false")

#define assert_eq(expr1, expr2)                                                \
    __check((expr1) == (expr2), #expr1 " != " #expr2)

#define assert_ne(expr1, expr2)                                                \
    __check((expr1) != (expr2), #expr1 " == " #expr2)

#define assert_str_eq(str1, str2)                                              \
    __check(strcmp((str1), (str2)) == 0, #str1 " != " #str2)

#define assert_str_ne(str1, str2)                                              \
    __check(strcmp((str1), (str2)) != 0, #str1 " == " #str2)

#define assert_mem_eq(data1, data2, size)                                      \
    __check(memcmp((data1), (data2), size) == 0, #data1 " != " #data2)

#define assert_mem_ne(data1, data2, size)                                      \
    __check(memcmp((data1), (data2), size) != 0, #data1 " == " #data2)

#define define_test(name) static void name(void)

#define run_tests(...)                                                         \
    int main(void) {                                                           \
        void (*tests[])(void) = __VA_ARGS__;                                   \
        for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {        \
            tests[i]();                                                        \
        }                                                                      \
                                                                               \
        return 0;                                                              \
    }

#define __concat0(a, b) a##b
#define __concat(a, b) __concat0(a, b)
#define __uniq() __concat(__uniq, __COUNTER__)
#define __defer(func, var, arg)                                                \
    auto void func(void *);                                                    \
    int var __attribute__((cleanup(func)));                                    \
    inline void func(__attribute__((unused)) void *arg)
#define defer __defer(__uniq(), __uniq(), __uniq())

#endif
