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

int cmp(const void *left, const void *right) {
    uint a = *(uint *)left;
    uint b = *(uint *)right;
    return (a > b) - (a < b);
}

inline void intersect_adj(
    bool adj[const static 1],
    uint_vec set[const static 1],
    uint v,
    uint since,
    uint_vec dest[static 1]
) {
    dest->size = 0;
    for (size_t i = since; i < set->size; i++) {
        uint c = VEC_AT(*set, i);
        if (adj[v * BOARD_SIZE + c]) {
            VEC_PUSH(*dest, c);
        }
    }
}

// Find a pivot vertex with max amount of neighbours
inline uint pivot(bool adj[const static 1], uint_vec p[const static 1]) {
    uint best = 0, bestv = 0;
    for (size_t i = 0; i < p->size; i++) {
        uint v = 0;
        for (size_t j = 0; j < p->size; j++) {
            if (adj[i * BOARD_SIZE + j])
                v++;
        }
        if (v > bestv) {
            best = i;
            bestv = v;
        }
    }
    return best;
}

// https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
void bron_kerbosch(
    bool adj[const static 1],
    uint_vec r[static 1],
    uint_vec p[static 1],
    uint_vec x[static 1],
    uint_vec best[static 1]
) {
    if (p->size == 0 && x->size == 0) {
        if (r->size > best->size) {
            VEC_DELETE(*best);
            VEC_CLONE(*best, *r);
        }
        return;
    }
    if (r->size + p->size <= best->size) {
        // We're only looking for the maximum clique. If (nodes we have + nodes
        // we'll check) is already less than the best, abort early - we can't
        // find a bigger clique here.
        return;
    }
    uint_vec p1, x1;
    VEC_WITH_CAPACITY(p1, p->size + 1);
    VEC_WITH_CAPACITY(x1, x->size + 1);
    uint u = pivot(adj, p);

    for (size_t i = 0; i < p->size; i++) {
        uint v = VEC_AT(*p, i);
        if (adj[v * BOARD_SIZE + u])
            continue;
        VEC_PUSH(*r, v);
        intersect_adj(adj, p, v, i, &p1);
        intersect_adj(adj, x, v, 0, &x1);
        bron_kerbosch(adj, r, &p1, &x1, best);
        r->size--;
        VEC_PUSH(*x, v);
    }
    VEC_DELETE(p1);
    VEC_DELETE(x1);
}

long part2(FILE *input) {
    bool *adj = calloc(BOARD_SIZE * BOARD_SIZE, sizeof(bool));
    if (adj == NULL) {
        perror("calloc failed");
        return -1;
    }
    read_edges(input, adj);

    uint_vec best;
    VEC_NEW(best);

    uint_vec r, p, x;
    VEC_NEW(r);
    VEC_NEW(x);
    VEC_WITH_CAPACITY(p, BOARD_SIZE);
    for (size_t i = 0; i < BOARD_SIZE; i++) {
        uint v = 0;
        for (size_t j = 0; j < BOARD_SIZE; j++) {
            if (adj[i * BOARD_SIZE + j]) {
                v++;
                break;
            }
        }
        if (v != 0) {
            VEC_PUSH(p, i);
        }
    }
    bron_kerbosch(adj, &r, &p, &x, &best);
    free(adj);
    VEC_DELETE(r);
    VEC_DELETE(p);
    VEC_DELETE(x);

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
