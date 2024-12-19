#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/vec.h"

const size_t MAX_PATTERN = 32;
#define MAX_LINE 255

typedef VEC_OF(char *) str_vec;

str_vec read_input(FILE *input) {
    str_vec arr;
    VEC_NEW(arr);

    char endl[2];
    while (true) {
        char *pat = malloc(MAX_PATTERN * sizeof(char));
        size_t filled = fscanf(input, "%31[^,\n]%[,] ", pat, endl);
        // printf(">>> %s\n", pat);
        if (filled >= 1) {
            VEC_PUSH(arr, pat);
            if (filled == 1) {
                // newline - end of input
                break;
            }
        } else {
            free(pat);
            break;
        }
    }
    return arr;
}

size_t check_line(
    char line[static 1],
    size_t start,
    str_vec patterns[static 1],
    size_t cache[static 1],
    bool abort_early
) {
    char *line0 = line + start;
    if (*line0 == 0)
        return true;
    if (cache[start] > 0)
        return cache[start] == (size_t)-1 ? 0 : cache[start];
    // printf("Checking %s\n", line+start);
    size_t ways = 0;
    for (size_t i = 0; i < patterns->size; i++) {
        size_t o = 0;
        char *line1 = line0;
        char *p = VEC_AT((*patterns), i);
        while (*p && *line1 && *p == *line1) {
            p++;
            line1++;
            o++;
        }
        if (*p == 0) {
            ways += check_line(line, start + o, patterns, cache, abort_early);
            if (abort_early && ways > 0)
                return 1;
        }
    }
    cache[start] = ways == 0 ? (size_t)-1 : ways;
    return ways;
}

long part1(FILE *input) {
    str_vec patterns = read_input(input);

// Just discard a newline
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\n");
#pragma GCC diagnostic pop

    size_t ans = 0;
    char line[MAX_LINE] = {0};
    size_t cache[MAX_LINE];
    while (fscanf(input, "%254[^\n]\n", line) == 1) {
        // printf(">>> %s %lu\n", line, ans);
        memset(cache, 0, MAX_LINE * sizeof(size_t));
        ans += check_line(line, 0, &patterns, cache, true) > 0;
    }

    for (size_t i = 0; i < patterns.size; i++) {
        free(VEC_AT(patterns, i));
    }
    VEC_DELETE(patterns);
    return (long)ans;
}

long part2(FILE *input) {
    str_vec patterns = read_input(input);

// Just discard a newline
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\n");
#pragma GCC diagnostic pop

    size_t ans = 0;
    char line[MAX_LINE] = {0};
    size_t cache[MAX_LINE];
    while (fscanf(input, "%254[^\n]\n", line) == 1) {
        memset(cache, 0, MAX_LINE * sizeof(size_t));
        ans += check_line(line, 0, &patterns, cache, false);
    }

    for (size_t i = 0; i < patterns.size; i++) {
        free(VEC_AT(patterns, i));
    }
    VEC_DELETE(patterns);
    return (long)ans;
}
