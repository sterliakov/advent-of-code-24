#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

inline uint_fast32_t mix(uint_fast32_t a, uint_fast32_t b) {
    return a ^ b;
}
inline uint_fast32_t prune(uint_fast32_t v) {
    return v % 16777216;
}
inline uint_fast32_t nxt(uint_fast32_t v) {
    v = prune(mix(v, v << 6));
    v = prune(mix(v, v >> 5));
    v = prune(mix(v, v << 11));
    return v;
}

long part1(FILE *input) {
    uint_fast32_t total = 0;
    uint_fast32_t v;
    while (fscanf(input, "%lu\n", &v) == 1) {
        for (size_t i = 0; i < 2000; i++) {
            v = nxt(v);
        }
        total += v;
    }

    return (long)total;
}

long part2(FILE *input) {
    const size_t diff_count = 19;
    const size_t diff_len = diff_count * diff_count * diff_count * diff_count;
    const size_t iters = 2000;

    uint_fast32_t *totals = calloc(diff_len, sizeof(uint_fast32_t));
    int_fast8_t *diff = calloc(iters + 1, sizeof(int_fast8_t));
    size_t *visited = calloc(diff_len, sizeof(size_t));
    if (totals == NULL || diff == NULL || visited == NULL) {
        free(totals);
        free(diff);
        free(visited);
        perror("malloc failed");
        return -1;
    }

    uint_fast32_t v;
    // Save on memset - instead of resetting visits, just check for new value
    // every time
    size_t id = 1;
    while (fscanf(input, "%lu\n", &v) == 1) {
        uint_fast8_t price = v % 10, oldprice = price;
        for (size_t i = 1; i <= iters; i++) {
            v = nxt(v);
            price = v % 10;
            diff[i] = (int_fast8_t)(price - oldprice + 9);
            if (i >= 4) {
                size_t k = (diff[i - 3]) * diff_count * diff_count * diff_count
                           + (diff[i - 2]) * diff_count * diff_count
                           + (diff[i - 1]) * diff_count + diff[i];
                if (visited[k] != id) {
                    visited[k] = id;
                    totals[k] += price;
                }
            }
            oldprice = price;
        }
        id++;
    }
    free(diff);
    free(visited);

    uint_fast32_t best = 0;
    for (size_t i = 0; i < diff_len; i++) {
        if (totals[i] > best) {
            best = totals[i];
        }
    }
    free(totals);

    return (long)best;
}
