#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/utils.c"
#include "../lib/vec.h"

const size_t MAX_PATTERN = 32;
const size_t MAX_LINE = 255;

typedef VEC_OF(char *) str_vec;

str_vec read_patterns(FILE *input) {
    str_vec arr;
    VEC_NEW(arr);

    char endl[2];
    while (true) {
        char *pat = malloc(MAX_PATTERN * sizeof(char));
        size_t filled = fscanf(input, "%31[^,\n]%[,] ", pat, endl);
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

int void_strcmp(const void *left, const void *right) {
    const char *const *const a = left;
    const char *const *const b = right;
    return strcmp(*a, *b);
}

int void_strcmp_first(const void *left, const void *right) {
    const char *const *const a = left;
    const char *const *const b = right;
    return strncmp(*a, *b, 1);
}

size_t check_line(
    char line[static 1],
    size_t start,
    const str_vec patterns[static 1],
    size_t cache[static 1],
    bool abort_early
) {
    char *line0 = line + start;
    if (*line0 == 0)
        return 1;
    if (cache[start] > 0)
        return cache[start] == (size_t)-1 ? 0 : cache[start];
    size_t ways = 0;
    char **first = bsearch_left(
        &line0, patterns->data, patterns->size, sizeof(char *),
        void_strcmp_first
    );
    if (first != NULL) {
        for (size_t i = first - patterns->data; i < patterns->size; i++) {
            size_t o = 0;
            char *p = VEC_AT(*patterns, i);
            if (*p != *line0)
                break;
            while (*p && line0[o] && *p == line0[o]) {
                p++;
                o++;
            }
            if (*p == 0) {
                ways += check_line(
                    line, start + o, patterns, cache, abort_early
                );
                if (abort_early && ways > 0)
                    return 1;
            }
        }
    }
    cache[start] = ways == 0 ? (size_t)-1 : ways;
    return ways;
}

long part1(FILE *input) {
    str_vec patterns = read_patterns(input);
    qsort(patterns.data, patterns.size, sizeof(char *), void_strcmp);

// Just discard a newline
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\n");
#pragma GCC diagnostic pop

    size_t ans = 0;
    char line[MAX_LINE];
    size_t cache[MAX_LINE];
    while (fscanf(input, "%254[^\n]\n", line) == 1) {
        memset(cache, 0, MAX_LINE * sizeof(size_t));
        ans += check_line(line, 0, &patterns, cache, true) > 0;
    }

    VEC_FOREACH(patterns, free);
    VEC_DELETE(patterns);
    return (long)ans;
}

long part2(FILE *input) {
    str_vec patterns = read_patterns(input);
    qsort(patterns.data, patterns.size, sizeof(char *), void_strcmp);

// Just discard a newline
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(input, "\n");
#pragma GCC diagnostic pop

    size_t ans = 0;
    char line[MAX_LINE];
    size_t cache[MAX_LINE];
    while (fscanf(input, "%254[^\n]\n", line) == 1) {
        memset(cache, 0, MAX_LINE * sizeof(size_t));
        ans += check_line(line, 0, &patterns, cache, false);
    }

    VEC_FOREACH(patterns, free);
    VEC_DELETE(patterns);
    return (long)ans;
}
