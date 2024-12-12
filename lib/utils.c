#include <stddef.h>

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
