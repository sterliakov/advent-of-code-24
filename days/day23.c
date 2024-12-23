#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include "../lib/vec.h"

const size_t LETTERS = 'z' - 'a' + 1;
const size_t BOARD_SIZE = LETTERS * LETTERS;

typedef unsigned int uint;
typedef VEC_OF(uint) uint_vec;

inline uint charcode(char s[static 2]) {
    return LETTERS * (s[0] - 'a') + (s[1] - 'a');
}

inline char first_letter(uint code) {
    return (char)(code / LETTERS + 'a');
}

inline char second_letter(uint code) {
    return (char)(code % LETTERS + 'a');
}

void read_edges(FILE *input, bool adj[static 1]) {
    char left[3] = {0};
    char right[3] = {0};
    while (fscanf(input, "%2s-%2s\n", left, right) == 2) {
        adj[charcode(left) * BOARD_SIZE + charcode(right)] = true;
        adj[charcode(right) * BOARD_SIZE + charcode(left)] = true;
    }
}

long part1(FILE *input) {
    bool *adj = calloc(BOARD_SIZE * BOARD_SIZE, sizeof(bool));
    if (adj == NULL) {
        perror("calloc failed");
        return -1;
    }
    read_edges(input, adj);

    long ans = 0;
    static long weights[] = {6, 3, 2};
    for (uint a = charcode("ta"); a <= charcode("tz"); a++) {
        for (uint b = 0; b < BOARD_SIZE; b++) {
            if (!adj[a * BOARD_SIZE + b])
                continue;
            for (uint c = b + 1; c < BOARD_SIZE; c++) {
                if (adj[a * BOARD_SIZE + c] && adj[b * BOARD_SIZE + c]) {
                    // We count 2 t's twice (starting in each) and 3 t's 3 times
                    // Divide weight by 6 at the end to avoid FP math
                    uint other_ts
                        = ((first_letter(b) == 't') + (first_letter(c) == 't'));
                    ans += weights[other_ts];
                }
            }
        }
    }

    free(adj);
    return ans / 6;
}

bool is_subset(uint_vec left[static 1], bool right[static 1]) {
    for (size_t i = 0; i < left->size; i++) {
        int p = VEC_AT(*left, i);
        if (!right[p])
            return false;
    }
    return true;
}

int cmp(const void *left, const void *right) {
    uint a = *(uint *)left;
    uint b = *(uint *)right;
    return (a > b) - (a < b);
}

long part2(FILE *input) {
    bool *adj = calloc(BOARD_SIZE * BOARD_SIZE, sizeof(bool));
    if (adj == NULL) {
        perror("calloc failed");
        return -1;
    }
    read_edges(input, adj);

    bool *seen = calloc(BOARD_SIZE, sizeof(bool));
    if (seen == NULL) {
        free(adj);
        perror("calloc failed");
        return -1;
    }
    uint_vec best, curr;
    VEC_NEW(best);
    VEC_NEW(curr);

    // We're looking for a largest clique in a graph.
    // The trivial check below is only a heuristic, but it finds a clique
    // if that's same size as max vertex degree, which is the case in our
    // input.
    for (uint a = 0; a < BOARD_SIZE; a++) {
        if (seen[a])
            continue;
        seen[a] = true;
        curr.size = 0;
        VEC_PUSH(curr, a);
        for (uint b = 0; b < BOARD_SIZE; b++) {
            if (!adj[a * BOARD_SIZE + b])
                continue;
            if (is_subset(&curr, adj + b * BOARD_SIZE)) {
                seen[b] = true;
                VEC_PUSH(curr, b);
            }
        }
        if (curr.size > best.size) {
            VEC_DELETE(best);
            best = curr;
            VEC_NEW(curr);
        }
    }

    free(adj);
    free(seen);
    VEC_DELETE(curr);

    long ans = (long)best.size;
    qsort(best.data, best.size, sizeof(int), cmp);
    printf("Part 2 sequence:");
    for (size_t i = 0; i < best.size; i++) {
        int curr = VEC_AT(best, i);
        printf(
            "%c%c%c", i == 0 ? ' ' : ',', first_letter(curr),
            second_letter(curr)
        );
    }
    puts("");
    VEC_DELETE(best);
    return ans;
}
