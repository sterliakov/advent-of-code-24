#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/input.c"
#include "../lib/utils.c"
#include "common.h"

#define MAX_CACHED 1024
#define MAX_STEPS 75

size_t count_outs(long n, size_t steps) {
    if (steps == 0)
        return 1;
    static size_t cache[MAX_CACHED * MAX_STEPS] = {0};
    static long powers_of_ten[]
        = {1,       10,       100,       1000,       10000,      100000,
           1000000, 10000000, 100000000, 1000000000, 10000000000};
    size_t cache_id = steps + (MAX_STEPS * n);

    size_t c;
    if (n < MAX_CACHED && (c = cache[cache_id])) {
        return c;
    } else {
        size_t d;
        if (n == 0) {
            c = count_outs(1, steps - 1);
        } else if (((d = numdigits(n)) & 1) == 0) {
            assert(d <= 22);
            long pt = powers_of_ten[d / 2];
            c = count_outs(n / pt, steps - 1) + count_outs(n % pt, steps - 1);
        } else {
            c = count_outs(n * 2024, steps - 1);
        }
        if (n < MAX_CACHED) {
            cache[cache_id] = c;
        }
        return c;
    }
}

long solve(FILE *input, size_t repeats) {
    assert(repeats <= MAX_STEPS);
    size_t allocated = 8, nums_count = 0;
    int *line = malloc(allocated * sizeof(int));
    if (line == NULL) {
        perror("malloc failed");
        return -1;
    }
    if (!csv_int_read_line(input, ' ', &line, &allocated, &nums_count)) {
        perror("Failed to read the input array");
        free(line);
        return -1;
    }

    size_t total = 0;
    for (size_t i = 0; i < nums_count; i++) {
        total += count_outs(line[i], repeats);
    }
    free(line);

    return (long)total;
}

long part1(FILE *input) {
    return solve(input, 25);
}
long part2(FILE *input) {
    return solve(input, 75);
}
