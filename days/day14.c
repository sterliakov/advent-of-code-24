#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct __attribute__((aligned(16))) pair_t {
    long x;
    long y;
} pair_t;

typedef struct __attribute__((aligned(32))) robot_t {
    pair_t pos;
    pair_t v;
} robot_t;

size_t read_input(FILE *input, robot_t **dest) {
    size_t count = 0;
    size_t arr_size = 2;
    *dest = malloc(arr_size * sizeof(robot_t));
    if (*dest == NULL) {
        perror("Malloc failed");
        return 0;
    }
    while (true) {
        if (count >= arr_size) {
            arr_size *= 2;
            robot_t *olddest = *dest;
            *dest = realloc(*dest, arr_size * sizeof(robot_t));
            if (*dest == NULL) {
                free(olddest);
                perror("Malloc failed");
                return 0;
            }
        }
        long x, y, vx, vy;
        if (fscanf(input, "p=%ld,%ld v=%ld,%ld\n", &x, &y, &vx, &vy) < 4) {
            break;
        }
        (*dest)[count] = (robot_t){(pair_t){x, y}, (pair_t){vx, vy}};
        count++;
    }
    return count;
}

long part1(FILE *input) {
    robot_t *m = NULL;
    size_t count = read_input(input, &m);
    if (count == 0) {
        free(m);
        return -1;
    }

    // long t = 100, w = 11, h = 7;
    const long t = 100, w = 101, h = 103;

    long quadrants[4] = {0};
    for (size_t i = 0; i < count; i++) {
        pair_t newpos
            = {.x = ((m[i].pos.x + t * m[i].v.x) % w + w) % w,
               .y = ((m[i].pos.y + t * m[i].v.y) % h + h) % h};
        if (newpos.x != w / 2 && newpos.y != h / 2) {
            quadrants
                [(newpos.x / ((w + 1) / 2)) * 2 + newpos.y / ((h + 1) / 2)]++;
        }
    }

    long total = 1;
    for (size_t i = 0; i < 4; i++) {
        total *= quadrants[i];
    }

    free(m);
    return total;
}

long part2(FILE *input) {
    robot_t *m = NULL;
    size_t count = read_input(input, &m);
    if (count == 0) {
        free(m);
        return -1;
    }

    const long w = 101, h = 103;

    int *positions = calloc(w * h, sizeof(int));
    if (positions == NULL) {
        perror("calloc failed");
        free(m);
        return -1;
    }
    long t = 1;
    for (; t < 10000; t++) {
        memset(positions, 0, sizeof(int) * w * h);
        for (size_t i = 0; i < count; i++) {
            pair_t newpos
                = {.x = ((m[i].pos.x + m[i].v.x) % w + w) % w,
                   .y = ((m[i].pos.y + m[i].v.y) % h + h) % h};
            positions[newpos.x * h + newpos.y]++;
            m[i].pos = newpos;
        }
        // A Christmas tree must have a trunk. Let's try to find it, assuming
        // that trunk takes at least 10% of total picture height.
        int ms = 0;
        for (size_t i = 0; i < (size_t)w; i++) {
            ms = 0;
            for (size_t j = 0; j < (size_t)h; j++) {
                if (positions[i * h + j])
                    ms++;
                else {
                    if (ms > 10)
                        break;
                    ms = 0;
                }
            }
            if (ms > 10)
                break;
        }
        if (ms > 10) {
            // Check visually whether it's really a tree
            for (size_t j = 0; j < (size_t)h; j++) {
                for (size_t i = 0; i < (size_t)w; i++) {
                    printf("%c", positions[i * h + j] ? '#' : '.');
                }
                printf("\n");
            }
            break;
        }
    }

    free(m);
    free(positions);
    return t;
}
