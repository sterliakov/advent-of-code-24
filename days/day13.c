#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

typedef struct __attribute__((aligned(16))) pair_t {
    long x;
    long y;
} pair_t;

typedef struct __attribute__((aligned(64))) machine_t {
    pair_t left;
    pair_t right;
    pair_t target;
} machine_t;

size_t read_input(FILE *input, machine_t **dest) {
    size_t count = 0;
    size_t arr_size = 2;
    *dest = malloc(arr_size * sizeof(machine_t));
    if (*dest == NULL) {
        perror("Malloc failed");
        return 0;
    }
    while (true) {
        if (count >= arr_size) {
            arr_size *= 2;
            machine_t *olddest = *dest;
            *dest = realloc(*dest, arr_size * sizeof(machine_t));
            if (*dest == NULL) {
                free(olddest);
                perror("Malloc failed");
                return 0;
            }
        }
        long a, b;
// Just discard a newline if any
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        fscanf(input, "\n");
#pragma GCC diagnostic pop
        if (fscanf(input, "Button A: X+%ld, Y+%ld\n", &a, &b) < 2) {
            break;
        }
        long c, d, x, y;
        if (fscanf(input, "Button B: X+%ld, Y+%ld\n", &c, &d) < 2) {
            fprintf(stderr, "Malformed input in block %ld\n", count + 1);
            return 0;
        }
        if (fscanf(input, "Prize: X=%ld, Y=%ld", &x, &y) < 2) {
            fprintf(stderr, "Malformed input in block %ld\n", count + 1);
            return 0;
        }
        (*dest)[count]
            = (machine_t){(pair_t){a, b}, (pair_t){c, d}, (pair_t){x, y}};
        count++;
    }
    return count;
}

// Returns -1 for no solution,
// -2 for infinitely many solutions,
// 0 for unique solution.
// Writes a and b iff a unique solution was found (returns 0)
int linsolve(machine_t *m, long a[static 1], long b[static 1]) {
    long d = m->left.x * m->right.y - m->left.y * m->right.x;
    long first = m->target.x * m->right.y - m->target.y * m->right.x;
    long second = m->left.x * m->target.y - m->left.y * m->target.x;
    if (d == 0) {
        // Determinant is zero, either no solution (partials nonzero)
        // or infinitely many of them (partials zero)
        return first != 0 && second != 0 ? -1 : -2;
    } else {
        // Determinant is nonzero, a solution is unique.
        if (first % d == 0 && second % d == 0) {
            // Solution is integer
            *a = first / d;
            *b = second / d;
            return 0;
        } else {
            // Solution isn't integer
            return -1;
        }
    }
}

long solve(machine_t *m) {
    long a, b;
    switch (linsolve(m, &a, &b)) {
        case 0:
            return a <= 100 && b <= 100 ? 3 * a + b : 0;
        case -1:
            return 0;
    }

    // So infinitely many solutions. Ignore y - we know vectors are
    // collinear. Find the matching linear combination with minimal cost.
    long x1 = m->left.x;
    long x2 = m->right.x;
    long x = m->target.x;

    // Prefer to maximize the first button unless it costs more
    long coefs[2] = {3, 1};
    if (x1 < x2 || (x1 / x2) < 3) {
        // First btn costs more, swap and solve
        coefs[0] = 1;
        coefs[1] = 3;
        long tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    a = x / x1;
    if (a > 100)
        a = 100;
    b = (x - a * x1) / x2;
    while (true) {
        while (b <= 100 && a * x1 + b * x2 < x)
            b++;
        if (b > 100)
            return 0;
        if (a * x1 + b * x2 == x)
            return coefs[0] * a + coefs[1] * b;
        a--;
        if (a < 0)
            return 0;
    }
}

long solve2(machine_t *m) {
    long a, b;
    switch (linsolve(m, &a, &b)) {
        case 0:
            return 3 * a + b;
        case -1:
            return 0;
    }
    long x1 = m->left.x;
    long x2 = m->right.x;
    long x = m->target.x;

    long coefs[2] = {3, 1};
    if (x1 < x2 || (x1 / x2) < 3) {
        coefs[0] = 1;
        coefs[1] = 3;
        long tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    a = x / x1;
    b = (x - a * x1) / x2;
    while (true) {
        while (a * x1 + b * x2 < x)
            b++;
        if (a * x1 + b * x2 == x)
            return coefs[0] * a + coefs[1] * b;
        a--;
        if (a < 0)
            return 0;
    }
}

long part1(FILE *input) {
    machine_t *m = NULL;
    size_t count = read_input(input, &m);
    if (count == 0) {
        free(m);
        return -1;
    }

    long sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += solve(m + i);
    }

    free(m);
    return sum;
}

long part2(FILE *input) {
    machine_t *m = NULL;
    size_t count = read_input(input, &m);
    if (count == 0) {
        free(m);
        return -1;
    }

    long sum = 0;
    for (size_t i = 0; i < count; i++) {
        m[i].target.x += 10000000000000;
        m[i].target.y += 10000000000000;
        sum += solve2(m + i);
    }

    free(m);
    return sum;
}
