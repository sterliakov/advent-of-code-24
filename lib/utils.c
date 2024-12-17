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

#endif
