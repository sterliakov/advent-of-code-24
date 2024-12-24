#ifndef CUSTOM_VEC_H
#define CUSTOM_VEC_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Can't wrap type name in parens here
// NOLINTBEGIN(bugprone-macro-parentheses)
#define VEC_OF(typ)                       \
    struct __attribute__((aligned(32))) { \
        typ *data;                        \
        size_t size;                      \
        size_t capacity;                  \
    }
// NOLINTEND(bugprone-macro-parentheses)

#define VEC_WITH_CAPACITY(vec, n)                       \
    do {                                                \
        (vec).data = malloc((n) * sizeof(*(vec).data)); \
        if ((vec).data == NULL) {                       \
            fputs("malloc failed!\n", stderr);          \
            abort();                                    \
        }                                               \
        (vec).size = 0;                                 \
        (vec).capacity = (n);                           \
    } while (0)

#define VEC_NEW(vec) VEC_WITH_CAPACITY(vec, 1)

#define VEC_PUSH(vec, val)                                                   \
    do {                                                                     \
        assert((vec).capacity > 0);                                          \
        if ((vec).size >= (vec).capacity) {                                  \
            (vec).capacity *= 2;                                             \
            void *new_data                                                   \
                = realloc((vec).data, (vec).capacity * sizeof(*(vec).data)); \
            if (new_data == NULL) {                                          \
                fprintf(stderr, "realloc failed\n");                         \
                exit(1);                                                     \
            }                                                                \
            (vec).data = new_data;                                           \
        }                                                                    \
        (vec).data[(vec).size++] = (val);                                    \
    } while (0)

#define VEC_AT(vec, i) (vec).data[i]

#define VEC_LAST(vec) (vec).data[(vec).size - 1]

#define VEC_DELETE(vec)     \
    do {                    \
        free((vec).data);   \
        (vec).size = 0;     \
        (vec).capacity = 0; \
    } while (0)

#define VEC_FOREACH(vec, fn)                  \
    for (size_t i = 0; i < (vec).size; i++) { \
        (fn)(VEC_AT((vec), i));               \
    }

#define VEC_CLONE(dest, src)                                               \
    do {                                                                   \
        (dest).size = (src).size;                                          \
        (dest).capacity = (src).capacity;                                  \
        (dest).data = calloc((src).capacity, sizeof(*(src).data));         \
        if ((dest).data == NULL) {                                         \
            fprintf(stderr, "calloc failed\n");                            \
            exit(1);                                                       \
        }                                                                  \
        memcpy((dest).data, (src).data, (src).size * sizeof(*(src).data)); \
    } while (0)

#define VEC_SORT(vec, cmp) \
    qsort((vec).data, (vec).size, sizeof(*(vec).data), (cmp))

#endif
