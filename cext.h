#ifndef __CEXT__
#define __CEXT__

/* basic types */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* thread_local */
#ifndef thread_local
#define thread_local __thread
#endif

/* test functions */
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

/* utils */
#define __concat0(a, b) a##b
#define __concat(a, b) __concat0(a, b)
#define __uniq() __concat(__uniq, __COUNTER__)

#define __min(vals, val, i, val1, val2, ...)                                   \
    ({                                                                         \
        __typeof__(val1) vals[] = {val1, val2, ##__VA_ARGS__};                 \
        __typeof__(val1) val = val1;                                           \
        for (size_t i = 0; i < sizeof(vals) / sizeof(vals[0]); i++) {          \
            val = val < vals[i] ? val : vals[i];                               \
        }                                                                      \
        val;                                                                   \
    })
#define min(val1, val2, ...)                                                   \
    __min(__uniq(), __uniq(), __uniq(), (val1), (val2), ##__VA_ARGS__)

#define __max(vals, val, i, val1, val2, ...)                                   \
    ({                                                                         \
        __typeof__(val1) vals[] = {val1, val2, ##__VA_ARGS__};                 \
        __typeof__(val1) val = val1;                                           \
        for (size_t i = 0; i < sizeof(vals) / sizeof(vals[0]); i++) {          \
            val = val > vals[i] ? val : vals[i];                               \
        }                                                                      \
        val;                                                                   \
    })
#define max(val1, val2, ...)                                                   \
    __max(__uniq(), __uniq(), __uniq(), (val1), (val2), ##__VA_ARGS__)

#define __swap(tmp, val1, val2)                                                \
    do {                                                                       \
        __typeof__(val1) tmp = val1;                                           \
        (val1) = val2;                                                         \
        (val2) = tmp;                                                          \
    } while (0)
#define swap(val1, val2) __swap(__uniq(), (val1), (val2))

#define array_len(arr) (sizeof(arr) / sizeof(arr[0]))

#define string_len(s) (__builtin_constant_p(s) ? (sizeof(s) - 1) : strlen(s))

#define __defer(func, var, arg)                                                \
    auto void func(void *);                                                    \
    int var __attribute__((cleanup(func)));                                    \
    inline void func(__attribute__((unused)) void *arg)
#define defer __defer(__uniq(), __uniq(), __uniq())

#endif
