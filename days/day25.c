#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#include "../lib/vec.h"

typedef VEC_OF(int_fast8_t *) intp_vec;

inline bool check(int_fast8_t *lock, int_fast8_t *key, size_t w, size_t h) {
    for (size_t i = 0; i < w; i++) {
        // both are at least one
        if ((size_t)(lock[i] + key[i] - 2) >= h - 1)
            return false;
    }
    return true;
}

inline bool ignore(FILE *input, char c) {
    int got = fgetc(input);
    return (got == 0 || got == c);
}

long part1(FILE *input) {
    intp_vec locks, keys;
    VEC_NEW(locks);
    VEC_NEW(keys);
    size_t w = 0, h = 0;
    char line[80];
    while (fscanf(input, "%79[^\n]", line) == 1) {
        if (w == 0)
            w = strlen(line);
        size_t h1 = 0;
        int_fast8_t *pins = calloc(w, sizeof(int_fast8_t));
        bool is_lock = line[0] == '#';

        do {
            h1++;
            for (size_t i = 0; i < w && line[i]; i++) {
                if (line[i] == '#')
                    pins[i]++;
            }
            if (!ignore(input, '\n'))
                break;
        } while (fscanf(input, "%79[^\n]", line) == 1);

        if (is_lock) {
            VEC_PUSH(locks, pins);
        } else {
            VEC_PUSH(keys, pins);
        }
        if (h == 0)
            h = h1;
        if (!ignore(input, '\n'))
            break;
    }

    long ans = 0;
    for (size_t i = 0; i < locks.size; i++) {
        int_fast8_t *lock = VEC_AT(locks, i);
        for (size_t j = 0; j < keys.size; j++) {
            ans += check(lock, VEC_AT(keys, j), w, h);
        }
    }

    VEC_FOREACH(locks, free);
    VEC_FOREACH(keys, free);
    VEC_DELETE(locks);
    VEC_DELETE(keys);
    return ans;
}

long part2(FILE *input) {
    return 0;
}
