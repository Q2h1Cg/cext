#ifndef __CEXT__
#define __CEXT__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* prelude */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* extension */
#define __concat0(a, b) a##b
#define __concat(a, b) __concat0(a, b)
#define __uniq() __concat(__uniq, __COUNTER__)

#define typeof __typeof__
#define let __auto_type

#define __lambda(func, args, ret, body) ({ inline ret func args body func; })
#define lambda(args, ret, body) __lambda(__uniq(), args, ret, body)

#define __defer(func, var, arg)                                                \
    auto void func(void *);                                                    \
    int var __attribute__((cleanup(func)));                                    \
    inline void func(__attribute__((unused)) void *arg)
#define defer __defer(__uniq(), __uniq(), __uniq())

#ifndef thread_local
#define thread_local __thread
#endif

/* container */
typedef struct {
    void *data;
    size_t __size;
    size_t __cap;
    size_t __len;
} array_t;

static inline void __array_init(array_t *a, size_t size) {
    memset(a, 0, sizeof(*a));
    a->__size = size;
}

#define array_init(a, type) __array_init(a, sizeof(type))

static inline void array_cleanup(array_t *a) {
    free(a->data);
}

static inline void *array_push(array_t *a) {
    if (a->__len == a->__cap) {
        a->__cap = a->__cap == 0 ? 1 : a->__cap * 2;
        a->data = realloc(a->data, a->__size * a->__cap);
    }

    a->__len++;
    return a->data + a->__size * (a->__len - 1);
}

static inline void *array_pop(array_t *a) {
    a->__len--;
    return a->data + a->__size * a->__len;
}

static inline void *array_get(array_t *a, size_t index) {
    return a->data + a->__size * index;
}

/* util */
#define __min(vals, val, i, val1, val2, ...)                                   \
    ({                                                                         \
        typeof(val1) vals[] = {val1, val2, ##__VA_ARGS__};                     \
        let val = val1;                                                        \
        for (let i = 0UL; i < len(vals); i++) {                                \
            val = val < vals[i] ? val : vals[i];                               \
        }                                                                      \
        val;                                                                   \
    })
#define min(val1, val2, ...)                                                   \
    __min(__uniq(), __uniq(), __uniq(), (val1), (val2), ##__VA_ARGS__)

#define __max(vals, val, i, val1, val2, ...)                                   \
    ({                                                                         \
        typeof(val1) vals[] = {val1, val2, ##__VA_ARGS__};                     \
        let val = val1;                                                        \
        for (let i = 0UL; i < len(vals); i++) {                                \
            val = val > vals[i] ? val : vals[i];                               \
        }                                                                      \
        val;                                                                   \
    })
#define max(val1, val2, ...)                                                   \
    __max(__uniq(), __uniq(), __uniq(), (val1), (val2), ##__VA_ARGS__)

#define __swap(tmp, val1, val2)                                                \
    do {                                                                       \
        let tmp = val1;                                                        \
        val1 = val2;                                                           \
        val2 = tmp;                                                            \
    } while (0)
#define swap(val1, val2) __swap(__uniq(), (val1), (val2))

#define __is_array_t(x)                                                        \
    (__builtin_types_compatible_p(typeof(x), array_t *) ||                     \
     __builtin_types_compatible_p(typeof(x), const array_t *))
#define __len_array_t(x) (((const array_t *)x)->__len)
#define __is_char_ptr(x)                                                       \
    (__builtin_types_compatible_p(typeof(x), char *) ||                        \
     __builtin_types_compatible_p(typeof(x), const char *))
#define __len_char_ptr(x) strlen((const char *)x)
#define __is_char_array(x)                                                     \
    (__is_array(x) && __builtin_types_compatible_p(typeof(x[0]), char))
#define __len_char_array(x)                                                    \
    __builtin_constant_p(x) ? (sizeof(x) - 1) : sizeof(x)
#define __is_array(x) (!__builtin_types_compatible_p(typeof(x), typeof(&x[0])))
#define __len_array(var, x)                                                    \
    ({                                                                         \
        _Pragma("GCC diagnostic push");                                        \
        _Pragma("GCC diagnostic ignored \"-Wsizeof-pointer-div\"");            \
        size_t var = sizeof(x) / sizeof(x[0]);                                 \
        var;                                                                   \
        _Pragma("GCC diagnostic pop");                                         \
    })
#define __len(var, x)                                                          \
    __builtin_choose_expr(                                                     \
        __is_array_t(x), __len_array_t(x),                                     \
        __builtin_choose_expr(                                                 \
            __is_char_ptr(x), __len_char_ptr(x),                               \
            __builtin_choose_expr(                                             \
                __is_char_array(x), __len_char_array(x),                       \
                __builtin_choose_expr(__is_array(x), __len_array(var, x),      \
                                      __builtin_unreachable()))))
#define len(x) __len(__uniq(), (x))

/* test */
#ifdef _WIN32
#define __FILENAME                                                             \
    ({                                                                         \
        let __fname = strrchr(__FILE__, '\\');                                 \
        __fname ? __fname + 1 : __FILE__;                                      \
    })
#else
#define __FILENAME                                                             \
    ({                                                                         \
        let __fname = strrchr(__FILE__, '/');                                  \
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
        for (let i = 0UL; i < sizeof(tests) / sizeof(tests[0]); i++) {         \
            tests[i]();                                                        \
        }                                                                      \
                                                                               \
        return 0;                                                              \
    }

#endif
