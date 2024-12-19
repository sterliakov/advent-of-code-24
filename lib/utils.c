#ifndef CUSTOM_UTILS_H
#define CUSTOM_UTILS_H

#include <stddef.h>

#define SWAP(t, a, b) \
    do {              \
        t tmp = (a);  \
        (a) = (b);    \
        (b) = tmp;    \
    } while (0);

long gcd(long a, long b) {
    long temp;
    while (b != 0) {
        temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

size_t numdigits(long val) {
    size_t n = 0;
    for (; val; val /= 10) {
        n++;
    }
    return n;
}

// same as `bsearch`, but always returning the leftmost matching entry
inline void *bsearch_left(
    const void *key,
    const void *base,
    size_t count,
    size_t size,
    int (*compar)(const void *, const void *)
) {
    while (count > 0) {
        size_t m = (count - 1) / 2;
        const char *p = (const char *)base + m * size;
        int order = compar(key, p);
        if (order < 0) {
            count = m;
        } else if (order > 0) {
            base = p + size;
            count -= m + 1;
        } else if (m == 0) {
            return (void *)p;
        } else {
            count = m + 1;
        }
    }
    return NULL;
}

#endif
